// $Id: Tokenize.cpp,v 1.9 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/Tokenize
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.9 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2005, 2008

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

#include <iostream>

#include <YGP/Tokenize.h>
#include <YGP/Internal.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing Tokenize...\n";
   YGP::Tokenize token ("/usr/include/std/");
   check (token.getActNode () == "/usr/include/std/");
   check (token.getNextNode ('/') == "usr");
   check (token.getNextNode ('/') == "include");
   check (token.getNextNode ('/') == "std");

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
