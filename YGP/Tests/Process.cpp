// $Id: Process.cpp,v 1.1 2004/10/25 02:54:05 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.10.2004
//COPYRIGHT   : Copyright (C) 2003 - 2004

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


#include <fstream>
#include <iostream>

#include <string.h>
#include <unistd.h>

#include <YGP/Process.h>

#include "Test.h"

unsigned int count (0);

const char TESTCHAR = '\xf3';


const char* argument ("Process.test");


int main (int argc, char* argv[]) {
   if (argc == 1)
      std::cout << "Testing processes ...\n";
   else {
      if (strcmp (argv[1], argument))
	 return 1;

      std::ofstream file (argv[1]);
      file << TESTCHAR;
      return 0;
   } 

   unsigned int cErrors (0);

   try {
      const char* arguments[] = { argv[0], argument, NULL };
      YGP::Process::execute (argv[0], arguments);

      std::ifstream input (argument);
      check (input);

      char ch;
      input >> ch;
      check (ch == TESTCHAR);
      unlink (argument);
   }
   catch (std::string& err) {
      std::cerr << err << '\n';
      check (0);
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
