// $Id: FileRExp.cpp,v 1.11 2005/11/09 19:21:45 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/FileRExp
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.11 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2005

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

#include <YGP/FileRExp.h>

#include "Test.h"


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing FileRegularExpr...\n";
   YGP::FileRegularExpr regExp ("a*b");
   check (!regExp.checkIntegrity () && regExp.matches ("ab"));

   regExp = "a[a-d]b";
   check (!regExp.checkIntegrity () && regExp.matches ("adb") && !regExp.matches ("axb"));

   regExp = "a[^xa-dy]b";
   check (!regExp.checkIntegrity () && regExp.matches ("afb") && !regExp.matches ("axb"));

   regExp = "[[:alpha:]]";
   check (!regExp.checkIntegrity ());
   // Check named character classes especially; not all fnmatch versions
   // understand them
   if (!regExp.matches ("x")) {
#ifdef HAVE_FNMATCH
       std::cout << "    -> Warning: Your fnmatch does not support named character classes\n"
                    "    -> Failed (regExp.matches (\"x\"); line " << __LINE__ << ")\n" << std::flush;
#else
       ERROROUT ("regExp.matches (\"x\")");
#endif
   }
   check (!regExp.matches (":"));

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
