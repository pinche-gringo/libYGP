// $Id: Handle.cpp,v 1.8 2004/11/28 01:04:26 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/Handle
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.8 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2004

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

#include <YGP/Trace.h>
#include <YGP/Handle.h>

#include "Test.h"


class test {
 public:
   test (const char* name = "???") : n (name) {
      TRACE1 ("test::test (const char*) - " << n); }
   ~test () { TRACE1 ("test::~test () - " << n); }

   const char* n;
};

class test2 : public test {
 public:
   test2 (const char* name = "???") : test (name) {
      TRACE1 ("test2::test2 (const char*) - " << n);
      len = strlen (name); }
   ~test2 () { TRACE1 ("test2::~test2 () - " << n); }

   unsigned int len;
};


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing SmartPtr & Handle...\n";
   definePtr (test);
   {
      Ptest pTest (new test ("SmartPtr"));
      pTest->n = "222";
   }

   defineHndl (test);
   defineHndl (test2);
   Htest hHandle;
   check (!hHandle.isDefined ());

   hHandle.define ();
   check (hHandle.isDefined ());
   hHandle->n = "Handle";
   check (strlen (hHandle->n) == 6);
   check (hHandle.isDefined ());
   hHandle.undefine ();
   check (!hHandle.isDefined ());

   Htest hHandle2 (new test ("Handle2"));
   check (hHandle2.isDefined ());
   hHandle2 = hHandle;
   check (!hHandle2.isDefined ());

   Htest2 derived;
   derived.define ();
   check (derived->len == 3);
   derived->n = "1";
   derived->len = strlen (derived->n);
   check (derived->len == 1);
   hHandle = Htest::cast (derived);
   check (strlen (hHandle->n) == 1);
   derived = Htest2::cast (hHandle);
   check (derived.ptr () == hHandle.ptr ());
   check (derived->len == 1);
   check (Htest2::cast (hHandle)->len == 1);

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
