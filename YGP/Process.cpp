//$Id: Process.cpp,v 1.8 2003/11/14 00:22:57 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Process
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.8 $
//AUTHOR      : Markus Schwab
//CREATED     : 04.02.2003
//COPYRIGHT   : Anticopyright (A) 2003

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


#ifdef _MSC_VER
#pragma warning(disable:4786)   // disable warning about truncating debug info
#endif


#include "YGP/Internal.h"

#include <cstdio>
#include <cerrno>
#include <cstring>

#if SYSTEM == UNIX
#  include <unistd.h>
#  include <sys/wait.h>
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
#include "YGP/Trace_.h"
#include "YGP/Internal.h"

#include "YGP/Process_.h"


//-----------------------------------------------------------------------------
/// Executes a program, either in the back- or foreground. If either the file
/// can not be found or produces an error while executing/initializing (in the
/// more or less 1st second) an describing text (at least if the program
/// produces an helpful output) is thrown.
/// \param file: Name of file to execute
/// \param arguments: Array with arguments for the file (as understood by execv)
/// \param wait: Flag, if to wait til the program terminates
/// \pre \c file is a valid ASCIIZ-string
/// \remarks The called file must follow some convention:
///    - Return 0 if OK and !0 if an error occured
///    - In case of an error the output should contain a describing message
//-----------------------------------------------------------------------------
void Process::start (const char* file, const char* const arguments[], bool wait)
   throw (std::string)
{
   errno = 0;
   std::string err;
   int pipes[2];
   pid_t pid;

#if SYSTEM == UNIX
   pid = (pipe (pipes) ? - 1 : fork ());
   switch (pid) {
   case 0: {                                            // Child: Start program
      // Close read pipe and set output to the write pipe
      close (pipes[0]);
      close (0);
      dup2 (dup (pipes[1]), 1);
      dup2 (pipes[1], 2);
      execvp (file, const_cast<char* const*> (arguments));
      perror ("");
      _exit (1);
      break; }

   case -1:
      TRACE9 ("Process::start (const char*, const char*) - Fork failed");
      break;

   default: {
      TRACE9 ("Process::start (const char*, const char*) - Fork OK");
      close (pipes[1]);

      if (!wait)
         sleep (1);

      int rc (0), rcwait;
      if ((rcwait = waitpid (pid, &rc, (wait ? 0 : WNOHANG))) == -1)
         break;

      TRACE9 ("RC = " << rc << "; wait: " << rcwait);
      if (rcwait && rc)
         err = readChildOutput (pipes[0]);
      break; }
   } // end-switch

#elif SYSTEM == WINDOWS
   if (pipe (pipes) != -1) {
      // Save original output-handles
      int sout (dup (1));
      int serr (dup (2));

      if ((sout != -1) && (serr != -1)) {
         // Duplicate write end of pipe to stdout/err and close it
         dup2 (dup (pipes[1]), 1);
         dup2 (pipes[1], 2);
         close (pipes[1]);

         pid = spawnvp (wait ? P_WAIT : P_NOWAIT, file,
                        const_cast<char* const*> (arguments));
         int serror = errno;

         // Restore stdout/err
         dup2 (sout, 1);
         dup2 (serr, 2);
         close (sout);
         close (serr);

         // Wait (or check if child has finished)
         unsigned long rc (0);
         if (wait)
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
#endif

   if (errno && !err.length ()) {
      err = (_("Error executing program `%1'!\n\nReason: %2"));
      err.replace (err.find ("%2"), 2, strerror (errno));
   }
   close (pipes[0]);

   if (err.length ()) {
      std::string cmd (file);
      const char* const* arg = arguments;
      while (*++arg)
         cmd += (std::string (1, ' ') + std::string (*arg));

      err.replace (err.find ("%1"), 2, cmd);
      throw err;
   }
 }

//-----------------------------------------------------------------------------
/// Reads the data for the passed descriptor and returns it with a
/// description.
/// \param file: Descriptor of stream to read from
/// \returns \c String with error-message
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
