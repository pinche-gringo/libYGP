//$Id: Parse.cpp,v 1.3 1999/08/25 22:40:21 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Parse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
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

#define DEBUG 9
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

ostream& ParseObject::error = cerr;


static char ESCAPE = '\\';


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : description: Description of object (what it parses)
//              max: Maximal cardinality
//              min: Minimal cardinality
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : description != NULL
/*--------------------------------------------------------------------------*/
ParseObject::ParseObject (const char* description, unsigned int max,
                          unsigned int min, const PFNCALLBACK callback,
			  bool skipWhitespace)
   : pDescription (description), maxCard (max), minCard (min)
   , pCallback (callback), skip (skipWhitespace) {
   TRACE9 ("Creating ParseObject " << pDescription);
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseObject::ParseObject (const ParseObject& other)
   : pDescription (other.pDescription), maxCard (other.maxCard)
   , minCard (other.minCard), pCallback (other.pCallback), skip (other.skip) {
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
   TRACE9 ("Assigning ParseObject " << pDescription);
   if (&other != this) {
      pDescription = other.pDescription;
      maxCard = other.maxCard;
      minCard = other.minCard;
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

   TRACE9 ("Skipping WS after " << pDescription);
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
   return pDescription ? (maxCard < minCard) : 2;
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
   : ParseObject (description, max, min, callback, skipWhitespace)
   , pValue (value) {
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
   TRACE9 ("Assigning ParseAttomic " << getDescription ());

   if (&other != this) {
      ParseObject::operator= ((const ParseObject&)other);
      pValue = other.pValue;
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
   TRACE9 ("ParseAttomic::doParse " << getDescription ());
   assert (!checkIntegrity ());

#ifdef MULTIBUFFER
   globVars buf;
#endif

   char* pAkt = global.buffer;
   char  ch;

   unsigned int i (0);
   while (i < maxCard) {                    // While not max. card is reached
      stream >> ch;
      TRACE6 ("ParseAttomic::doParse " << getDescription () << " -> " << ch);

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
   TRACE2 ("ParseAttomic::doParse final" << getDescription () << " -> "
           << global.buffer);

   int rc (PARSE_OK);
   if (i >= minCard) {                                    // Cardinalities OK?
      TRACE6 ("ParseAttomic::doParse " << getDescription () << "Found '"
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
         error << "Parse-error in line " << stream.getLine () << " column "
               << stream.getColumn () << ":\nExpected: " << getDescription ()
               << " found: " << global.buffer << '\n';
      } // end-if mandatory value not found
   } // endif error
   else
      skipWS (stream);

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
bool ParseAttomic::checkValue (char ch) const {
   TRACE9 ("ParseAttomic::checkValue " << getDescription () << ' ' << ch);
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
   return pValue ? ParseObject::checkIntegrity () : 11;
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : pObjectList: NULL-terminated array of objects to parse
//              description: Description of object (what it parses)
//              max: Maximal cardinality
//              min: Minimal cardinality
//              callback: Method to call if object is parsed
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : description != NULL
/*--------------------------------------------------------------------------*/
ParseSequence::ParseSequence (const ParseObject* apObjectList[],
                              const char* description, unsigned int max,
                              unsigned min, PFNCALLBACK callback,
                              bool skipWhitespace)
   : ParseObject (description, max, min, callback, skipWhitespace)
   , ppList (apObjectList) {
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
   TRACE9 ("Assigning ParseSequence " << getDescription ());

   if (&other != this) {
      ParseObject::operator= ((const ParseObject&)other);
      ppList = other.ppList;
   } // endif other object

   assert (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Tries to parse the object from the stream
//Parameters  : stream: Source from which to read
//              optional: Flag, if node must be found
/*--------------------------------------------------------------------------*/
int ParseSequence::doParse (Xistream& stream, bool optional) const {
   TRACE9 ("ParseSequence::doParse " << getDescription ());
   assert (!checkIntegrity ());

   unsigned int i (0);
   int rc (PARSE_OK);

   while (i++ < maxCard) {
      const ParseObject** ppAct (ppList); assert (*ppAct);

      while (*ppAct != NULL) {                  // While list contains objects
         if (rc = (**ppAct).doParse (stream,   // Parse (putback first always)
                                     ppAct == ppList ? true : optional))
            break;

         ++ppAct;
      } // end-while list-entries

      if (rc) {   // Simple error while parsing: Check if in begin of sequence
         if ((rc > 0) && (ppAct == ppList) && (i >= minCard))
            rc = PARSE_OK;
         break;
      } // endif error occured
   } // end-while i < maxCard

   if (!rc && pCallback)         // Perform callback with sequence-description
      rc = pCallback (getDescription ());

   if ((rc < 0) || (rc && !optional))
      error << "Parse-error in line " << stream.getLine () << " column "
            << stream.getColumn () << ":\nExpected: " << getDescription () << '\n';

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the constraints of the object
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseSequence::checkIntegrity () const {
   return (ppList && *ppList) ? ParseObject::checkIntegrity () : 10;
}
