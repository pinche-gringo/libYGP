// $Id: ANumeric.cpp,v 1.2 2002/04/27 19:05:32 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/ANumeric
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

#include <ANumeric.h>

#include <Internal.h>
#include "Test.h"


int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);     // Specify messagefile for gettext
   textdomain (PACKAGE);

   unsigned int cErrors (0);

   cout << "Testing ANumeric...\n";
   ANumeric num;
   check (!num.isDefined ());

   ANumeric num2 (-2000);
   check (num2.isDefined ());
   check (num2 < num);
   num2 = 0;
   check (num2 > num);
   num2 += 2000;
   ANumeric num3 (num2 + num);
   check (num3 == num2);
   num3 = num2 / num;
   check (num3 == num2);
   num = num2;
   check (num.isDefined ());
   check (num == num2);

   std::string numStr (num2.toString ());
   check (numStr.length () >= 4);

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
