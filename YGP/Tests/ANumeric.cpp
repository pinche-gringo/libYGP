// $Id: ANumeric.cpp,v 1.10 2004/11/28 01:03:40 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/ANumeric
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
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

#include <clocale>

#include <iostream>

#include <YGP/ANumeric.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing ANumeric...\n";
   YGP::ANumeric num;
   check (!num.isDefined ());

   YGP::ANumeric num2 (-2000);
   check (num2.isDefined ());
   check (num2 < num);
   num2 = 0;
   check (num2 < num);
   num2 += 2000;
   YGP::ANumeric num3 (num2 + num);
   check (num3 == num2);
   num3 = num2 / num;
   check (num3 == num2);
   num = num2;
   check (num.isDefined ());
   check (num == num2);

   std::string numStr (num2.toString ());
   check (numStr.length () >= 4);

   setlocale (LC_NUMERIC, "de");                      // Activate german locale
   try {
      num = "123.456.789";
      check (num == 123456789);
      num = "-123.456.789";
      check (num == -123456789);
   }
   catch (std::invalid_argument& error) {
      std::cout << "Invalid argument: " << error.what () << '\n';
      check (!"Invalid argument!");
   }
#ifdef TEST_OUTPUT
   setlocale (LC_ALL, "");                           // Activate current locale

   std::cout << YGP::ANumeric::toString (-1) << '\n';
   std::cout << YGP::ANumeric::toString (-12) << '\n';
   std::cout << YGP::ANumeric::toString (-123) << '\n';
   std::cout << YGP::ANumeric::toString (-1234) << '\n';
   std::cout << YGP::ANumeric::toString (-12345) << '\n';
   std::cout << YGP::ANumeric::toString (-123456) << '\n';
   std::cout << YGP::ANumeric::toString (-1234567) << '\n';
   std::cout << YGP::ANumeric::toString (-12345678) << '\n';

   std::cout << YGP::ANumeric::toString (1) << '\n';
   std::cout << YGP::ANumeric::toString (12) << '\n';
   std::cout << YGP::ANumeric::toString (123) << '\n';
   std::cout << YGP::ANumeric::toString (1234) << '\n';
   std::cout << YGP::ANumeric::toString (12345) << '\n';
   std::cout << YGP::ANumeric::toString (123456) << '\n';
   std::cout << YGP::ANumeric::toString (1234567) << '\n';
   std::cout << YGP::ANumeric::toString (12345678) << '\n';
#endif

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
