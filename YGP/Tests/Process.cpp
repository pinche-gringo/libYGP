// $Id: Process.cpp,v 1.3 2005/01/12 22:11:57 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.10.2004
//COPYRIGHT   : Copyright (C) 2003 - 2005

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


#include <unistd.h>

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
   char buffer[40] = "";

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
	    std::cout << "OK";
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

      int pipes[2];
      int rc (pipe (pipes));
      check (!rc);

      arguments[1] = argIOConnected;
      YGP::Process::execIOConnected (argv[0], arguments, pipes);

      check (write (pipes[1], argIOConnected, strlen (argIOConnected)) != -1);
      check (close (pipes[1]) != -1);
      check (!*buffer);
      check (read (pipes[0], buffer, sizeof (buffer)) != -1);
      close (pipes[0]);
      close (pipes[1]);
      check (!strcmp (buffer, "OK"));
   }
   catch (std::string& err) {
      std::cerr << err << '\n';
      check (0);
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
