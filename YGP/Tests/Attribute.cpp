// $Id: Attribute.cpp,v 1.10 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/Attribute
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
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

#include <YGP/ATime.h>
#include <YGP/Attribute.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing Attribute...\n";
   long lattr;
   YGP::Attribute<long> lAttr ("lAttr", lattr);
   check (lAttr.assignFromString ("1234"));
   check (lattr == 1234);
   check (!lAttr.assignFromString ("abcde"));
   check (!lAttr.assignFromString ("123z"));
   YGP::ATime time;
   YGP::Attribute<YGP::ATime> timeAttr ("timeAttr", time);
   check (timeAttr.assignFromString ("121005"));
#ifndef __CYGWIN__
   // Cygwin (at least in my version B20) seems to have problems with exceptions
   check (!timeAttr.assignFromString ("12"));
#endif

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
