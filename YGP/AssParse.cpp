//$Id: AssParse.cpp,v 1.19 2004/11/04 16:31:18 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : AssignmentParse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.19 $
//AUTHOR      : Markus Schwab
//CREATED     : 25.8.2001
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


#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Internal.h"
#include "YGP/Attribute.h"

#include "YGP/AssParse.h"


namespace YGP {

const char AssignmentParse::SEPARATOR = ';';
const char AssignmentParse::EQUALSIGN = '=';
const char AssignmentParse::QUOTE     = '"';
const char AssignmentParse::ESCAPE    = '\\';


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
AssignmentParse::~AssignmentParse () {
}


//-----------------------------------------------------------------------------
/// Returns the next assignment-statement. If the value of the assignment is
/// quoted, all quotes (") inside the value must be escaped with a backslash
/// (\). Those characters are removed by this function.
/// \returns \c Next node (empty string at end)
/// \throw std::string: describing error if node doesn't contain a valid
///     assignment
//-----------------------------------------------------------------------------
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
   unsigned int pos (posValue + 1);
   char ch (_string[posValue]);
   if (ch == QUOTE) {
      do {
         TRACE9 ("AssignmentParse::getNextNode () - Analyzing " << _string.substr (pos));
         if ((pos = _string.find (QUOTE, pos)) == std::string::npos) {
            key = _("Invalid value for an attribute: '%1'");
            key.replace (key.find ("%1"), 2, _string.substr (posValue));
            throw key;
         }

         if (_string[pos - 1] != ESCAPE)           // Check if quote is escaped
            break;

         // Remove all escape-characters before quotes
         _string.replace (pos - 1, 1, 0, '\0');
      } while (true);
      ++pos;

      if ((pos < _string.length ()) && (_string[pos] != SEPARATOR)) {
         key = _("Quoted value is not followed by a separator: '%1'");
         key.replace (key.find ("%1"), 2, _string.substr (pos - 10, 20));
         throw key;
      }
   }
   else
      if ((pos = _string.find (SEPARATOR, pos)) == std::string::npos)
         pos = _string.length ();

   len = pos - actPos + 1;

   TRACE3 ("AssignmentParse::getNextNode () - Final: " << getActNode ().c_str ());
   return getActNode ();
}

//-----------------------------------------------------------------------------
/// Returns the key (name) of the actual assignment.
/// \returns \c Name of key
/// \pre getNextNode must have been called already
//-----------------------------------------------------------------------------
std::string AssignmentParse::getActKey () const {
   Check1 (posValue != std::string::npos);

   TRACE3 ("AssignmentParse::getActKey () - "
           << _string.substr (actPos, posValue - actPos - 1).c_str ());
   return _string.substr (actPos, posValue - actPos - 1);
}

//-----------------------------------------------------------------------------
/// Returns the value of the actual assignment.
/// \returns \c Value
/// \pre getNextNode must have been called already
//-----------------------------------------------------------------------------
std::string AssignmentParse::getActValue () const {
   TRACE9 ("AssignmentParse::getActValue () const - Pos = " << posValue);
   Check1 (posValue != std::string::npos);

   std::string ret;
   if (_string[posValue] == QUOTE)
      ret = _string.substr (posValue + 1, len - 3 - posValue + actPos);
   else
      ret = _string.substr (posValue, len - posValue + actPos - 1);

   TRACE3 ("AssignmentParse::getActValue () const - " << ret.c_str ());
   return ret;
}

//-----------------------------------------------------------------------------
/// Escapes all quote-characters inside a string
/// \param value: String to check (and change)
//-----------------------------------------------------------------------------
void AssignmentParse::escapeQuotes (std::string& value) {
   TRACE9 ("AssignmentParse::escapeQuotes (std::string&) - " << value);
   unsigned int pos (-1U);

   while ((pos = value.find (QUOTE, pos + 1)) != std::string::npos) {
      TRACE8 ("AssignmentParse::escapeQuotes (std::string&) - Quote position " << pos);
      value.replace (pos++, 0, 1, ESCAPE);
   } // endwhile
}

//-----------------------------------------------------------------------------
/// Makes an assignment-statement with quoted value. Every quote (") inside
/// the value is escaped with a backslash (\).
/// \param key: Name of key
/// \param value: Value of key
/// \param length: Length of value; if -1 value must be zero-terminated
/// \returns \c std::string: Created assignment
/// \pre key is a valid pointer to length bytes or zero-terminated
//-----------------------------------------------------------------------------
std::string AssignmentParse::makeAssignment (const char* key, const char* value,
                                             size_t length) {
   TRACE9 ("AssignmentParse::makeAssignment (const char*, const char*) - "
           << key << " = " << value);
   Check1 (key);
   Check1 (value);

   std::string temp (value, (length == -1U) ? strlen (value) : length);
   escapeQuotes (temp);
   
   std::string ret (key);
   ret += EQUALSIGN;
   ret += QUOTE;
   ret += temp;
   ret += QUOTE;
   ret += SEPARATOR;
   return ret;
}

//-----------------------------------------------------------------------------
/// Makes an assignment-statement with quoted value. Every quote (") inside
/// the value is escaped with a backslash (\).
/// \param key: Name of key
/// \param value: Value of key
/// \returns \c std::string: Created assignment
/// \pre key is an ASCIIZ-string
//-----------------------------------------------------------------------------
std::string AssignmentParse::makeAssignment (const char* key, const std::string& value) {
   TRACE9 ("AssignmentParse::makeAssignment (const char*, const std::string&) - "
           << key << " = " << value);
   Check1 (key);

   std::string ret (value);
   escapeQuotes (ret);

   ret = std::string (key) + std::string (EQUALSIGN, 1) + std::string (QUOTE, 1) + ret;
   ret += QUOTE;
   ret += SEPARATOR;
   return ret;
}

}
