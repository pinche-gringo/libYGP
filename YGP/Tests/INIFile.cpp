// $Id: INIFile.cpp,v 1.17 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/INIFile
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.17 $
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

#include <cstdio>

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <YGP/ADate.h>
#include <YGP/ATStamp.h>
#include <YGP/INIFile.h>
#include <YGP/XStream.h>
#include <YGP/ANumeric.h>
#include <YGP/Internal.h>

#include "Test.h"

#if SYSTEM == UNIX
#  define PATH
#else
#  define PATH "..\\YGP\\Tests\\"
#endif

int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   std::cout << "Testing INI-file parser...\n";
   try {
      int Attr1;
      std::string Attr2;
      YGP::ANumeric Attr3;
      YGP::ADate Attr4;
      YGP::ATime Attr5;
      YGP::ATimestamp Attr6;
      std::vector<std::string> list;
      std::map<std::string, std::string> map;

      YGP::INISection global ("Global");
      YGP::Attribute<int> attr1 ("Attr1", Attr1);
      YGP::Attribute<std::string> attr2 ("Attr2", Attr2);
      YGP::Attribute<YGP::ANumeric> attr3 ("Attr3", Attr3);
      YGP::Attribute<YGP::ADate> attr4 ("Attr4", Attr4);
      global.addAttribute (attr1);
      global.addAttribute (attr2);
      global.addAttribute (attr3);
      global.addAttribute (attr4);

      INIFILE (PATH "INIFile.test");
      INISECTION (Special);
      INIATTR (Special, YGP::ATime, Attr5);
      INIATTR (Special, YGP::ATimestamp, Attr6);

      INILIST2 (List, std::string, list);
      INIMAP2  (Map, std::string, map);

      INISECTION (Local);
      INIATTR (Local, int, Attr1);
      std::string attr2_;
      INIATTR2 (Local, std::string, attr2_, Attr2);
      INIATTR (Local, YGP::ANumeric, Attr3);
      INIATTR (Local, YGP::ADate, Attr4);

      try {
         int rc = global.readFromStream ((YGP::Xistream&)_inifile_.getFile ());
         check (!rc);
      } // end-try
      catch (std::exception& e) {
         throw (std::string ("Error parsing INIFile.test in line ")
                + YGP::ANumeric (_inifile_.getFile ().getLine ()).toString ()
                + std::string (" (")
                + YGP::ANumeric (_inifile_.getFile ().getColumn ()).toString ()
                + std::string ("): ") + e.what ());
      } // end-catch

      int rc = INIFILE_READ ();
      check (!rc);

      check (list[0] == "Zero");
      check (list[1] == "One");
      check (list[2] == "Two");
      check (list[3] == "Three");
      check (map["A"] == "First letter");
      check (map["B"] == "Second letter");
      check (map["Z"] == "Last letter");
   } // end-try
   catch (std::exception& e) {
      ++cErrors;
      std::cerr << e.what () << '\n';
   }

   std::cout << "Testing INI-file writer ...\n";
   try {
      char output[] = "[Global]\n"
	 "Key=\"Value with space\"     ; Global value\n"
	 "ID=234\n\n"
	 "[Local]                    ; Local section\n"
	 "Key=Value                  ; Local value\n"
	 "ID=123\n";

      std::string file (tmpnam (NULL));
      std::ofstream ostr (file.c_str (), std::ios::binary | std::ios::out);
      ostr << output;
      ostr.close ();

      std::string Key ("Value_*no*_space");
      int ID (321);
      INIFILE (file.c_str ());
      INISECTION (Global);
      INIATTR (Global, std::string, Key);
      INISECTION (Local);
      INIATTR (Local, int, ID);

      INIFILE_WRITE ();

      std::string input;
      std::ifstream istr (file.c_str ());
      while (istr) {
	 char buffer[80];
	 istr.read (buffer, sizeof (buffer));
	 input += std::string (buffer, istr.gcount ());
      }

      // Update the output to match the changes
      memcpy (output + 19, "_*no*_", 6);
      memcpy (output + sizeof (output) - 5, "321", 3);

      std::cout << "Output:\n" << output << "\n\n" << "Input:\n" << input;

      check (input == output);
      unlink (file.c_str ());
   }
   catch (std::exception& e) {
      ++cErrors;
      std::cerr << e.what () << '\n';
   }


   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
