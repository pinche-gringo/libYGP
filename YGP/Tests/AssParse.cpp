// $Id: AssParse.cpp,v 1.4 2002/08/21 20:21:28 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/AssParse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
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

#include <iostream>

#include <AssParse.h>

#include "Test.h"

#define K1     "key1"
#define KV1    "12342"
#define A1     K1 "=" KV1
#define K2     "key2"
#define KV2    "a;b;"
#ifndef _MSC_VER
#define A2     K2 "=\"" KV2 "\""
#else
#define A2     K2 "=\042" KV2 "\042"
#endif
#define K3     "key3"
#ifndef _MSC_VER
#define KV3    "abcd\\\"def\""
#else
#define KV3    "abcd\\\042def\042"
#endif
#define A3     K3 "=" KV3


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing AssignmentParse ...\n";
   AssignmentParse attrs (A1 ";" A2 ";" A3);
   try {
      std::string node (attrs.getNextNode ());
      check (attrs.getActNode () == A1);
      check (attrs.getActKey () == K1);
      check (attrs.getActValue ()== KV1);

      node = attrs.getNextNode ();
      check (attrs.getActNode () == A2);
      check (attrs.getActKey () == K2);
      check (attrs.getActValue () == KV2);

      node = attrs.getNextNode ();
      check (attrs.getActNode () == A3);
      check (attrs.getActKey () == K3);
      check (attrs.getActValue () == KV3);

      check (attrs.getNextNode ().empty ());
   }
   catch (std::string& e) {
      std::cerr << "Test: Error: " << e << '\n';
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
