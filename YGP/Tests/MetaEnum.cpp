// $Id: MetaEnum.cpp,v 1.3 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/MetaEnum
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 6.12.2004
//COPYRIGHT   : Copyright (C) 2004, 2005, 2008

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


#include <YGP/MetaEnum.h>

#include "Test.h"


class test : public YGP::MetaEnum {
 public:
   static test* getInstance () {
      if (!instance)
	 instance = new test;
      return instance; }
   ~test () { }

 private:
   test () {
      insert (std::make_pair (1, "One"));
      insert (std::make_pair (2, "Two"));
      insert (std::make_pair (3, "Three"));
      insert (std::make_pair (10, "Ten"));
   }

   static test* instance;
};

test* test::instance (NULL);


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing MetaEnum...\n";
   test& t (*test::getInstance ());

   check (t[1] == "One");
   check (t["One"] == 1);

   check (t[2] == "Two");
   check (t["Two"] == 2);

   check (t[3] == "Three");
   check (t["Three"] == 3);

   check (t[10] == "Ten");
   check (t["Ten"] == 10);

   try {
      check (t[4].size ());
      check (!"No exception");
   }
   catch (std::out_of_range) {
      check ("Exception");
   }
   return cErrors ? 1 : 0;
}
