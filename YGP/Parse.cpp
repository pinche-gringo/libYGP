//$Id: Parse.cpp,v 1.1 1999/08/24 00:07:43 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Parse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.8.1999
//COPYRIGHT   : Anticopyright (A) 1999

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

#include <ctype.h>

#include "Parse.h"
#include "XStream.h"


#ifndef MULTIBUFFER
std::string ParseAttomic::buffer;
#endif

ostream& ParseObject::error = cerr;


static char ESCAPE = '\\';


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : description: Description of object (what it parses)
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : description != NULL
/*--------------------------------------------------------------------------*/
ParseObject::ParseObject (const char* description, const PFNCALLBACK callback,
			  bool skipWhitespace)
   : pDescription (description), pCallback (callback), skip (skipWhitespace) {
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseObject::ParseObject (const ParseObject& other)
   : pDescription (other.pDescription), pCallback (other.pCallback)
   , skip (other.skip) {
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseObject::~ParseObject () {
}


/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
const ParseObject& ParseObject::operator= (const ParseObject& other) {
   if (&other != this) {
      pDescription = other.pDescription;
      pCallback = other.pCallback;
      skip = other.skip;
   } // endif other object

   assert (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Skips the whitespaces (blank and tab) at the actual stream-pos
//Parameters  : stream: Source from which to read
/*--------------------------------------------------------------------------*/
void ParseObject::skipWS (Xistream& stream) const {
   char c;
   while (!stream.eof ()) {
      stream >> c;
      if (!isspace (c)) {
         stream.putback (c);
	 break;
      } // endif non-whitespace found
   } // end-while !EOF
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the constraints of the object
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseObject::checkIntegrity () const {
   return !pDescription;
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : value: List of valid characters
//              description: Description of object (what it parses)
//              max: Maximal cardinality
//              min: Minimal cardinality
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : description != NULL
/*--------------------------------------------------------------------------*/
ParseAttomic::ParseAttomic (const char* value, const char* description,
                            unsigned int max, unsigned int min,
                            PFNCALLBACK callback, bool skipWhitespace)
   : ParseObject (description, callback, skipWhitespace), pValue (value)
   , maxCard (max), minCard (min) {
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseAttomic::ParseAttomic (const ParseAttomic& other)
   : ParseObject ((const ParseObject&)other), pValue (other.pValue) {
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseAttomic::~ParseAttomic () {
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
const ParseAttomic& ParseAttomic::operator= (const ParseAttomic& other) {
   if (&other != this) {
      ParseObject::operator= ((const ParseObject&)other);
      pValue = other.pValue;
      maxCard = other.maxCard;
      minCard = other.minCard;
   } // endif other object

   assert (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Tries to parse the object from the stream
//Parameters  : stream: Source from which to read
//              optional: Flag, if node must be found
/*--------------------------------------------------------------------------*/
int ParseAttomic::doParse (Xistream& stream, bool optional) const {
   assert (!checkIntegrity ());

#ifdef MULTIBUFFER
   std::string buffer;
#endif
   char ch;
   unsigned int i (0);
   while (i <= maxCard) {                    // While not max. card is reached
      stream >> ch;

      if (!checkValue (ch))                        // Read and check next char
         break;

      buffer[i++] = ch;                                        // Store, if OK
   } // end-while !maximal cardinality

   int rc (PARSE_OK);
   if ((i >= minCard) && (i <= maxCard)) {                // Cardinalities OK?
      if (pCallback)
         rc = pCallback (buffer.c_str ());        // Execute callback (if set)
   } // endif value OK
   else
      rc = PARSE_ERROR;

   if (rc && optional) {
      while (i)
         stream.putback (buffer[--i]);

      buffer[10] = '\0';
      error << "Parse-error in line " << stream.getLine () << " column "
            << stream.getColumn () << ":\nExpected: " << getDescription ()
            << " found: " << buffer << '\n';
   } // end-if mandatory value not found

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
bool ParseAttomic::checkValue (char ch) const {
   assert (!checkIntegrity ());

   const char* pHelp = pValue; assert (pHelp);
   while (*pHelp) {
      if (*pHelp == ESCAPE) {
         switch (*++pHelp) {
         case '9': if (isdigit (ch)) return true;
            break;

         case 'X': if (isdigit (ch)) return true;
         case 'A': if (isalpha (ch)) return true;
            break;

         case ' ': if (isspace (ch)) return true;
            break;

         case '*': return true;
            break;

         default:
            if (ch == *pHelp)
               return true;
         } // end-switch
      } // endif ESCAPE-char in pValue
      else
         if (*pHelp == ch)
            return true;

      ++pHelp;
   } // end-while valid chars left

   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the constraints of the object
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseAttomic::checkIntegrity () const {
   return pValue ? maxCard < minCard ? 10 : ParseObject::checkIntegrity () : 11;
}
