// $Id: XStream.cpp,v 1.5 2003/02/21 19:42:12 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/XStream
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Anticopyright (A) 2001

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

#include <ctype.h>

#include <iostream>

#include <XStream.h>

#include "Test.h"

using namespace std;

#if SYSTEM == UNIX
#  define PATH ""
#else
#  define PATH "..\\Common\\Tests\\"
#endif

int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing XStream...\n";

   Xifstream xin;
   xin.open (PATH "XStrBuf.test", std::ios::in);
   check (xin);
   if (xin) {
      char c;
      check (!xin.eof ());

      xin.init ();

      xin >> c >> c;
      check (xin.getLine () == 1);
      check (xin.getColumn () == 2);
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
