// $Id: MetaEnum.cpp,v 1.1 2004/12/06 23:05:08 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/Handle
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2004

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
