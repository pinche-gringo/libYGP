// $Id: PathDirSrch.cpp,v 1.1 2001/08/27 15:24:30 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/PathDirSrch
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

#include <PathDirSrch.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   cout << "Testing PathDirectorySearch...\n";
   dirEntry file;
#if SYSTEM == UNIX
   PathDirectorySearch pds (".:../../X-windows", "Makefile.*");
#else
   PathDirectorySearch pds (".;..\\..\\X-windows", "Makefile.*");
#endif
   check (!pds.find (file, DirectorySearch::FILE_NORMAL
                           | DirectorySearch::FILE_READONLY));
   check (!pds.find ());
   check (!pds.find ());
   check (!pds.find ());
   check (pds.find ());

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
