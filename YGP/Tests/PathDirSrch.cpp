// $Id: PathDirSrch.cpp,v 1.7 2003/11/14 00:22:57 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/PathDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.7 $
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

#include <YGP/File.h>
#include <YGP/Internal.h>
#include <YGP/PathDirSrch.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing PathDirectorySearch...\n";
#if SYSTEM == UNIX
   PathDirectorySearch pds (".:../../XGP", "Makefile.*");
#else
   PathDirectorySearch pds ("..\\YGP;..\\XGP", "Makefile.*");
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
