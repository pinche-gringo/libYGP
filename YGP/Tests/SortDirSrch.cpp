// $Id: SortDirSrch.cpp,v 1.1 2005/03/07 22:33:51 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/SortDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 5.3.2005
//COPYRIGHT   : Copyright (C) 2005

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

#include <ygp-cfg.h>

#include <iostream>

#include <YGP/File.h>
#include <YGP/SortDirSrch.h>

#define VERBOSE
#undef  VERBOSE
#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing SortedDirSearch ...\n";
   YGP::SortedDirSearch<YGP::DirectorySearch> ds ("*");

   std::string lastName;
   const YGP::File* result (ds.find ());
   while (result) {
      check (lastName.compare (result->name ()) < 0);
      lastName = result->name ();
      result = ds.next ();
   } // endwhile

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
