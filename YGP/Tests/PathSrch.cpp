// $Id: PathSrch.cpp,v 1.1 2001/08/27 15:24:30 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/PathSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
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

#include <gzo-cfg.h>

#include <PathSrch.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   cout << "Testing PathSearch...\n";
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
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
