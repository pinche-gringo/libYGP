// $Id: AYear.cpp,v 1.3 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/AYear
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 7.11.2004
//COPYRIGHT   : Copyright (C) 2004, 2005, 2008

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

#include <YGP/AYear.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing AYear...\n";
   YGP::AYear y1;
   check (!y1.isDefined ());
   y1 = 2001;
   YGP::AYear past (1900);
   check (past.isDefined ());
   check (y1 > past);

   y1 =  "2000";
   check (y1.isLeapYear ());
   y1 += 4;
   check (y1.isLeapYear ());
   y1 -= 104;
   check (!y1.isLeapYear ());

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
