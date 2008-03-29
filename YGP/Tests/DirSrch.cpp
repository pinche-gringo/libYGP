// $Id: DirSrch.cpp,v 1.20 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/DirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.20 $
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


#include <ygp-cfg.h>

#include <iostream>

#include <YGP/File.h>
#include <YGP/DirSrch.h>

#define VERBOSE
#undef  VERBOSE
#include "Test.h"

#define NAME    "DirSrch.cpp"

#if SYSTEM == UNIX
#  define PATH
#  define PATHBACK   "../"
#else
#  define PATH       "..\\YGP\\Tests\\"
#  define PATHBACK   "..\\YGP\\"
#endif


int dirSearchRecursive (const char* pFile) {
   YGP::DirectorySearch ds (pFile ? PATH "T*" : PATH "D*");

   if (!ds.find (YGP::DirectorySearch::FILE_NORMAL))
      return 1;

   return pFile ? dirSearchRecursive (NULL) : 0;
}

int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing File...\n";
   try {
      YGP::File test (PATH NAME);
      check (!strcmp (test.name (), NAME));
      check (*test.path () == '.');
#if SYSTEM == UNIX
      check (test.path ()[1] == YGP::File::DIRSEPARATOR);
      check (test.path ()[2] == '\0');
#endif

      test = PATHBACK NAME;
      check (!strcmp (test.name (), NAME));
      check (!strcmp (test.path (), PATHBACK));
   }
   catch (YGP::FileError& err) {
      std::cerr << err.what () << '\n';
   }

   std::cout << "Testing DirectorySearch...\n";
   YGP::DirectorySearch ds;
   check (ds.find (PATH "T*", YGP::IDirectorySearch::FILE_NORMAL
                              | YGP::IDirectorySearch::FILE_READONLY));
   check (ds.next ());

   check (ds.find (PATH ".*", YGP::IDirectorySearch::FILE_DIRECTORY
                              | YGP::IDirectorySearch::FILE_HIDDEN));
   check (ds.next ());                 // Don't perform test for no more files!

   check (ds.find (".", YGP::IDirectorySearch::FILE_DIRECTORY
                        | YGP::IDirectorySearch::FILE_HIDDEN));
   std::string temp (PATH "..");
   temp += YGP::File::DIRSEPARATOR;
   temp += "Tests";
   check (ds.find (temp.c_str (), YGP::IDirectorySearch::FILE_DIRECTORY));
   check (!ds.next ());

   check (!dirSearchRecursive (NULL));
   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
