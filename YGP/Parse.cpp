//$Id: Parse.cpp,v 1.31 2002/11/26 04:26:58 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Parse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.31 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.8.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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

#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif


#include <ctype.h>
#include <string.h>

#include <map>
#include <string>

#include "Trace_.h"

#include "Parse.h"
#include "Thread.h"
#include "XStream.h"
#include "Internal.h"

#define BUFFER  (buffers[Thread::currentID ()])


// Parse-buffers; one per thread
static map<unsigned long, std::string> buffers;


void ParseAttomic::freeBuffer () {
   buffers.erase (buffers.find (Thread::currentID ()));
}

static char ESCAPE = '\\';


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : description: Description of object (what it parses)
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : description != NULL
/*--------------------------------------------------------------------------*/
ParseObject::ParseObject (const char* description, bool skipWhitespace)
   : pDescription (description), skip (skipWhitespace) {
   TRACE9 ("Creating ParseObject " << getDescription ());
   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseObject::ParseObject (const ParseObject& other)
   : pDescription (other.pDescription), skip (other.skip) {
   TRACE9 ("Copying ParseObject " << getDescription ());
   Check3 (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseObject::~ParseObject () {
   TRACE9 ("ParseObject::~ParseObject: " << getDescription ());
}


/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
ParseObject& ParseObject::operator= (const ParseObject& other) {
   TRACE8 ("ParseObject::operator=: " << pDescription);
   if (&other != this) {
      pDescription = other.pDescription;
      skip = other.skip;
   } // endif other object

   Check3 (!checkIntegrity ());
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

   TRACE8 ("ParseObject::skipWS (Xistream&) - " << pDescription);
   char c ('\0');
   stream >> c;
   stream.putback (c);
}

/*--------------------------------------------------------------------------*/
//Purpose     : Callback if an object was found
//Parameters  : pFoundValue: Pointer to found value
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseObject::found (const char* pFoundValue, unsigned int) {
   Check3 (pFoundValue);
   return PARSE_OK;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the constraints of the object
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseObject::checkIntegrity () const {
   return pDescription ? OK : NO_DESCRIPTION;
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseEOF::~ParseEOF () {
   TRACE9 ("ParseEOF::~ParseEOF () - " << getDescription ());
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseSkip::~ParseSkip () {
   TRACE9 ("ParseSkip::~ParseSkip () - " << getDescription ());
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : value: List of valid characters
//              description: Description of object (what it parses)
//              max: Maximal cardinality
//              min: Minimal cardinality
//              skipWhitespace: Flag if TRAILING WS are skipped
//              reportData: Flag if parsed data should be stored and reported
//                          via the virtual found method
//Requires    : !checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseAttomic::ParseAttomic (const char* value, const char* description,
                            unsigned int max, unsigned int min,
                            bool skipWhitespace, bool reportData)
   : ParseObject (description, skipWhitespace)
   , pValue (value), maxCard (max), minCard (min), report (reportData) {
   TRACE9 ("Creating ParseAttomic " << getDescription ());
   Check3 (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseAttomic::ParseAttomic (const ParseAttomic& other)
   : ParseObject ((const ParseObject&)other), pValue (other.pValue)
     , maxCard (other.maxCard), minCard (other.minCard), report (other.report) {
   TRACE9 ("Copying ParseAttomic " << getDescription ());
   Check3 (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseAttomic::~ParseAttomic () {
   TRACE9 ("ParseAttomic::~ParseAttomic: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
ParseAttomic& ParseAttomic::operator= (const ParseAttomic& other) {
   TRACE8 ("ParseAttomic::operator=: " << getDescription ());

   if (&other != this) {
      ParseObject::operator= ((const ParseObject&)other);
      pValue = other.pValue;
      maxCard = other.maxCard;
      minCard = other.minCard;
      report = other.report;
   } // endif other object

   Check3 (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Tries to parse the object from the stream
//Parameters  : stream: Source from which to read
//              optional: Flag, if node must be found
/*--------------------------------------------------------------------------*/
int ParseAttomic::doParse (Xistream& stream, bool optional) throw (std::string) {
   TRACE1 ("ParseAttomic::doParse -> " << getDescription ());
   Check3 (!checkIntegrity ());

   int ch (0);
   std::string& buffer = BUFFER;
   buffer = "";
   unsigned int len (0);

   while (buffer.size () < maxCard) {        // While not max. card is reached
      ch = stream.get ();
      TRACE6 ("ParseAttomic::doParse -> " << getDescription () << " -> "
              << (char)ch << " = 0x" << hex << ch << dec);

      if (ch == EOF)
         break;

      int cmp = checkValue ((char)ch);
      if (!cmp) {                                  // Read and check read char
         stream.putback ((char)ch);
         break;
      }
      else
         if (cmp == -1)
            continue;

      if (report || !len)
         buffer += (char)ch;                                   // Store, if OK
      ++len;
   } // end-while !maximal cardinality
   TRACE6 ("ParseAttomic::doParse -> " << getDescription () << ": Final = '"
           << buffer << '\'');

   int rc (PARSE_OK);
   if (len >= minCard) {                                  // Cardinalities OK?
      ch = 0;
      if (report)
         rc = found (buffer.c_str (), len);          // Report object as found
      else {
         buffer += "...";
         buffer += ch;
      }
      TRACE2 ("ParseAttomic::doParse -> " << getDescription () << ": Found '"
              << buffer << '\'');
   } // endif value OK
   else
      rc = PARSE_ERROR;

   if (rc) {
      if ((optional || (rc > 0))) {
         if (report) {
            unsigned int len (buffer.size ());
            while (len--)
               stream.putback (buffer[len]);
         }
      }
      else {
         std::string error (_("Expected %1, found: '%2'"));
         error.replace (error.find ("%1"), 2, getDescription ());
         error.replace (error.find ("%2"), 2, buffer, 0,
                        minCard > 10 ? (minCard > buffer.size ()
			                ? buffer.size () : minCard) : 10);
	 throw (error);
      } // end-if mandatory value not found
   } // endif error
   else
      skipWS (stream);

   buffer = "";
   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. Valid
//              are every characters specified in pValue. If pValue contains
//              a backslash (\) the next character has a special meaning:
//                - 9: ch is valid if it is a digit
//                - A: ch is valid if it is alphabetic
//                - X: ch is valid if it is alphanumeric
//                - ' ' (blank): ch is valid if it is a white-space
//                - 0: ch is valid if it is the zero-character (\0)
//                - \: ch is valid if it is the back-slash (\)
//                - r: ch is valid if it is carriage-return (\r)
//                - n: ch is valid if it is line-feed (\n)
//                - *: ch is valid.
//                - Else: ch is valid if it equal to this char
//Parameters  : ch: Char to check
//Returns     : int: Result; true if valid
/*--------------------------------------------------------------------------*/
int ParseAttomic::checkValue (char ch) {
   TRACE8 ("ParseAttomic::checkValue " << getDescription () << ' ' << ch);
   Check3 (!checkIntegrity ());

   const char* pHelp = pValue; Check3 (pHelp);
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

         case 'n': if (ch == '\n') return true;
            break;

         case 'r': if (ch == '\r') return true;
            break;

         case '0': if (ch == '\0') return true;
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
   return (pValue ? (maxCard < minCard
                     ? MAX_MIN_ERROR : ParseObject::checkIntegrity ())
           : NO_VALUE);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseText::~ParseText () {
   TRACE9 ("ParseText::~ParseText: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. This
//              is true if ch doesn't match any character in the list.
//Parameters  : ch: Char to check
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
int ParseText::checkValue (char ch) {
   TRACE8 ("ParseText::checkValue " << getDescription () << ' ' << ch);

   const char* pHelp = pValue; Check3 (pHelp);
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
//              skipWhitespace: Flag if TRAILING WS are skipped
//              reportData: Flag if data should be stored and reported
//Requires    : value != NULL && !ParseObject::checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseTextEsc::ParseTextEsc (const char* abort, const char* description,
                            unsigned int max, unsigned int min, char escape,
                            bool skipWhitespace, bool reportData)
   : ParseText (abort, description, max, min, skipWhitespace, reportData)
   , esc (escape), last (!escape), prelast (!escape) {
   TRACE9 ("Creating ParseTextEsc " << getDescription ());
};

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseTextEsc::ParseTextEsc (const ParseTextEsc& other)
   : ParseText (other), esc (other.esc), last (!other.esc), prelast (!other.esc) {
   TRACE9 ("Copying ParseTextEsc " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseTextEsc::~ParseTextEsc () {
   TRACE9 ("ParseTextEsc::~ParseTextEsc: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
ParseTextEsc& ParseTextEsc::operator= (const ParseTextEsc& other) {
   TRACE8 ("ParseTextExact::operator=: " << getDescription ());

   if (&other != this) {
      esc = other.esc;
      prelast = !other.esc;last = !other.esc;
      ParseText::operator= (other);
   } // endif other object

   Check3 (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. This
//              is true if ch doesn't match any character in the list (except
//              if it is escaped by the passed escape-character
//Parameters  : ch: Char to check
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
int ParseTextEsc::checkValue (char ch) {
   TRACE8 ("ParseTextEsc::checkValue (char) - " << getDescription () << ' '
           << ch << " [" << last << '/' << prelast << ']');

   const char* pHelp = pValue; Check3 (pHelp);
   while (*pHelp) {
      if ((*pHelp == ch) && ((last != esc)
                             || ((last == esc) && (prelast == esc)))) {
         prelast = last = !esc;
	 return false;
      } // endif
      ++pHelp;
   } // endwhile chars available

   prelast = last;
   last = ch;
   return ((ch == esc) && (prelast != esc)) ? -1 : true;
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : value: List of valid characters
//              description: Description of object (what it parses)
//              skipWhitespace: Flag if TRAILING WS are skipped
//              reportData: Flag, if data should be stored and reported
//Requires    : value != NULL && !ParseObject::checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseExact::ParseExact (const char* value, const char* description,
                        bool skipWhitespace, bool reportData)
   : ParseAttomic (value, description, 1, 1, skipWhitespace, reportData)
   , pos (0) {
   TRACE9 ("Creating ParseExact " << getDescription ());
   unsigned int len (strlen (value));      // value !NULL is checked by parent
   minCard = len;
   maxCard = len;
   Check3 (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseExact::~ParseExact () {
   TRACE9 ("ParseExact::~ParseExact: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
ParseExact& ParseExact::operator= (const ParseExact& other) {
   TRACE8 ("ParseExact::operator=: " << getDescription ());

   if (&other != this) {
      ParseAttomic::operator= ((const ParseAttomic&)other);
      pos = 0;
   } // endif other object

   Check3 (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. This
//              is true, if
//Parameters  : ch: Char to check
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
int ParseExact::checkValue (char ch) {
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
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseUpperExact::~ParseUpperExact () {
   TRACE9 ("ParseUpperExact::~ParseUppeExact: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks if the passed character is valid acc. to pValue. This
//              is true if  ch matches the char in pValue in the same position
//              (not case-sensitive)
//Parameters  : ch: Char to check
//Returns     : boolean: Result; true if valid
/*--------------------------------------------------------------------------*/
int ParseUpperExact::checkValue (char ch) {
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
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : apObjectList != NULL && !ParseObject::checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseSequence::ParseSequence (ParseObject* apObjectList[],
                              const char* description, unsigned int max,
                              unsigned int min, bool skipWhitespace)
   : ParseObject (description, skipWhitespace)
   , ppList (apObjectList), maxCard (max), minCard (min) {
   TRACE9 ("Creating ParseSequence " << getDescription ());
   Check3 (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseSequence::ParseSequence (const ParseSequence& other)
   : ParseObject ((const ParseObject&)other), ppList (other.ppList) {
   TRACE9 ("Copying ParseSequence " << getDescription ());
   Check3 (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseSequence::~ParseSequence () {
   TRACE9 ("ParseSequence::~ParseSequence: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
ParseSequence& ParseSequence::operator= (const ParseSequence& other) {
   TRACE8 ("ParseSequence::operator=: " << getDescription ());

   if (&other != this) {
      ParseObject::operator= ((const ParseObject&)other);
      ppList = other.ppList;
      maxCard = other.maxCard;
      minCard = other.minCard;
   } // endif other object

   Check3 (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Tries to parse the object from the stream
//Parameters  : stream: Source from which to read
//              optional: Flag, if node must be found
//Returns     : PARSE_OK if selection found; PARSE_ERROR if not
//Note        : If a hard error occured or any error for not-optional
//              sequences, an exception is thrown
/*--------------------------------------------------------------------------*/
int ParseSequence::doParse (Xistream& stream, bool optional) throw (std::string) {
   TRACE1 ("ParseSequence::doParse -> " << getDescription ());
   Check3 (!checkIntegrity ());

   unsigned int i (0);
   int rc (PARSE_OK);

   ParseObject** ppAct = NULL;
   while (i++ < maxCard) {
      ppAct = ppList; Check3 (ppAct); Check3 (*ppAct);

      while (*ppAct) {                          // While list contains objects
         if ((rc = (**ppAct).doParse (stream,    // Parse (putback only first)
                                      ppAct == ppList ? optional : false)) != 0)
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

   if (!rc)               // Report found of object  with sequence-description
      rc = found (getDescription (), strlen (getDescription ()));

   if ((rc < 0) || (rc && !(optional && (ppAct == ppList)))) {
      std::string error;
      error = _(*ppAct ? "Error in sequence %1; Expected: %2" : "Error in sequence %1");
      error.replace (error.find ("%1"), 2, getDescription ());
      if (*ppAct)
         error.replace (error.find ("%2"), 2, (*ppAct)->getDescription ());
      throw (error);
   }

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the constraints of the object
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int ParseSequence::checkIntegrity () const {
   return ((ppList)
           ? maxCard < minCard ? MAX_MIN_ERROR : ParseObject::checkIntegrity ()
	   : INVALID_LIST);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Constructor
//Parameters  : pObjectList: NULL-terminated array of objects to parse
//              description: Description of object (what it parses)
//              max: Maximal cardinality
//              min: Minimal cardinality
//              skipWhitespace: Flag if TRAILING WS are skipped
//Requires    : apObjectList != NULL && !ParseObject::checkIntegrity ()
/*--------------------------------------------------------------------------*/
ParseSelection::ParseSelection (ParseObject* apObjectList[],
                                const char* description, unsigned int max,
                                unsigned int min, bool skipWhitespace)
   : ParseSequence (apObjectList, description, max, min, skipWhitespace) {
   TRACE9 ("Creating ParseSelection " << getDescription ());
   Check3 (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Copy-constructor
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
ParseSelection::ParseSelection (const ParseSelection& other)
   : ParseSequence ((const ParseSequence&)other) {
   TRACE9 ("Copying ParseSelection " << getDescription ());
   Check3 (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
ParseSelection::~ParseSelection () {
   TRACE9 ("ParseSelection::~ParseSelection: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
ParseSelection& ParseSelection::operator= (const ParseSelection& other) {
   TRACE8 ("ParseSelection::operator=: " << getDescription ());

   if (&other != this)
      ParseSequence::operator= ((const ParseSequence&)other);

   Check3 (!checkIntegrity ());
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Tries to parse the object from the stream
//Parameters  : stream: Source from which to read
//              optional: Flag, if node must be found
//Returns     : PARSE_OK if selection found; PARSE_ERROR if not
//Note        : If a hard error occured or any error for not-optional
//              sequences, an exception is thrown
/*--------------------------------------------------------------------------*/
int ParseSelection::doParse (Xistream& stream, bool optional) throw (std::string) {
   TRACE1 ("ParseSelection::doParse -> " << getDescription ());
   Check3 (!checkIntegrity ());

   unsigned int i (0);
   int rc (PARSE_OK);

   while (i++ < maxCard) {
      ParseObject** ppAct = ppList; Check3 (ppAct); Check3 (*ppAct);

      while (*ppAct) {                          // While list contains objects
         if ((rc = (**ppAct).doParse (stream,        // Parse (putback always)
                                      (ppAct + 1) == NULL ? optional : true))
             == 0) {                                   // Break if match found
            TRACE8 ("ParseSelection::doParse -> " << getDescription ()
                    << " exiting with rc = " << rc);
            break;
         } // endif

         ++ppAct;
      } // end-while list-entries

      if (!*ppAct) {                                     // Does no entry fit?
         if (i > minCard)
            rc = PARSE_OK;
         break;
      }
   } // end-while i < maxCard

   if (!rc)               // Report found of object with selection-description
      if (i < minCard)
         rc = PARSE_ERROR;
      else
         rc = found (getDescription (), strlen (getDescription ()));

   if ((rc < 0) || (rc && !optional)) {
      std::string error;
      error = _("Expected selection %1");
      error.replace (error.find ("%1"), 2, getDescription ());
      throw (error);
   }

   return rc;
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
CBParseEOF::~CBParseEOF () {
   TRACE9 ("CBParseEOF::~CBParseEOF: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Callback if an object was found
//Parameters  : pFoundValue: Pointer to found value
//              len: Length of found data (= 0)
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int CBParseEOF::found (const char* pFoundValue, unsigned int len) {
   Check3 (pCallback); Check3 (pFoundValue); Check3 (!len);
   return pCallback (pFoundValue, len);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
CBParseAttomic::~CBParseAttomic () {
   TRACE9 ("CBParseAttomic::~CBParseAttomic: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
CBParseAttomic& CBParseAttomic::operator= (const CBParseAttomic& other) {
   TRACE8 ("ParseAttomic::operator=: " << getDescription ());

   if (this != &other) {
      pCallback = other.pCallback; Check3 (pCallback);
      ParseAttomic::operator= (other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Callback if an object was found
//Parameters  : pFoundValue: Pointer to found value
//              len: Length of found data
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int CBParseAttomic::found (const char* pFoundValue, unsigned int len) {
   Check3 (pCallback); Check3 (pFoundValue);
   return pCallback (pFoundValue, len);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
CBParseText::~CBParseText () {
   TRACE9 ("CBParseText::~CBParseText: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
CBParseText& CBParseText::operator= (const CBParseText& other) {
   TRACE8 ("ParseText::operator=: " << getDescription ());

   if (this != &other) {
      pCallback = other.pCallback; Check3 (pCallback);
      ParseText::operator= (other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Callback if an object was found
//Parameters  : pFoundValue: Pointer to found value
//              len: Length of found data
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int CBParseText::found (const char* pFoundValue, unsigned int len) {
   Check3 (pCallback); Check3 (pFoundValue);
   return pCallback (pFoundValue, len);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
CBParseTextEsc::~CBParseTextEsc () {
   TRACE9 ("CBParseTextEsc::~CBParseTextEsc: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
CBParseTextEsc& CBParseTextEsc::operator= (const CBParseTextEsc& other) {
   TRACE8 ("ParseTextEsc::operator=: " << getDescription ());

   if (this != &other) {
      pCallback = other.pCallback; Check3 (pCallback);
      ParseTextEsc::operator= (other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Callback if an object was found
//Parameters  : pFoundValue: Pointer to found value
//              len: Length of found data
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int CBParseTextEsc::found (const char* pFoundValue, unsigned int len) {
   Check3 (pCallback); Check3 (pFoundValue);
   return pCallback (pFoundValue, len);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
CBParseExact::~CBParseExact () {
   TRACE9 ("CBParseExact::~CBParseExact: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
CBParseExact& CBParseExact::operator= (const CBParseExact& other) {
   TRACE8 ("ParseExact::operator=: " << getDescription ());

   if (this != &other) {
      pCallback = other.pCallback; Check3 (pCallback);
      ParseExact::operator= (other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Callback if an object was found
//Parameters  : pFoundValue: Pointer to found value
//              len: Length of found data
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int CBParseExact::found (const char* pFoundValue, unsigned int len) {
   Check3 (pCallback); Check3 (pFoundValue);
   return pCallback (pFoundValue, len);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
CBParseUpperExact::~CBParseUpperExact () {
   TRACE9 ("CBParseUpperExact::~CBParseUpperExact: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
CBParseUpperExact& CBParseUpperExact::operator= (const CBParseUpperExact& other) {
   TRACE8 ("ParseUpperExact::operator=: " << getDescription ());

   if (this != &other) {
      pCallback = other.pCallback; Check3 (pCallback);
      ParseUpperExact::operator= (other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Callback if an object was found
//Parameters  : pFoundValue: Pointer to found value
//              len: Length of found data
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int CBParseUpperExact::found (const char* pFoundValue, unsigned int len) {
   Check3 (pCallback); Check3 (pFoundValue);
   return pCallback (pFoundValue, len);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
CBParseSequence::~CBParseSequence () {
   TRACE9 ("CBParseSequence::~CBParseSequence: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//              len: Length of found data
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
CBParseSequence& CBParseSequence::operator= (const CBParseSequence& other) {
   TRACE8 ("ParseSequence::operator=: " << getDescription ());

   if (this != &other) {
      pCallback = other.pCallback; Check3 (pCallback);
      ParseSequence::operator= (other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Callback if an object was found
//Parameters  : pFoundValue: Pointer to found value
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int CBParseSequence::found (const char* pFoundValue, unsigned int len) {
   Check3 (pCallback); Check3 (pFoundValue);
   return pCallback (pFoundValue, len);
}


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
CBParseSelection::~CBParseSelection () {
   TRACE9 ("CBParseSelection::~CBParseSelection: " << getDescription ());
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
//Returns     : Reference of this
/*--------------------------------------------------------------------------*/
CBParseSelection& CBParseSelection::operator= (const CBParseSelection& other) {
   TRACE8 ("CBParseSelection::operator=: " << getDescription ());

   if (this != &other) {
      pCallback = other.pCallback; Check3 (pCallback);
      ParseSelection::operator= (other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose     : Callback if an object was found
//Parameters  : pFoundValue: Pointer to found value
//              len: Length of found data
//Returns     : int: Status; 0 OK
/*--------------------------------------------------------------------------*/
int CBParseSelection::found (const char* pFoundValue, unsigned int len) {
   Check3 (pCallback); Check3 (pFoundValue);
   return pCallback (pFoundValue, len);
}
