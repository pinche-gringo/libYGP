//$Id: Process.cpp,v 1.3 2003/02/12 22:44:58 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Process
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
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


#include "Internal.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "Check.h"
#include "Trace_.h"
#include "Internal.h"

#include "Process.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Executes a program in the background. If either the file can
//            not be found or produces an error while executing/initializing
//            (in the more or less 1st second) an describing text is thrown.
//Parameters: file: Name of file to execute
//            arguments: Array with arguments for the file (as understood by execv)
//            wait: Flag, if to wait til the program terminates
//Requires  : file a valid ASCIIZ-string
//Remarks   : The called file must follow some convention:
//              - Return 0 if OK and !0 if an error occured
//              - In case of an error the output should contain the message
/*--------------------------------------------------------------------------*/
void Process::start (const char* file, const char* const arguments[], bool wait)
   throw (std::string)
{
   errno = 0;
   string error;
   int pipes[2];

   pid_t pid (pipe (pipes) ? - 1 : fork ());
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
      if (rcwait && rc) {
         error = _("The command `%1' returned an error!\n\nOutput: %2");
         std::string output;
         char buffer[80];
         int cChar;
         while ((cChar = read (pipes[0], buffer, sizeof (buffer)))
                && (cChar != -1))
            output.append (buffer, cChar);
         if (errno == EAGAIN)
            errno = 0;

         std::string cmd (file);
         const char* const* arg = arguments;
         while (*++arg)
            cmd += (std::string (1, ' ') + std::string (*arg));

         error.replace (error.find ("%1"), 2, cmd);
         error.replace (error.find ("%2"), 2, output);
         TRACE8 ("Process::start (const char*, const char*) - Prg-error: " << error);
      }
      break; }
   }

   if (errno && !error.length ()) {
      std::string cmd (file);
      const char* const* arg = arguments;
      while (*++arg)
         cmd += (std::string (1, ' ') + std::string (*arg));

      error = (_("Error starting program `%1'!\n\nReason: %2"));
      error.replace (error.find ("%1"), 2, cmd);
      error.replace (error.find ("%2"), 2, strerror (errno));
   }
   if (error.length ())
      throw error;
 }