// $Id: XStrBuf.cpp,v 1.5 2002/12/15 22:31:53 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/XStrBuf
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

#include <fstream>
#include <iostream>

#include <gzo-cfg.h>
#include <Check.h>
#include <XStrBuf.h>

#include "Test.h"

using namespace std;

#if SYSTEM == WINDOWS
static const char* TESTFILE = "..\\Common\\Tests\\Parsertest.dat";
#else
static const char* TESTFILE = "Parsertest.dat";
#endif


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   cout << "Testing extStreambuf...\n";

   ifstream in (TESTFILE, ios::in);
   check (in);
   if (in) {
      extStreambuf str (*in.rdbuf ());
      in.ios::rdbuf (&str);
      char c;

      char buffer[20], *pAct = buffer;

      int forAlpha (0), afterAlpha (0);

      while ((in.get (c)), in) {
         if (isalpha (c)) {
            check (str.getLine () == 3);
            check (str.getColumn () == 2);
            Check3 ((pAct + 4) >= buffer);
            in.putback (c);
            in.putback (*--pAct);
            in.putback (*--pAct);
            in.putback (*--pAct);
            break;
         } // endif alpha found
         Check3 (buffer + sizeof (buffer) > pAct);
         *pAct++ = c;
         ++forAlpha;
      } // end-while

      while ((in.get (c)), in)
         ++afterAlpha;

      check ((forAlpha == 16) && (afterAlpha == 12));
      check (str.getLine () == 5);
      check (str.getColumn () == 0);
   }

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
