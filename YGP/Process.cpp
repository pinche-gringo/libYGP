//$Id: Process.cpp,v 1.24 2008/06/10 21:47:20 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Process
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.24 $
//AUTHOR      : Markus Schwab
//CREATED     : 04.02.2003
//COPYRIGHT   : Copyright (C) 2003 - 2006, 2008

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#ifdef _MSC_VER
#pragma warning(disable:4786)   // disable warning about truncating debug info
#endif


#include "YGP/Internal.h"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>

#if SYSTEM == UNIX
#  if !defined HAVE_PIPE && defined HAVE__PIPE
#    define pipe(p)  _pipe((p), 256, 0)
#  endif

#  ifdef HAVE_SYS_WAIT_H
#     include <sys/wait.h>
#   endif

#  if defined HAVE_WINDOWS_H
#     include <windows.h>                // For HANDLE and GetExitCodeProcess
#     include <cstdlib>                                          // For sleep

#     if defined HAVE__SLEEP
#        define sleep      _sleep
#     endif
#  endif

#elif SYSTEM == WINDOWS
#  include <io.h>
#  include <fcntl.h>
#  include <process.h>

#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

#  define close    _close
#  ifdef _MSC_VER
#     define dup      _dup
#     define dup2     _dup2
#  endif
#  define pipe(p)  _pipe((p), 256, O_TEXT | O_NOINHERIT)
#  define read     _read
#  define sleep(s) Sleep ((s) * 1000)
#  define write    _write

#endif

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Internal.h"

