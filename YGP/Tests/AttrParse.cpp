// $Id: AttrParse.cpp,v 1.7 2003/11/14 20:27:55 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/AttrParse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.7 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Anticopyright (A) 2001 - 2003

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
   catch (std::string& err) {
      std::cerr << "Test: Error " << err << '\n';
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
