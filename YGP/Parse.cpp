//$Id: Parse.cpp,v 1.8 1999/10/25 17:59:30 Markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Parse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.8 $
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
#include <string.h>

#define DEBUG 0
#include "Trace.h"
#include "Parse.h"
#include "XStream.h"


// Structure for global values: Contains the (growing-if-neccessary buffer for
// parsing (In case you wonder why I didn't use std::string and care about
// growing and clean-up by myself? Well, for some reason I got a segmentation
// fault during prg-cleanup. Don't ask me why)
typedef struct globVars {
   unsigned int buflen;
   char*        buffer;

   globVars () : buflen (50), buffer (new char [buflen]) { assert (buffer); }
   ~globVars () { delete [] buffer; }
} globVars;


#ifndef MULTIBUFFER
static globVars global;

void ParseAttomic::freeBuffer () {
   delete [] global.buffer; global.buffer = NULL; global.buflen = 0;
}
#endif

_IO_ostream_withassign& ParseObject::error = cerr;


static char ESCAPE = '\\';


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : description: Description of object (what it parses)
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : description != NULL
/*--------------------------------------------------------------------------*/
ParseObject::ParseObject (const char* description, const PARSECALLBACK callback,
			  bool skipWhitespace)
   : pDescription (description), pCallback (callback), skip (skipWhitespace) {
   TRACE9 ("Creating ParseObject " << pDescription);
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseObject::ParseObject (const ParseObject& other)
   : pDescription (other.pDescription), pCallback (other.pCallback)
   , skip (other.skip) {
   TRACE9 ("Copying ParseObject " << pDescription);
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseObject::~ParseObject () {
   TRACE9 ("Deleting ParseObject " << pDescription);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
const ParseObject& ParseObject::operator= (const ParseObject& other) {
   TRACE8 ("Assigning ParseObject " << pDescription);
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
//Remarks     : Only skip whitespaces, if param skip is true
/*--------------------------------------------------------------------------*/
void ParseObject::skipWS (Xistream& stream) const {
   if (!skip)
      return;

   TRACE8 ("Skipping WS after " << pDescription);
   char c;
   stream >> c; assert (!isspace (c));
   stream.putback (c);
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the constraints of the object
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseObject::checkIntegrity () const {
   return pDescription ? OK : NO_DESCRIPTION;
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : value: List of valid characters
//              description: Description of object (what it parses)
//              max: Maximal cardinality
//              min: Minimal cardinality
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : value != NULL && !ParseObject::checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseAttomic::ParseAttomic (const char* value, const char* description,
                            unsigned int max, unsigned int min,
                            PARSECALLBACK callback, bool skipWhitespace)
   : ParseObject (description, callback, skipWhitespace)
   , pValue (value), maxCard (max), minCard (min) {
   TRACE9 ("Creating ParseAttomic " << getDescription ());
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseAttomic::ParseAttomic (const ParseAttomic& other)
   : ParseObject ((const ParseObject&)other), pValue (other.pValue) {
   TRACE9 ("Copying ParseAttomic " << getDescription ());
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseAttomic::~ParseAttomic () {
   TRACE9 ("Deleting ParseAttomic " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
const ParseAttomic& ParseAttomic::operator= (const ParseAttomic& other) {
   TRACE8 ("Assigning ParseAttomic " << getDescription ());

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
int ParseAttomic::doParse (Xistream& stream, bool optional) {
   TRACE8 ("ParseAttomic::doParse -> " << getDescription ());
   assert (!checkIntegrity ());

#ifdef MULTIBUFFER
   globVars buf;
#endif

   char* pAkt = global.buffer;
   int   ch;

   unsigned int i (0);
   while (i < maxCard) {                    // While not max. card is reached
      ch = stream.get ();
      TRACE6 ("ParseAttomic::doParse -> " << getDescription () << " -> " << (char)ch);

      if (ch == EOF)
         break;

      if (!checkValue (ch)) {                      // Read and check next char
         stream.putback (ch);
         break;
      }

      if (i == global.buflen) {                        // Buffer already full?
         pAkt = global.buffer;                // Resize to double  buffer-size
         delete global.buffer;
         global.buffer = new char [global.buflen <<= 1];
         memcpy (global.buffer, pAkt, i);
         pAkt += i;
      } // endif old buffer full

      *pAkt++ = ch;                                            // Store, if OK
      ++i;
   } // end-while !maximal cardinality
   *pAkt = '\0';
   TRACE2 ("ParseAttomic::doParse -> " << getDescription () << ": Final = '"
           << global.buffer << '\'');

   int rc (PARSE_OK);
   if (i >= minCard) {                                    // Cardinalities OK?
      TRACE6 ("ParseAttomic::doParse -> " << getDescription () << ": Found '"
              << global.buffer << '\'');
      if (pCallback)
         rc = pCallback (global.buffer);           // Execute callback (if set)
   } // endif value OK
   else
      rc = PARSE_ERROR;

   if (rc) {
      if (optional || (rc < 0))
         while (pAkt > global.buffer)
            stream.putback (*--pAkt);
      if (!optional || (rc < 0)) {
         global.buffer[10] = '\0';
         error << "Expected '" << getDescription ()
               << "'; found: " << global.buffer << (char)ch << '\n';
      } // end-if mandatory value not found
   } // endif error
   else
      skipWS (stream);

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. Valid
//              are every characters specified in pValue. If pValue contains
//              a backslash (\) the next character has a special meaning:
//                - 9: ch is valid if it is a digit
//                - A: ch is valid if it is alphabetic
//                - X: ch is valid if it is alphanumeric
//                - *: ch is valid.
//                - Else: ch is valid if it equal to this char
//Parameters  : ch: Char to check
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
bool ParseAttomic::checkValue (char ch) {
   TRACE8 ("ParseAttomic::checkValue " << getDescription () << ' ' << ch);
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
   return (pValue ? (maxCard < minCard
                     ? MAX_MIN_ERROR : ParseObject::checkIntegrity ())
           : NO_VALUE);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. This
//              is true if ch doesn't match any character in the list.
//Parameters  : ch: Char to check
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
bool ParseText::checkValue (char ch) {
   TRACE8 ("ParseText::checkValue " << getDescription () << ' ' << ch);

   const char* pHelp = pValue; assert (pHelp);
   while (*pHelp) {
      if (*pHelp++ == ch)
	 return false;
   } // endwhile chars available

   return true;
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : abort: List of valid characters
//              description: Description of object (what it parses)
//              max: Maximal cardinality
//              min: Minimal cardinality
//              escape: Character which escapes chars in value
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : value != NULL && !ParseObject::checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseTextEsc::ParseTextEsc (const char* abort, const char* description,
                            unsigned int max, unsigned min, char escape,
                            PARSECALLBACK callback, bool skipWhitespace)
   : ParseText (abort, description, max, min, callback, skipWhitespace)
   , esc (escape), last (!escape) {
   TRACE9 ("Creating ParseTextEsc " << getDescription ());
};

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseTextEsc::ParseTextEsc (const ParseTextEsc& other)
   : ParseText (other), esc (other.esc), last (!other.esc) {
   TRACE9 ("Copying ParseTextEsc " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. This
//              is true if ch doesn't match any character in the list (except
//              if it is escaped by the passed escape-character
//Parameters  : ch: Char to check
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
bool ParseTextEsc::checkValue (char ch) {
   TRACE8 ("ParseTextEsc::checkValue " << getDescription () << ' ' << ch);

   const char* pHelp = pValue; assert (pHelp);
   while (*pHelp) {
      if ((*pHelp == ch) && (last != esc)) {
         last = !esc;
	 return false;
      } // endif

      ++pHelp;
      last = ch;
   } // endwhile chars available

   return true;
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : value: List of valid characters
//              description: Description of object (what it parses)
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : value != NULL && !ParseObject::checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseExact::ParseExact (const char* value, const char* description,
                  PARSECALLBACK callback, bool skipWhitespace)
   : ParseAttomic (value, description, 1, 1, callback, skipWhitespace)
   , pos (0) {
   TRACE9 ("Creating ParseExact " << getDescription ());
   unsigned int len (strlen (value));      // value !NULL is checked by parent
   minCard = len;
   maxCard = len;
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
const ParseExact& ParseExact::operator= (const ParseExact& other) {
   TRACE8 ("Assigning ParseExact " << getDescription ());

   if (&other != this) {
      ParseAttomic::operator= ((const ParseAttomic&)other);
      pos = 0;
   } // endif other object

   assert (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. This
//              is true, if
//Parameters  : ch: Char to check
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
bool ParseExact::checkValue (char ch) {
   TRACE8 ("ParseExact::checkValue " << getDescription () << ' ' << ch);
   if (pValue[pos++] == ch) {   // Valid if ch == act-char; if wrong reset pos
      if (pos >= maxCard)           // Reset position after successfull serach
         pos = 0;

      return true;
   }

   pos = 0;
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the constraints of the object
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseExact::checkIntegrity () const {
   return pos > (strlen (pValue) + 1) ? POS_ERROR : ParseAttomic::checkIntegrity ();
}


/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. This
//              is true if  ch matches the char in pValue in the same position
//              (not case-sensitive)
//Parameters  : ch: Char to check
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
bool ParseUpperExact::checkValue (char ch) {
   TRACE8 ("ParseUpperExact::checkValue " << getDescription () << ' ' << ch);
   return ParseExact::checkValue ((char)toupper (ch));
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the constraints of the object
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseUpperExact::checkIntegrity () const {
   for (unsigned int i (0); i < maxCard; ++i)
      if (pValue[i] != toupper (pValue[i]))
	 return VALUE_NOT_UPPERCASE;

   return ParseExact::checkIntegrity ();
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : pObjectList: NULL-terminated array of objects to parse
//              description: Description of object (what it parses)
//              max: Maximal cardinality
//              min: Minimal cardinality
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : apObjectList != NULL && !ParseObject::checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseSequence::ParseSequence (ParseObject* apObjectList[],
                              const char* description, unsigned int max,
                              unsigned min, PARSECALLBACK callback,
                              bool skipWhitespace)
   : ParseObject (description, callback, skipWhitespace)
   , ppList (apObjectList), maxCard (max), minCard (min) {
   TRACE9 ("Creating ParseSequence " << getDescription ());
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseSequence::ParseSequence (const ParseSequence& other)
   : ParseObject ((const ParseObject&)other), ppList (other.ppList) {
   TRACE9 ("Copying ParseSequence " << getDescription ());
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseSequence::~ParseSequence () {
   TRACE9 ("Deleting ParseSequence " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
const ParseSequence& ParseSequence::operator= (const ParseSequence& other) {
   TRACE8 ("Assigning ParseSequence " << getDescription ());

   if (&other != this) {
      ParseObject::operator= ((const ParseObject&)other);
      ppList = other.ppList;
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
int ParseSequence::doParse (Xistream& stream, bool optional) {
   TRACE8 ("ParseSequence::doParse -> " << getDescription ());
   assert (!checkIntegrity ());

   unsigned int i (0);
   int rc (PARSE_OK);

   while (i++ < maxCard) {
      ParseObject** ppAct (ppList); assert (ppAct); assert (*ppAct);

      while (*ppAct != NULL) {                  // While list contains objects
         if ((rc = (**ppAct).doParse (stream,  // Parse (putback first always)
                                      ppAct == ppList ? true : optional)) != 0)
            break;
         ++ppAct;
      } // end-while list-entries

      if (rc) {   // Simple error while parsing: Check if in begin of sequence
         TRACE8 ("ParseSequence::doParse -> " << getDescription ()
                 << " exiting with rc = " << rc);

         // Reset error if not hard (<0) and the error happened parsing the
         // first element (but only if mincard is fullfilled)
         if ((rc > 0) && (ppAct == ppList) && (i > minCard))
            rc = PARSE_OK;
         break;
      } // endif error occured
   } // end-while i < maxCard

   if (!rc && pCallback)         // Perform callback with sequence-description
      rc = pCallback (getDescription ());

   if ((rc < 0) || (rc && !optional))
      writeError (stream);

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the constraints of the object
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseSequence::checkIntegrity () const {
   return ((ppList && *ppList)
           ? maxCard < minCard ? MAX_MIN_ERROR : ParseObject::checkIntegrity ()
	   : INVALID_LIST);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : pObjectList: NULL-terminated array of objects to parse
//              description: Description of object (what it parses)
//              max: Maximal cardinality
//              min: Minimal cardinality
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : apObjectList != NULL && !ParseObject::checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseSelection::ParseSelection (ParseObject* apObjectList[],
                                const char* description, unsigned int max,
                                unsigned min, PARSECALLBACK callback,
                                bool skipWhitespace)
   : ParseSequence (apObjectList, description, max, min, callback,
                    skipWhitespace) {
   TRACE9 ("Creating ParseSelection " << getDescription ());
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseSelection::ParseSelection (const ParseSelection& other)
   : ParseSequence ((const ParseSequence&)other) {
   TRACE9 ("Copying ParseSelection " << getDescription ());
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseSelection::~ParseSelection () {
   TRACE9 ("Deleting ParseSelection " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
const ParseSelection& ParseSelection::operator= (const ParseSelection& other) {
   TRACE8 ("Assigning ParseSelection " << getDescription ());

   if (&other != this)
      ParseSequence::operator= ((const ParseSequence&)other);

   assert (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Tries to parse the object from the stream
//Parameters  : stream: Source from which to read
//              optional: Flag, if node must be found
/*--------------------------------------------------------------------------*/
int ParseSelection::doParse (Xistream& stream, bool optional) {
   TRACE8 ("ParseSelection::doParse -> " << getDescription ());
   assert (!checkIntegrity ());

   unsigned int i (0);
   int rc (PARSE_OK);

   while (i++ < maxCard) {
      ParseObject** ppAct (ppList); assert (ppAct); assert (*ppAct);

      while (*ppAct != NULL) {                  // While list contains objects
         if ((rc = (**ppAct).doParse (stream,        // Parse (putback always)
                                      (ppAct + 1) == NULL ? optional : true))
             == 0)                                     // Break if match found
{        TRACE8 ("ParseSelection::doParse -> " << getDescription ()
                 << " exiting with rc = " << rc);
            break;
}
         ++ppAct;
      } // end-while list-entries

      if (*ppAct == NULL) {             // Does no entry of the selection fit?
         rc = PARSE_ERROR;                              // Return error anyway
         break;
      } // endif no entry found
   } // end-while i < maxCard

   if (!rc && (i < minCard))            // To less selections found: Set error
      rc = PARSE_ERROR;

   if (!rc && pCallback)        // Perform callback with selection-description
      rc = pCallback (getDescription ());

   if ((rc < 0) || (rc && !optional))
      writeError (stream);

   return rc;
}
