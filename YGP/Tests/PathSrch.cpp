// $Id: PathSrch.cpp,v 1.5 2003/11/14 00:22:57 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/PathSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Anticopyright (A) 2001 - 2003

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

#include <gzo-cfg.h>

#include <YGP/PathSrch.h>
#include <YGP/Internal.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing PathSearch...\n";
#if SYSTEM == UNIX
   PathSearch ps (".:..:/::/usr/:/usr");
   check (ps.getActNode () == ".:..:/::/usr/:/usr");
   check (ps.getNextNode () == ".");
   check (ps.getNextNode () == "..");
   check (ps.getNextNode () == "/");
   check (ps.getNextNode () == "/usr/");
   check (ps.getNextNode () == "/usr");
#else
   PathSearch ps (".;..;\\;;\\usr\\;\\usr");
   check (ps.getActNode () == ".;..;\\;;\\usr\\;\\usr");
   check (ps.getNextNode () == ".");
   check (ps.getNextNode () == "..");
   check (ps.getNextNode () == "\\");
   check (ps.getNextNode () == "\\usr\\");
   check (ps.getNextNode () == "\\usr");
#endif

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
