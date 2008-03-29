// $Id: AttrParse.cpp,v 1.12 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/AttrParse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.12 $
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

#include <YGP/ANumeric.h>
#include <YGP/ATStamp.h>
#include <YGP/AttrParse.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing AttributeParse...\n";
   try {
      std::string file;
      YGP::ANumeric    size;
      YGP::ATimestamp  time;

      YGP::AttributeParse attrs;
      ATTRIBUTE (attrs, std::string, file, "File");
      ATTRIBUTE (attrs, YGP::ANumeric, size, "Size");
      ATTRIBUTE (attrs, YGP::ATimestamp, time, "Time");

      attrs.assignValues ("File=\"ADate.cpp\";Size=18180;Time=01012000 121005");
   }
   catch (YGP::ParseError& err) {
      ++cErrors;
      std::cerr << "Test-Error: " << err.what () << '\n';
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
