// $Id: CRegExp.cpp,v 1.1 2001/08/27 15:24:29 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/CRegExp
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
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

#include <assert.h>

#include <iostream.h>

#include <Trace_.h>

#include <Parse.h>
#include <CRegExp.h>
#include <XStream.h>
#include <ANumeric.h>

#include "Test.h"


unsigned int cErrors (0);
std::string strRE, strVal;
RegularExpression regexp ("");
bool match;


void removeEscapeChar (std::string& str, char esc = '\\') {
   int pos (0);
   while ((pos = str.find (esc, pos)) != std::string::npos)
      str.replace (pos++, 1, 0, '\0');
}

int foundRegExp (const char* pRegExp) {
   assert (pRegExp);
   TRACE1 ("Found regular expression: " << pRegExp);
   strRE = pRegExp;
   removeEscapeChar (strRE);
   regexp = strRE.c_str ();
   return ParseObject::PARSE_OK;
   }

int foundValue (const char* pValue) {
   assert (pValue);
   check (!strRE.empty ());
   TRACE1 ("Found value: " << pValue);
   strVal = pValue;
   removeEscapeChar (strVal);
   match = regexp.matches (pValue);
   return ParseObject::PARSE_OK;
}

int foundResult (const char* pResult) {
   assert (pResult);
   if ((*pResult != '0') != match)
      ERROROUT ("RegExp (\"" << strRE.c_str () << "\").matches (\""
		<< strVal.c_str () << "\") = " << pResult);

   return ParseObject::PARSE_OK;
}

int main (int argc, char* argv[]) {
   cout << "Testing RegularExpression...\n";
   Xifstream frexexp;
   frexexp.open ("CRegExp.test", ios::in | ios::nocreate);
   check (frexexp);
   if (frexexp) {
      frexexp.init ();

      try {
         ParseObject*   _RegExpTest[3];
         ParseSelection RegExpTest (_RegExpTest, "Regular expression-file", -1, 0);
         ParseObject*   _RegExpHeader[4];
         ParseSequence  RegExpHeader (_RegExpHeader, "Regexp-header", 1, 1);
         ParseObject*   _values[4];
         ParseSequence  values (_values, "Values", 1, 1);
         CBParseTextEsc RegExp ("]", "Regular expression", &foundRegExp, 512, 1, '\\', false);
         CBParseTextEsc value ("=\n\r", "Value", &foundValue, 512, 1, false);
         CBParseAttomic result ("\\9", "Result", &foundResult, 1, 1);
         ParseExact RegExpBegin ("[", "Begin of regexp ([)", false);
         ParseExact RegExpEnd ("]", "End of regexp (])");
         ParseExact equals ("=", "Equal-sign (=)", false);

         _RegExpTest[0] = &RegExpHeader; _RegExpTest[1] = &values;
         _RegExpTest[2] = NULL;
         _RegExpHeader[0] = &RegExpBegin; _RegExpHeader[1] = &RegExp;
         _RegExpHeader[2] = &RegExpEnd; _RegExpHeader[3] = NULL;
         _values[0] = &value; _values[1] = &equals; _values[2] = &result;
	 _values[3] = NULL;

         RegExpTest.parse ((Xistream&)frexexp);
      }
      catch (std::string& e) {
	 cerr << e.c_str () << "\nActual position: Test.rex - "
              << ANumeric (frexexp.getLine ()).toString ().c_str () << '/'
              << ANumeric (frexexp.getColumn ()).toString ().c_str () << '\n';
      }
   } // endif

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