#include "YGP/Process.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Executes a program, either in the back- or foreground. If either the file
/// can not be found or produces an error while executing/initializing (in the
/// more or less 1st second) an describing text (at least if the program
/// produces an helpful output) is thrown.
/// \param file Name of file to execute
/// \param arguments Array with arguments for the file (as understood by execv)
/// \param flags Flags; e.g. if to wait til the program terminates
/// \param fd Filedescriptors for communication with the child.
///     - fd[0] contains the filedescriptor the child should read its data from;
///             will be overwritten with the descriptor the output from the
///             child can be read from (if flags & CONNECT_STDOUT or
///             flags & CONNECT_STDOUT_AND_ERR)
///     - fd[1] is expected to contain the matching file descriptor for the
///             pipe for the communication (which the child can close)
///     - fd[2] will be filled with the descriptor from which the errors of the
///             child can be read from (if flags & CONNECT_STDERR)
///     filled with the filedescriptor for the output
/// \pre \c file is a valid ASCIIZ-string
/// \returns pid_t PID of created process
/// \throw YGP::ExecError An error-message displayed by the program
/// \remarks The called file must follow some convention:
///    - Return 0 if OK and !0 if an error occured
///    - In case of an error the output should contain a describing message
//-----------------------------------------------------------------------------
pid_t Process::start (const char* file, const char* const arguments[],
		      int flags, int* fd) throw (YGP::ExecError)
{
   TRACE9 ("Process::start (const char*, const char*, int, int*) - " << file);
   Check1 (file);
   Check1 ((flags & CONNECT_STDOUT_AND_ERR) ? (!(flags & (CONNECT_STDOUT | CONNECT_STDERR))) : 1);

   errno = 0;
   std::string err;
   pid_t pid (0);
   int pipes[2];
   int errPipes[2];

#if defined HAVE_SPAWNVP && HAVE_WINDOWS_H
   if ((pipe (pipes) != -1)
       && ((flags & CONNECT_STDOUT_AND_ERR) && pipe (errPipes) != -1)) {
      // Save original output-handles
      int sin (0);
      if (fd) {
	 sin = dup (0);
	 dup2 (fd[0], 0);
	 dup2 (dup (pipes[1]), fd[0]);
      }

      int sout (dup (1));
      int serr (dup (2));

      if ((sout != -1) && (serr != -1)) {
         // Duplicate write end of pipe to stdout/err and close it
         dup2 (pipes[0], 0);
	 if (flags & CONNECT_STDOUT_AND_ERR) {
	    dup2 (dup (pipes[1]), 1);
	    dup2 (errPipes[1], 2);
	 }
	 else {
	    if (flags & CONNECT_STDOUT)
	       dup2 (dup (pipes[1]), 1);
	    if (flags & CONNECT_STDERR)
	       dup2 (pipes[1], 2);
	 }

         pid = spawnvp ((flags & WAIT) ? P_WAIT : P_NOWAIT, file,
                        const_cast<char* const*> (arguments));

         // Restore stdout/err
	 if (fd) {
	    dup2 (sin, 0);
	    close (sin);
	 }

         dup2 (sout, 1);
         dup2 (serr, 2);
         close (sout);
         close (serr);

         // Wait (or check if child has finished)
         unsigned long rc (0);
         if ((flags & WAIT))
            GetExitCodeProcess ((HANDLE)pid, &rc);
         else {
            sleep (1);
            // Assume that process has finished, if GetProcessVersions returns 0
            HANDLE hProc (OpenProcess (PROCESS_QUERY_INFORMATION, false, pid));
            if (hProc) {
               GetExitCodeProcess (hProc, &rc);
               if (rc)
                  err = readChildOutput (pipes[0]);
            }
         }
      }
   }
#elif defined HAVE_FORK
   if (((pid = pipe (pipes)) != -1)
       && ((flags & CONNECT_STDERR) ? ((pid = pipe (errPipes)) != -1) : true)) {
      pid = fork ();
      TRACE9 ("Errpipes: " << errPipes[0] << '/' << errPipes[1]);
   }
   switch (pid) {
   case 0: {                                            // Child: Start program
      // Close input pipe and set output to the write pipe
      close (pipes[0]);
      if (fd) {
	 close (fd[1]);
	 dup2 (fd[0], STDIN_FILENO);
	 close (fd[0]);
      }

      if (flags & CONNECT_STDOUT_AND_ERR) {
	 dup2 (pipes[1], STDOUT_FILENO);
	 dup2 (pipes[1], STDERR_FILENO);
      }
      else {
	 if (flags & CONNECT_STDOUT)
	    dup2 (pipes[1], STDOUT_FILENO);
	 if (flags & CONNECT_STDERR) {
	    dup2 (errPipes[1], STDERR_FILENO);
	    close (errPipes[1]);
	 }
      }
      close (pipes[1]);

      errno = 0;
      int rc (execvp (file, const_cast<char* const*> (arguments)));
      if (!errno)
	 errno = rc;
      perror ("");
      exit (1);
      break; }

   case -1:
      TRACE9 ("Process::start (const char*, const char*) - Fork failed");
      break;

   default: {
      TRACE9 ("Process::start (const char*, const char*) - Fork OK - " << pid);
      if (fd) {
	 dup2 (pipes[0], fd[0]);
	 if (flags & CONNECT_STDERR) {
	    fd[2] = errPipes[0];
	    close (errPipes[1]);
	 }
      }
      close (pipes[1]);

      if (!((flags & WAIT) || fd))
         sleep (1);

      int rc (0), rcwait (0);
      if ((rcwait = waitpid (pid, &rc, ((flags & WAIT) ? 0 : WNOHANG))) == -1)
         break;

      TRACE9 ("RC = " << rc << "; wait: " << rcwait);
      if (rcwait && rc)
         err = readChildOutput (pipes[0]);
      break; }
   } // end-switch
#else
#  error Not yet implemented!
#endif
   if (!fd)
      close (pipes[0]);

   if (errno && err.empty ()) {
      err = (_("Error executing program `%1'!\n\nReason: %2"));
      err.replace (err.find ("%2"), 2, strerror (errno));
   }

   if (err.length ()) {
      std::string cmd (file);
      const char* const* arg = arguments;
      while (*++arg)
         cmd += (std::string (1, ' ') + std::string (*arg));

      err.replace (err.find ("%1"), 2, cmd);
      throw (YGP::ExecError (err));
   }

   return pid;
 }

//-----------------------------------------------------------------------------
/// Reads the data for the passed descriptor and returns it with a
/// description.
/// \param file Descriptor of stream to read from
/// \returns std::string String with error-message
//-----------------------------------------------------------------------------
std::string Process::readChildOutput (int file) {
   std::string err (_("The command `%1' returned an error!\n\nOutput: %2"));
   std::string output;
   char buffer[80];
   int cChar;
   while ((cChar = read (file, buffer, sizeof (buffer)))
            && (cChar != -1))
      output.append (buffer, cChar);
   if (errno == EAGAIN)
      errno = 0;

   err.replace (err.find ("%2"), 2, output);
   TRACE8 ("Process::readChildOutput (const char*, const char*) - Prg-error: " << err);
   return err;
}

//-----------------------------------------------------------------------------
/// Wait til process has terminated
/// \param pid Proess to wait for
/// \returns int Returncode of the program
//-----------------------------------------------------------------------------
int Process::waitForProcess (pid_t pid) {
#if defined HAVE_SPAWNVP && HAVE_WINDOWS_H
   unsigned long rc (-1UL);
   GetExitCodeProcess ((HANDLE)pid, &rc);
   return (int)rc;
#elif defined HAVE_FORK
   int rc (-1);
   waitpid (pid, &rc, 0);
   return WIFEXITED (rc) ? WEXITSTATUS (rc) : -1;
#endif
}


}
