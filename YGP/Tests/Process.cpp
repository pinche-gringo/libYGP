// $Id: Process.cpp,v 1.9 2006/03/18 03:16:08 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.9 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.10.2004
//COPYRIGHT   : Copyright (C) 2004 - 2006

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
#  if !defined HAVE_PIPE && defined HAVE__PIPE
#    define pipe(p)  _pipe((p), 256, 0)
#  endif
#elif SYSTEM == WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

#  define pipe(p)  _pipe((p), 256, O_TEXT | O_NOINHERIT)
#endif

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>

#include <YGP/Process.h>

#include "Test.h"

unsigned int count (0);

const char  TESTCHAR = '\xf3';


const char* argFile ("Process.test");
const char* argIOConnected ("--io");


int main (int argc, char* argv[]) {
   char buffer[8] = "";

   if (argc == 1)
      std::cout << "Testing processes ...\n";
   else {
      if (!strcmp (argv[1], argFile)) {
	 std::ofstream file (argv[1]);
	 file << TESTCHAR;
	 return 0;
      }
      else if (!strcmp (argv[1], argIOConnected)) {
	 std::cin.read (buffer, sizeof (buffer));

	 if (strcmp (buffer, argIOConnected)) {
	    std::cout << "Invalid input: '" << buffer << "'!\n";
	    return 2;
	 }
	 else {
	    std::cout << "O"; std::cout.flush ();
	    std::cerr << "K"; std::cerr.flush ();
	    return 3;
	 }
      }
      std::cout << "Invalid parameter!";
      return 1;
   }

   unsigned int cErrors (0);
   try {
      const char* arguments[] = { argv[0], argFile, NULL };
      YGP::Process::execute (argv[0], arguments);

      std::ifstream input (argFile);
      check (input);

      char ch;
      input >> ch;
      check (ch == TESTCHAR);
      unlink (argFile);

      int pipes[3];
      check (!pipe (pipes));

      arguments[1] = argIOConnected;
      pid_t pid (YGP::Process::execIOConnected (argv[0], arguments, pipes));

      check (write (pipes[1], argIOConnected, strlen (argIOConnected)) != -1);
      check (close (pipes[1]) != -1);
      check (!*buffer);
      unsigned int r (read (pipes[0], buffer, sizeof (buffer)));
      check ((r == 1) || (r == 2));
      check (*buffer == 'O');
      if (r == 1)
	 check ((r = read (pipes[0], buffer + 1, sizeof (buffer) - 1)) == 1);
      check (buffer[1] == 'K');

      check (YGP::Process::waitForProcess (pid) == 3);
      close (pipes[0]);
      close (pipes[1]);

      check (!pipe (pipes));
      pid = YGP::Process::execIOConnected (argv[0], arguments, pipes,
					   YGP::Process::CONNECT_STDOUT
					   | YGP::Process::CONNECT_STDERR);

      check (write (pipes[1], argIOConnected, strlen (argIOConnected)) != -1);
      check (close (pipes[1]) != -1);
      check (read (pipes[0], buffer + 2, sizeof (buffer) - 2) == 1);
      check (buffer[2] == 'O');
      check (read (pipes[2], buffer + 3, sizeof (buffer) - 3) == 1);
      check (buffer[3] == 'K');

      check (YGP::Process::waitForProcess (pid) == 3);
      close (pipes[0]);
      close (pipes[1]);
   }
   catch (std::string& err) {
      std::cerr << err << '\n';
      check (0);
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
