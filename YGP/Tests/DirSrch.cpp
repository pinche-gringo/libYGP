// $Id: DirSrch.cpp,v 1.10 2003/02/14 04:15:15 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/DirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
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

#include <gzo-cfg.h>

#include <iostream>

#include <File.h>
#include <DirSrch.h>

#define VERBOSE
#undef  VERBOSE
#include "Test.h"

#define NAME    "DirSrch.cpp"

#if SYSTEM == UNIX
#  define PATH
#  define PATHBACK   "../"
#else
#  define PATH       "..\\Common\\Tests\\"
#  define PATHBACK   "..\\Common\\"
#endif


int dirSearchRecursive (const char* pFile) {
   DirectorySearch ds (pFile ? PATH "T*" : PATH "D*");

   if (!ds.find (DirectorySearch::FILE_NORMAL))
      return 1;

   return pFile ? dirSearchRecursive (NULL) : 0;
}

int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing File...\n";
   try {
      File test (PATH NAME);
      check (!strcmp (test.name (), NAME));
      check (*test.path () == '.');
#if SYSTEM == UNIX
      check (test.path ()[1] == File::DIRSEPARATOR);
      check (test.path ()[2] == '\0');
#endif

      test = PATHBACK NAME;
      check (!strcmp (test.name (), NAME));
      check (!strcmp (test.path (), PATHBACK));
   }
   catch (const char* err) {
      std::cerr << err << '\n';
   }

   std::cout << "Testing DirectorySearch...\n";
   DirectorySearch ds;
   check (ds.find (PATH "T*", IDirectorySearch::FILE_NORMAL
                              | IDirectorySearch::FILE_READONLY));
   check (ds.next ());

   check (ds.find (PATH ".*", IDirectorySearch::FILE_DIRECTORY
                              | DirectorySearch::FILE_HIDDEN));
   check (ds.next ());                 // Don't perform test for no more files!

   check (ds.find (".", DirectorySearch::FILE_DIRECTORY
                        | IDirectorySearch::FILE_HIDDEN));
   std::string temp (PATH "..");
   temp += File::DIRSEPARATOR;
   temp += "Tests";
   check (ds.find (temp.c_str (), IDirectorySearch::FILE_DIRECTORY));
   check (!ds.next ());

   check (!dirSearchRecursive (NULL));
   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
