// $Id: PathDirSrch.cpp,v 1.5 2002/12/15 22:32:22 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/PathDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
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

#include <File.h>
#include <PathDirSrch.h>

#include <Internal.h>
#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing PathDirectorySearch...\n";
#if SYSTEM == UNIX
   PathDirectorySearch pds (".:../../X-windows", "Makefile.*");
#else
   PathDirectorySearch pds ("..\\Common;..\\X-windows", "Makefile.*");
#endif
   check (pds.find (DirectorySearch::FILE_NORMAL
                    | DirectorySearch::FILE_READONLY));
   check (pds.next ());
   check (pds.next ());
   check (pds.next ());
   check (!pds.next ());

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
