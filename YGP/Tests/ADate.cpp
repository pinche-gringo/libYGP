// $Id: ADate.cpp,v 1.12 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/ADate
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.12 $
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

#include <YGP/ADate.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing ADate...\n";
   YGP::ADate today;
   check (!today.isDefined ());
   YGP::ADate past (false);
   check (past.isDefined ());
   check (today < past);

   today = YGP::ADate::today ();
   check (today.isDefined ());
   check (today > past);

   today.sub (0, 1);
   check (today < YGP::ADate::today ());
   today.add (0, 1);
   check (today.getMonth () == YGP::ADate::today ().getMonth ());
   today.sub (0, 1);
   today -= YGP::ADate::today ();            // Results in 0.11.-1 => 31.10.-1

   today.setDay (25);
   today.setMonth (1);
   today.sub (0, 2);
   check (today.getMonth () == 11);

   today.add (0, 3);
   check (today.getMonth () == 2);

   today.setYear (2000);
   check (today.isLeapYear ());
   today.add (0, 0, 1);
   check (!today.isLeapYear ());

   check (!past.isLeapYear ());
   past.add (0, 0, 4);
   check (past.isLeapYear ());

   std::string out (past.toString ());
   check (out.size () == 8);

   today = "12112005";
   check (today.getDay () == 12);
   check (today.getMonth () == 11);
   check (today.getYear () == 2005);

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
