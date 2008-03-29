// $Id: Module.cpp,v 1.2 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
//AUTHOR      : Markus Schwab
//CREATED     : 2.6.2006
//COPYRIGHT   : Copyright (C) 2004 - 2006, 2008

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

#include <YGP/Module.h>

#include "Test.h"

int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   try {
      YGP::Module mod1 ("YGP-" LIBRELEASE);
      YGP::Module mod2 ("libYGP-" LIBRELEASE);
      YGP::Module mod3 ("libYGP-" LIBRELEASE ".so");

      check (mod1.getSymbol ("check"));
      check (mod2.getSymbol ("check"));
      check (mod3.getSymbol ("check"));
   }
   catch (YGP::FileError& e) {
      std::cerr << e.what () << '\n';
      check (0);
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
