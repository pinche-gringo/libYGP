//$Id: AssParse.cpp,v 1.3 2001/10/08 23:33:41 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : AssignmentParse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 25.8.2001
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

#define DEBUG 0
#include "Trace_.h"
#include "Attribute.h"

#include "AssParse.h"


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
AssignmentParse::~AssignmentParse () {
}


/*--------------------------------------------------------------------------*/
//Purpose     : Retrieves the next (= first, at first call) node of the string.
//Returns     : Next node (empty string at end)
//Requires    : split != '\0' (operates on strings)
/*--------------------------------------------------------------------------*/
std::string AssignmentParse::getNextNode () throw (std::string) {
   std::string key (Tokenize::getNextNode (EQUALSIGN));

   if (key.empty ())
      return key;

   TRACE8 ("AssignmentParse::getNextNode () - Pos = " << actPos << "; Len = " << len);
   if (_string[actPos + len - 1] != EQUALSIGN) {
      key = "Not a valid assignment: " + key;
      throw (key);
   }

   posValue = actPos + len;
   int pos (posValue);
   char ch (_string[pos]);
   if (ch == QUOTE) {
      do {
         pos = _string.find (QUOTE, pos + 1);
         if (pos == std::string::npos) {
            key = "Invalid value for attribute: " + _string.substr (actPos + 1);
            throw key;
         }
      } while (_string[pos - 1] == ESCAPE);
      ++pos;

      if ((pos < _string.length ()) && (_string[pos] != SEPERATOR)) {
	 key = "Quoted value not followed by seperator: "
            + _string.substr (pos - 10, 20);
	 throw key;
      }
   }
   else {
      pos = _string.find (SEPERATOR, pos);
      if (pos == std::string::npos)
         pos = _string.length ();
   }

   len = pos - actPos + 1;

   TRACE3 ("AssignmentParse::getNextNode () - Final: " << getActNode ().c_str ());
   return getActNode ();
}

/*--------------------------------------------------------------------------*/
//Purpose     : Retrieves the key (name) of the attribute
//Returns     : Name of key
//Requires    : Must be called after getNextNode
/*--------------------------------------------------------------------------*/
std::string AssignmentParse::getActKey () const {
   assert (posValue != std::string::npos);

   TRACE3 ("AssignmentParse::getActKey () - "
           << _string.substr (actPos, posValue - actPos - 1).c_str ());
   return _string.substr (actPos, posValue - actPos - 1);
}

/*--------------------------------------------------------------------------*/
//Purpose     : Retrieves the value of the attribute
//Returns     : Value
//Requires    : Must be called after getNextNode
/*--------------------------------------------------------------------------*/
std::string AssignmentParse::getActValue () const {
   assert (posValue != std::string::npos);

   bool quoted (_string[posValue] == QUOTE);
   std::string ret;

   if (quoted) {
      ret = _string.substr (posValue + 1, len - 3 - posValue + actPos);
      int pos (0);
      
      // Remove all escape-characters inside the string (although only quotes
      // should be quoted)!
      while ((pos = ret.find (ESCAPE, pos)) != std::string::npos)
	 ret.replace (pos++, 1, 0, '\0');
   }
   else
      ret = _string.substr (posValue, len - posValue + actPos - 1);

   TRACE3 ("AssignmentParse::getActValue () - " << ret.c_str ());
   return ret;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Escapes all quote-characters inside a string
//Parameters  : value: String to check (and change)
/*--------------------------------------------------------------------------*/
void AssignmentParse::escapeQuotes (std::string& value) {
   int pos (-1);

   while ((pos = value.find (QUOTE, pos + 1)) == std::string::npos) {
      value.replace (pos++, 0, 1, ESCAPE);
   } // endwhile
}

/*--------------------------------------------------------------------------*/
//Purpose     : Builds an assignment out of key and value
//Parameters  : key: Name of key
//              value: Value of key
//Returns     : std::string: Created assignment
//Requires    : key is an ASCIIZ-string
/*--------------------------------------------------------------------------*/
std::string AssignmentParse::makeAssignment (const char* key, const char* value) {
   assert (key);
   assert (value);

   std::string temp (value);
   escapeQuotes (temp);
   
   std::string ret (key);
   ret += EQUALSIGN;
   ret += QUOTE;
   ret += temp;
   ret += QUOTE;
   ret += SEPERATOR;
   return ret;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Builds an assignment out of key and value
//Parameters  : key: Name of key
//              value: Value of key
//Returns     : std::string: Created assignment
//Requires    : key is an ASCIIZ-string
/*--------------------------------------------------------------------------*/
std::string AssignmentParse::makeAssignment (const char* key, const std::string& value) {
   assert (key);

   std::string ret (value);
   escapeQuotes (ret);

   ret = std::string (key) + std::string (EQUALSIGN, 1) + std::string (QUOTE, 1) + ret;
   ret += QUOTE;
   ret += SEPERATOR;
   return ret;
}
