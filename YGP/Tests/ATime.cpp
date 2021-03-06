// $Id: ATime.cpp,v 1.10 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/ATime
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

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing ATime...\n";
   YGP::ATime act;
   check (!act.isDefined ());
   YGP::ATime first (false);
   check (first.isDefined ());
   check (act < first);

   act = YGP::ATime::now ();
   check (act.isDefined ());
   check (act > first);

   try {
      act = "12:34:50";
      YGP::ATime now ("123450");
      check (act == now);

      act = "11:11:00";
      now.setMode (YGP::ATime::MODE_HHMM);
      now = "1111";
      check (act == now);

      act = "00:11:11";
      now.setMode (YGP::ATime::MODE_MMSS);
      now = "1111";
      check (act == now);
   }
   catch (std::exception& e) {
      std::cout << "Error: " << e.what () << '\n';
      check (!"Unhandled exception!");
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
