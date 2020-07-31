// $Id$

//PROJECT     : libYGP
//SUBSYSTEM   : Test/FileTypeChk
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision$
//AUTHOR      : Markus Schwab
//CREATED     : 29.7.2008
//COPYRIGHT   : Copyright (C) 2008 - 2020

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


#include <string>
#include <iostream>

#include <YGP/File.h>
#include <YGP/DirSrch.h>
#include <YGP/FileTypeChk.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing FileTypeChecker...\n";
   YGP::FileTypeCheckerByExtension chkExt;
   YGP::FileTypeCheckerByContent chkCont;

   const YGP::File* file;
   YGP::DirectorySearch ds ("FileTypes/*");

   YGP::FileTypeChecker::FileType aTypes[] = {
      YGP::FileTypeChecker::OOXML, YGP::FileTypeChecker::OPENOFFICE, YGP::FileTypeChecker::HTML,
      YGP::FileTypeChecker::MSOFFICE, YGP::FileTypeChecker::ABIWORD, YGP::FileTypeChecker::MP3,
      YGP::FileTypeChecker::MSOFFICE, YGP::FileTypeChecker::STAROFFICE, YGP::FileTypeChecker::PNG,
      YGP::FileTypeChecker::RTF, YGP::FileTypeChecker::PDF, YGP::FileTypeChecker::OGG,
      YGP::FileTypeChecker::GIF, YGP::FileTypeChecker::JPEG, YGP::FileTypeChecker::JPEG };

   if ((file = ds.find ())) {
      unsigned int offset (0);
      do {
	 std::string name (file->path ());
	 name += file->name ();

	 check (chkExt.getType (name.c_str ())  == aTypes[offset]);
	 check (chkCont.getType (name.c_str ()) == aTypes[offset]);
	 ++offset;
      } while ((file = ds.next ()));
   }
   else
      std::cout << "    -> Warning: No files to check found!\n" << std::flush;

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
