// $Id: INIFile.cpp,v 1.3 2002/05/01 00:43:29 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/INIFile
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Anticopyright (A) 2001

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

#include <iostream.h>

#include <ADate.h>
#include <ATStamp.h>
#include <INIFile.h>
#include <XStream.h>
#include <ANumeric.h>

#include <Internal.h>
#include "Test.h"


int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);     // Specify messagefile for gettext
   textdomain (PACKAGE);

   unsigned int cErrors (0);

   cout << "Testing INI-file parser...\n";
   int Attr1;
   std::string Attr2;
   ANumeric Attr3;
   ADate Attr4;
   Xifstream inifile;
   ATime Attr5;
   ATimestamp Attr6;
   try {
      INISection global ("Global");
      Attribute<int> attr1 ("Attr1", Attr1);
      Attribute<std::string> attr2 ("Attr2", Attr2);
      Attribute<ANumeric> attr3 ("Attr3", Attr3);
      Attribute<ADate> attr4 ("Attr4", Attr4);
      global.addAttribute (attr1);
      global.addAttribute (attr2);
      global.addAttribute (attr3);
      global.addAttribute (attr4);

      inifile.open ("INIFile.test", ios::in | ios::nocreate);
      check (inifile);

      if (inifile) {
         INIFILE ("INIFile.test");
         INISECTION (Special);
         INIATTR (Special, ATime, Attr5);
         INIATTR (Special, ATimestamp, Attr6);

         INISECTION (Local);
         INIATTR (Local, int, Attr1);
         std::string attr2_;
         INIATTR2 (Local, std::string, attr2_, Attr2);
         INIATTR (Local, ANumeric, Attr3);
         INIATTR (Local, ADate, Attr4);

         _inifile_.addSection (global);

         inifile.init ();
         try {
            int rc = global.readFromStream ((Xistream&)inifile);
            check (!rc);
         } // end-try
         catch (std::string& e) {
            throw (std::string ("Error parsing INIFile.test in line ")
                   + ANumeric (inifile.getLine ()).toString ()
                   + std::string (" (") + ANumeric (inifile.getColumn ()).toString ()
                   + std::string ("): ") + e);
         } // end-catch

         int rc = INIFILE_READ ();
         check (!rc);
      } // endif
   } // end-try
   catch (std::string& e) {
      cerr << e.c_str () << '\n';
   }

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
