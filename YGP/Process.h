#ifndef PROCESS_H
#define PROCESS_H

//$Id: Process.h,v 1.14 2006/04/10 01:45:32 markus Rel $

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

#include <ygp-cfg.h>
#if SYSTEM == UNIX
#  include <unistd.h>
#  include <sys/types.h>
#elif SYSTEM == WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

typedef int pid_t;
#endif


#include <string>


namespace YGP {

/**Class to execute a program either synchronlously or asynchronlously.

   The program is supposed to return 0, if it succeeds and any other value
   if it encounters an error. In case of an error it should display its error
   on stdout or stderr. This output is captured and thrown as exception for
   the caller.

   In case of an asynchron running program it must display its error in
   (more or less) the first second of its execution (as the controlling
   process only waits one second before checking for program termination).

   \note This class has only static members!
*/
class Process {
 public:
   enum { NO_WAIT = 0, WAIT = 1, CONNECT_STDOUT = 2, CONNECT_STDERR = 4,
	  CONNECT_STDOUT_AND_ERR = 8 };

   /// Executes a program asynchronously to the execution of the
   /// current process. If either the file can not be found or
   /// produces an error while initializing (in the more or less 1st
   /// second) an describing text (at least if the program produces an
   /// helpful output) is thrown.
   /// \param file: Name of file to execute
   /// \param arguments: Array with arguments for the file (as understood by execv)
   /// \returns pid_t: PID of created process
   /// \pre \c file is a valid ASCIIZ-string
   /// \remarks The called file must follow some convention:
   ///    - Return 0 if OK and !0 if an error occured
   ///    - In case of an error the output should contain a describing message
   static pid_t execAsync (const char* file, const char* const arguments[])
      throw (std::string) {
      return start (file, arguments, NO_WAIT); }

   /// Executes a program. The execution of the calling process is
   /// suspended, til the new program has terminated. If either the
   /// file can not be found or produces an error while executing an
   /// describing text (at least if the program produces an helpful
   /// output) is thrown.
   /// \param file: Name of file to execute
   /// \param arguments: Array with arguments for the file (as understood by execv)
   /// \pre \c file is a valid ASCIIZ-string
   /// \remarks The called file must follow some convention:
   ///    - Return 0 if OK and !0 if an error occured
   ///    - In case of an error the output should contain a describing message
   static void execute (const char* file, const char* const arguments[])
      throw (std::string) {
      start (file, arguments, WAIT); }

   /// Executes a program in the background. If either the file can not be
   /// found or produces an error while initializing (in the more or less 1st
   /// second) an describing text (at least if the program produces an helpful
   /// output) is thrown.
   /// \param file: Name of file to execute
   /// \param arguments: Array with arguments for the file (as understood by execv)
   /// \param fd: Pipes for communication
   /// \param flags: Flags describing how to connect stdin/stdout
   /// \returns pid_t: PID of created process
   /// \pre \c file is a valid ASCIIZ-string
   /// \remarks The called file must follow some convention:
   ///    - Return 0 if OK and !0 if an error occured
   ///    - In case of an error the output should contain a describing message
   static pid_t execIOConnected (const char* file, const char* const arguments[],
				 int* fd, unsigned int flags = CONNECT_STDOUT_AND_ERR)
      throw (std::string) {
      return start (file, arguments, NO_WAIT | flags, fd); }

   /// Returns the process ID of the actual process
   /// \return pid_t: Process ID of running process
   static pid_t getPID () {
#if SYSTEM == UNIX
      return getpid ();
#elif SYSTEM == WINDOWS
      return GetCurrentProcessId ();
#endif
   }

   static int waitForProcess (pid_t pid);

 protected:
   static pid_t start (const char* file, const char* const arguments[],
		       int flags, int* fd = NULL)
      throw (std::string);

 private:
   Process ();
   Process (const Process& other);
   virtual ~Process ();

   const Process& operator= (const Process& other);

   static std::string readChildOutput (int file);
};

}

#endif
