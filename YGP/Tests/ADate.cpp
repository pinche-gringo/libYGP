// $Id: ADate.cpp,v 1.3 2002/08/21 20:21:28 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/ADate
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
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

#include <ADate.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing ADate...\n";
   ADate today;
   check (!today.isDefined ());
   ADate past (false);
   check (past.isDefined ());
   check (today < past);

   today = ADate::today ();
   check (today.isDefined ());
   check (today > past);

   today.sub (0, 1);
   check (today < ADate::today ());
   today.add (0, 1);
   check (today == ADate::today ());
   today.sub (0, 1);
   today -= ADate::today ();                 // Results in 0.11.-1 => 31.10.-1

   today.setMonth (1);
   today.sub (0, 2);
   check (today.getMonth () == 10);

   today.add (0, 3);
   check (today.getMonth () == 1);

   today.setYear (2000);
   check (today.isLeapYear ());
   today.add (0, 0, 1);
   check (!today.isLeapYear ());

   check (!past.isLeapYear ());
   past.add (0, 0, 4);
   check (past.isLeapYear ());
   
   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
