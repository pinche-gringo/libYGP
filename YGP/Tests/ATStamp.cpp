// $Id: ATStamp.cpp,v 1.2 2002/04/27 19:05:32 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/ATStamp
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

#include <ATStamp.h>

#include <Internal.h>
#include "Test.h"


int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);     // Specify messagefile for gettext
   textdomain (PACKAGE);

   unsigned int cErrors (0);

   cout << "Testing ATimestamp...\n";
   ATimestamp now;
   check (!now.isDefined ());
   ATimestamp early (false);
   check (early.isDefined ());
   check (now < early);

   now = ATimestamp::now ();
   check (now.isDefined ());
   check (now > early);

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
