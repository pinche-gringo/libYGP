// $Id: Entity.cpp,v 1.1 2005/01/08 22:08:27 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/Entity
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 8.1.2005
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


#include <cstdio>
#include <unistd.h>

#include <string>
#include <fstream>

#include <YGP/AYear.h>

#include <YGP/Entity.h>

#include "Test.h"


class test : public YGP::Entity {
 public:
   test () : nr (0) {
      addAttribute (*new YGP::Attribute<std::string> ("Text", text));
      addAttribute (*new YGP::Attribute<YGP::AYear> ("Year", year));
      addAttribute (*new YGP::Attribute<int> ("Number", nr));
   }

   std::string text;
   YGP::AYear  year;
   int         nr;
};


int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing Entity...\n";
   std::string tmp (tmpnam (NULL)); check (tmp.size ());
   std::ofstream tmpFile (tmp.c_str ());
   std::ifstream input (tmp.c_str ());
   check (tmpFile); check (input);
   test obj1;
   obj1.text ="TEXT";
   obj1.year = 2005;
   obj1.nr = 4711;

   tmpFile << obj1;
   tmpFile.close ();

   test obj2;
   input >> obj2;
   check (obj1.text == obj2.text);
   check (obj1.year == obj2.year);
   check (obj1.nr == obj2.nr);

   check (1);
   int rc (unlink (tmp.c_str ())); check (!rc);
   return cErrors ? 1 : 0;
}
