// $Id: DirSrch.cpp,v 1.3 2002/04/27 19:05:32 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/DirSrch
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

#include <iostream.h>

#include <File.h>
#include <DirSrch.h>

#define VERBOSE
#undef  VERBOSE
#include <Internal.h>
#include "Test.h"


int dirSearchRecursive (const char* pFile) {
   DirectorySearch ds (pFile ? "T*" : "D*");

   if (!ds.find (DirectorySearch::FILE_NORMAL))
      return 1;

   return pFile ? dirSearchRecursive (NULL) : 0;
}

int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);     // Specify messagefile for gettext
   textdomain (PACKAGE);

   unsigned int cErrors (0);

   cout << "Testing DirectorySearch...\n";
   DirectorySearch ds;
   check (ds.find ("T*", IDirectorySearch::FILE_NORMAL
                         | IDirectorySearch::FILE_READONLY));
   check (ds.next ());

   check (ds.find (".*", IDirectorySearch::FILE_DIRECTORY
                         | DirectorySearch::FILE_HIDDEN));
   check (ds.next ());
   check (ds.next ());
   check (ds.next ());
   check (!ds.next ());

   check (ds.find (".", DirectorySearch::FILE_DIRECTORY
                        | IDirectorySearch::FILE_HIDDEN));
   std::string temp ("..");
   temp += File::DIRSEPERATOR;
   temp += "Tests";
   check (ds.find (temp.c_str (), IDirectorySearch::FILE_DIRECTORY));
   check (!ds.next ());

   check (!dirSearchRecursive (NULL));

   check (!ds.find ("CVS", IDirectorySearch::FILE_NORMAL));

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
