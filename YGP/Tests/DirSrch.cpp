// $Id: DirSrch.cpp,v 1.1 2001/08/27 15:24:29 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/DirSrch
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

#include <DirSrch.h>

#include "Test.h"


int dirSearchRecursive (const char* pFile) {
   DirectorySearch ds (pFile ? "T*" : "D*");
   dirEntry file;

   if (ds.find (file, DirectorySearch::FILE_NORMAL))
      return 1;

   return pFile ? dirSearchRecursive (NULL) : 0;
}

int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   cout << "Testing DirectorySearch...\n";
   DirectorySearch ds;
   dirEntry file;
   check (!ds.find ("T*", file, IDirectorySearch::FILE_NORMAL
                                  | IDirectorySearch::FILE_READONLY));
   check (!ds.find ());

   check (!ds.find (".*", file, IDirectorySearch::FILE_DIRECTORY
                                  | DirectorySearch::FILE_HIDDEN));
   check (!ds.find ());
   check (!ds.find ());
   check (!ds.find ());
   check (ds.find ());

   check (!ds.find (".", file, DirectorySearch::FILE_DIRECTORY
                               | IDirectorySearch::FILE_HIDDEN));
   std::string temp ("..");
   temp += dirEntry::DIRSEPERATOR;
   temp += "Tests";
   check (!ds.find (temp.c_str (), file, IDirectorySearch::FILE_DIRECTORY));
   check (ds.find ());

   check (!dirSearchRecursive (NULL));

   check (ds.find ("CVS", file, IDirectorySearch::FILE_NORMAL));

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
