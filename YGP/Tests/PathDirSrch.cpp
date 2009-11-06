// $Id: PathDirSrch.cpp,v 1.13 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/PathDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.13 $
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


#include <iostream>

#include <YGP/File.h>
#include <YGP/Internal.h>
#include <YGP/PathDirSrch.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

   std::cout << "Testing PathDirectorySearch...\n";
#if SYSTEM == UNIX
   const std::string path ("..:../../XGP");
#else
   const std::string path ("..\\YGP;..\\XGP");
#endif
   YGP::PathDirectorySearch pds (path, "?GP.pc.*");
   check (pds.find (YGP::IDirectorySearch::FILE_NORMAL
                    | YGP::IDirectorySearch::FILE_READONLY));
   check (pds.next ());
   check (!pds.next ());

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
