//$Id: AssParse.cpp,v 1.6 2002/04/09 20:02:50 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : AssignmentParse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 25.8.2001
//COPYRIGHT   : Anticopyright (A) 2001, 2002

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
#include "Internal.h"
#include "Attribute.h"

#include "AssParse.h"


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
AssignmentParse::~AssignmentParse () {
}


/*--------------------------------------------------------------------------*/
//Purpose     : Retrieves the next (= first, at first call) node of the string.
//              All escaped quotes are changed back (-> (\") is changed to ("))
//Returns     : Next node (empty string at end)
//Requires    : split != '\0' (operates on strings)
/*--------------------------------------------------------------------------*/
std::string AssignmentParse::getNextNode () throw (std::string) {
   std::string key (Tokenize::getNextNode (EQUALSIGN));

   if (key.empty ())
      return key;

   TRACE8 ("AssignmentParse::getNextNode () - Pos = " << actPos << "; Len = " << len);
   if (_string[actPos + len - 1] != EQUALSIGN) {
      std::string error (_("Not a valid assignment: '%1'"));
      error.replace (error.find ("%1"), 2, key);
      throw (error);
   }

   posValue = actPos + len;
   int pos (posValue);
   char ch (_string[pos]);
   if (ch == QUOTE) {
      do {
         pos = _string.find (QUOTE, pos + 1);

         if (pos == std::string::npos) {
            key = _("Invalid value for attribute: '%1'");
            key.replace (key.find ("%1"), 2, _string.substr (actPos + 1));
            throw key;
         }

         if (_string[pos - 1] != ESCAPE)           // Check if quote is escaped
            break;

         // Remove all escape-characters before quotes
         _string.replace (pos++ - 1, 1, 0, '\0');
      } while (true);
      ++pos;

      if ((pos < _string.length ()) && (_string[pos] != SEPERATOR)) {
         key = _("Quoted value not followed by seperator: '%1'");
         key.replace (key.find ("%1"), 2, _string.substr (pos - 10, 20));
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
   TRACE9 ("AssignmentParse::getActValue () const - Pos = " << posValue);
   assert (posValue != std::string::npos);

   bool quoted (_string[posValue] == QUOTE);
   std::string ret;

   if (quoted)
      ret = _string.substr (posValue + 1, len - 3 - posValue + actPos);
   else
      ret = _string.substr (posValue, len - posValue + actPos - 1);

   TRACE3 ("AssignmentParse::getActValue () const - " << ret.c_str ());
   return ret;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Escapes all quote-characters inside a string
//Parameters  : value: String to check (and change)
/*--------------------------------------------------------------------------*/
void AssignmentParse::escapeQuotes (std::string& value) {
   TRACE9 ("AssignmentParse::escapeQuotes (std::string&) - " << value);
   int pos (-1);

   while ((pos = value.find (QUOTE, pos + 1)) != std::string::npos) {
      TRACE8 ("AssignmentParse::escapeQuotes (std::string&) - Quote position " << pos);
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
std::string AssignmentParse::makeAssignment (const char* key, const char* value,
                                             size_t length) {
   TRACE9 ("AssignmentParse::makeAssignment (const char*, const char*) - "
           << key << " = " << value);
   assert (key);
   assert (value);

   std::string temp (value, length);
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
std::string AssignmentParse::makeAssignment (const char* key, const std::string& value,
                                             size_t length) {
   TRACE9 ("AssignmentParse::makeAssignment (const char*, const std::string&) - "
           << key << " = " << value);
   assert (key);

   std::string ret (value, length);
   escapeQuotes (ret);

   ret = std::string (key) + std::string (EQUALSIGN, 1) + std::string (QUOTE, 1) + ret;
   ret += QUOTE;
   ret += SEPERATOR;
   return ret;
}
