// $Id: CRegExp.cpp,v 1.17 2003/11/17 15:07:05 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/CRegExp
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.17 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Anticopyright (A) 2001 - 2003

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

#include "Test.h"

#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/ANumeric.h>
#include <YGP/Parse.h>
#include <YGP/CRegExp.h>
#include <YGP/XStream.h>


unsigned int cErrors (0);
std::string strRE, strVal;
YGP::RegularExpression regexp ("");
bool match;


int foundRegExp (const char* pRegExp, unsigned int) {
   Check1 (pRegExp);
   TRACE1 ("Found regular expression: " << pRegExp);
   strRE = pRegExp;
   regexp = strRE.c_str ();
   return YGP::ParseObject::PARSE_OK;
   }

int foundValue (const char* pValue, unsigned int) {
   Check1 (pValue);
   Check1 (strRE.size ());
   TRACE1 ("Found value: " << pValue);
   strVal = pValue;
   match = regexp.matches (pValue);
   return YGP::ParseObject::PARSE_OK;
}

int foundResult (const char* pResult, unsigned int) {
   Check1 (pResult);
   PRINT (strRE << " matches " << strVal << " == " << pResult << '\n');
   if ((*pResult != '0') != match)
      ERROROUT ("RegExp (\"" << strRE << "\").matches (\"" << strVal
                << "\") == " << pResult);

   return YGP::ParseObject::PARSE_OK;
}


#if SYSTEM == WINDOWS
static const char* TESTFILE = "..\\Common\\Tests\\CRegExp.test";
#else
static const char* TESTFILE = "CRegExp.test";
#endif

int main (int argc, char* argv[]) {
   std::cout << "Testing RegularExpression...\n";

   try {
      if (argc > 1) { // If a parameter is passed, treat it as regexp to check
         regexp = argv[1];

         const char* pMatch = ((argc > 2) ? argv[2] : "");
         int result ((argc > 3) ? (*argv[3] - '0') : 1);
         PRINT (argv[1] << " matches " << pMatch << " == " << result << '\n');
         int rc (regexp.matches (pMatch));
         if (rc != result)
            ERROROUT ("RegExp (\"" << argv[1] << "\").matches (\""
                      << pMatch << "\") == " << result);
         return !rc;
      }
   }
   catch (std::string& e) {
      std::cerr << e.c_str () << '\n';
   }

   YGP::Xifstream frexexp;
   try {
      frexexp.open (TESTFILE, std::ios::in);
      check (frexexp);
      if (frexexp) {
         frexexp.init ();

         YGP::ParseObject*   _RegExpTest[3];
         YGP::ParseSelection RegExpTest (_RegExpTest, "Regular expression-file", -1U, 0);
         YGP::ParseObject*   _RegExpHeader[4];
         YGP::ParseSequence  RegExpHeader (_RegExpHeader, "Regexp-header", 1, 1);
         YGP::ParseObject*   _values[4];
         YGP::ParseSequence  values (_values, "Values", 1, 1);
         YGP::CBParseTextEsc RegExp ("]", "Regular expression", &foundRegExp, 512, 1, '\\', false);
         YGP::CBParseTextEsc value ("=\n\r", "Value", &foundValue, 512, 1, false);
         YGP::CBParseAttomic result ("\\9", "Result", &foundResult, 1, 1);
         YGP::ParseExact RegExpBegin ("[", "Begin of regexp ([)", false);
         YGP::ParseExact RegExpEnd ("]", "End of regexp (])");
         YGP::ParseExact equals ("=", "Equal-sign (=)", false);

         _RegExpTest[0] = &RegExpHeader; _RegExpTest[1] = &values;
         _RegExpTest[2] = NULL;
         _RegExpHeader[0] = &RegExpBegin; _RegExpHeader[1] = &RegExp;
         _RegExpHeader[2] = &RegExpEnd; _RegExpHeader[3] = NULL;
         _values[0] = &value; _values[1] = &equals; _values[2] = &result;
	 _values[3] = NULL;

         RegExpTest.parse ((YGP::Xistream&)frexexp);
      } // endif
   } // end-try
   catch (std::string& e) {
      std::cerr << "Error parsing '" << TESTFILE << ": " << e.c_str () << "\nActual position: "
                << YGP::ANumeric (frexexp.getLine ()).toString () << '/'
                << YGP::ANumeric (frexexp.getColumn ()).toString () << '\n';
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
