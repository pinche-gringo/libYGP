// $Id: AByteArray.cpp,v 1.2 2002/04/27 19:05:32 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/AByteArray
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
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

#include <iostream.h>

#include <AByteArray.h>

#include <Internal.h>
#include "Test.h"


int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);     // Specify messagefile for gettext
   textdomain (PACKAGE);

   unsigned int cErrors (0);

   cout << "Testing AByteArray...\n";
   try {
      AByteArray a1; check (!a1.isDefined ());
      AByteArray a2 (""); check (a2.isDefined ());
      AByteArray a3 (std::string ("")); check (a3.isDefined ()); check (a3.empty ());
      AByteArray a4 ((const char*)NULL); check (!a4.isDefined ());

      a1 = "1234"; check (a1.isDefined ());
      a2 = "5678";
      a3.assign ("9012", 2);
      a4 += a1 + a2 + a3; check (a1.isDefined ());
      check (a4[2] == '3');
      check (a4[5] == '6');
      check (a4.length () == 10);
      check (a1 < a4);
      check (a3 >= a2);
   }
   catch (logic_error& e) {
      cerr << "Exception in AByteArray: " << e.what () << '\n';
   }

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
