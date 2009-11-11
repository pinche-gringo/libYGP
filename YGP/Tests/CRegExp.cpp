// $Id: CRegExp.cpp,v 1.23 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/CRegExp
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.23 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2005, 2008, 2009

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

#include <iostream>

#include "Test.h"

#include <YGP/Trace.h>
#include <YGP/ANumeric.h>
#include <YGP/Parse.h>
#include <YGP/CRegExp.h>
#include <YGP/XStream.h>


unsigned int cErrors (0);
std::string strRE, strVal;
YGP::RegularExpression regexp ("a");
bool match;


int foundRegExp (const char* pRegExp, unsigned int) {
   check (pRegExp);
   TRACE1 ("Found regular expression: " << pRegExp);
   strRE = pRegExp;
   regexp = strRE.c_str ();
   return YGP::ParseObject::PARSE_OK;
   }

int foundValue (const char* pValue, unsigned int) {
   check (pValue);
   check (strRE.size ());
   TRACE1 ("Found value: " << pValue);
   strVal = pValue;
   match = regexp.matches (pValue);
   return YGP::ParseObject::PARSE_OK;
}

int foundResult (const char* pResult, unsigned int) {
   check (pResult);
   PRINT (strRE << " matches " << strVal << " == " << pResult << '\n');
   if ((*pResult != '0') != match)
      ERROROUT ("RegExp (\"" << strRE << "\").matches (\"" << strVal
                << "\") == " << pResult);

   return YGP::ParseObject::PARSE_OK;
}


#if SYSTEM == WINDOWS
static const char* TESTFILE = "..\\YGP\\Tests\\CRegExp.test";
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
   catch (std::invalid_argument& e) {
      std::cerr << e.what () << '\n';
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

   if (cErrors) {
#ifdef HAVE_REGEX_H
     std::cout << "    -> Information: Your system's implementation of regular expressions differs\n"
	       << "       from the glibc-implementation these test base upon. Either ignore the\n"
	       << "       error(s) or compile without having HAVE_REGEX_H defined\n";
     cErrors = 0;
#else
      std::cout << "Failures: " << cErrors << '\n';
#endif
   }
   return cErrors ? 1 : 0;
}
