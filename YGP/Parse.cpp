//$Id: Parse.cpp,v 1.60 2008/03/23 13:56:12 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : Parse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.60 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.8.1999
//COPYRIGHT   : Copyright (C) 1999 - 2008

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


#include <cctype>
#include <cstring>

#include <map>
#include <string>

#include "YGP/Trace.h"
#include "YGP/Thread.h"
#include "YGP/XStream.h"
#include "YGP/Internal.h"

#include "YGP/Parse.h"

#define BUFFER  (buffers[Thread::currentID ()])


/// Parse-buffers; one per thread
static std::map<unsigned long, std::string> buffers;
static char ESCAPE = '\\';            ///< Escape character for ParseAttomic()

namespace YGP {


//-----------------------------------------------------------------------------
/// Frees the buffer internally used while parsing.
/// \remarks Don't delete the buffer while parsing (inside a callback)!
//-----------------------------------------------------------------------------
void ParseObject::freeBuffer () {
   buffers.erase (Thread::currentID ());
}

//-----------------------------------------------------------------------------
/// Constructor The description must be valid during the LIFETIME (OR AT LEAST
/// the usage) of the object!
/// \param description: Description of the object (what it parses)
/// \param skipWhitespace: Flag if \b trailing whitespaces should be skipped
/// after sucessfully parsing the object
/// \pre description valid ASCIIZ string != NULL
//-----------------------------------------------------------------------------
ParseObject::ParseObject (const char* description, bool skipWhitespace)
   : skip (skipWhitespace), pDescription (description) {
   TRACE9 ("ParseObject::ParseObject (const char*, bool) - " << getDescription ());
   Check1 (!checkIntegrity ());
}

//-----------------------------------------------------------------------------
/// Copy-constructor
/// \param other: Object to clone
//-----------------------------------------------------------------------------
ParseObject::ParseObject (const ParseObject& other)
   : skip (other.skip) , pDescription (other.pDescription) {
   TRACE9 ("ParseObject::ParseObject (const ParseObject&) - " << getDescription ());
   Check1 (!checkIntegrity ());
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseObject::~ParseObject () {
   TRACE9 ("ParseObject::~ParseObject () - " << getDescription ());
}


//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
ParseObject& ParseObject::operator= (const ParseObject& other) {
   TRACE9 ("ParseObject::operator= (const ParseObject&) - " << pDescription);
   Check1 (!other.checkIntegrity ());

   pDescription = other.pDescription;
   skip = other.skip;
   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Skips all whitespace characters (blank, tabulator, carriage return and
/// line feed) from the current position in the passed stream (if the object
/// \param stream: Source from which to read
//-----------------------------------------------------------------------------
void ParseObject::skipWS (Xistream& stream) {
   char c ('\0');
   stream >> c;
   stream.putback (c);
}

//-----------------------------------------------------------------------------
/// Callback if an object was found
/// \param: Pointer to found value
/// \param: Length of found value
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int ParseObject::found (const char*, unsigned int) {
   return PARSE_OK;
}

//-----------------------------------------------------------------------------
/// Checks the constraints of the object
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int ParseObject::checkIntegrity () const {
   return pDescription ? OK : NO_DESCRIPTION;
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseEOF::~ParseEOF () {
   TRACE9 ("ParseEOF::~ParseEOF () - " << getDescription ());
}


//-----------------------------------------------------------------------------
/// "Parses" the object. This class returns found, if there is no more
/// data available to parse.
///
/// Returns \c PARSE_OK if data matching the object is found (and the callback
/// does not report something different).
/// \param stream: Source from which to read
//-----------------------------------------------------------------------------
int ParseEOF::doParse (Xistream& stream, bool) {
   int ch (stream.get ());
   TRACE9 ("ParseEOF::doParse (Xistream&, bool) - " << ch);
   if (ch == EOF) {
      return found ("EOF", 3);
   }
   stream.putback ((char)ch);
   return PARSE_ERROR;
}

//-----------------------------------------------------------------------------
/// Construcutor
/// \param bytes: Number of bytes to skip
/// \param seek: From where to seek in the search
//-----------------------------------------------------------------------------
ParseSkip::ParseSkip (std::streamoff bytes, std::ios_base::seekdir seek)
   : ParseObject ("Skip", false), offset (bytes), seek (seek) {
 }

//-----------------------------------------------------------------------------
/// Copy constructor
/// \param other: Object to copy
//-----------------------------------------------------------------------------
ParseSkip::ParseSkip (const ParseSkip& other)
   : ParseObject (other), offset (other.offset), seek (other.seek) {
 }

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseSkip::~ParseSkip () {
   TRACE9 ("ParseSkip::~ParseSkip () - " << getDescription ());
}


//-----------------------------------------------------------------------------
/// "Parses" the object; the previously specified number of bytes are
/// skipped (from the also set position).
//-----------------------------------------------------------------------------
int ParseSkip::doParse (Xistream& stream, bool) {
   TRACE5 ("ParseSkip::doParse (Xistream& stream, bool) - " << offset << '/' << (int)seek);
   stream.seekg (offset, seek);
   return PARSE_OK;
}


//-----------------------------------------------------------------------------
/// Constructor; sets the neccessary data of this object.
/// \param value: List of valid characters (see class description)
/// \param description: Description of the object (what it parses)
/// \param max: Maximal cardinality, how often can the object be parsed
/// \param min: Minimal cardinality, how often must the object be parsed
/// \param skipWhitespace: Flag if trailing whitespaces should be skipped after sucessfully parsing the object
/// \param reportData: Flag if parsed data should be stored and reported via the virtual found method
/// \pre !checkIntegrity ()
//-----------------------------------------------------------------------------
ParseAttomic::ParseAttomic (const char* value, const char* description,
                            unsigned int max, unsigned int min,
                            bool skipWhitespace, bool reportData)
   : ParseObject (description, skipWhitespace)
   , pValue (value), maxCard (max), minCard (min), report (reportData) {
   TRACE9 ("ParseAttomic::ParseAttomic (2x const char*, 3x unsigned int, "
           "2x bool)\n\t- " << getDescription ());
   Check1 (!checkIntegrity ());
}

//-----------------------------------------------------------------------------
/// Copy-constructor
/// \param other: Object to clone
//-----------------------------------------------------------------------------
ParseAttomic::ParseAttomic (const ParseAttomic& other)
   : ParseObject ((const ParseObject&)other), pValue (other.pValue)
     , maxCard (other.maxCard), minCard (other.minCard), report (other.report) {
   TRACE9 ("ParseAttomic::ParseAttomic (const ParseAttomic&) - " << getDescription ());
   Check1 (!checkIntegrity ());
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseAttomic::~ParseAttomic () {
   TRACE9 ("ParseAttomic::~ParseAttomic () - " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
ParseAttomic& ParseAttomic::operator= (const ParseAttomic& other) {
   TRACE9 ("ParseAttomic::operator= (const ParseAttomic&) - " << getDescription ());
   Check1 (!other.checkIntegrity ());

   ParseObject::operator= ((const ParseObject&)other);
   pValue = other.pValue;
   maxCard = other.maxCard;
   minCard = other.minCard;
   report = other.report;

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Tries to parse the object from the stream, as long as checkValue() reports
/// the input as valid (or as to ignore, by returning -1).
///
/// Returns \c PARSE_OK if data matching the object is found (and the callback
/// does not report something different).
///
/// It is a soft error (\c PARSE_ERROR) if the minimal cardinality is not
/// fullfilled. If parsing is optional or the error is recoverable (> 0), the
/// parsed data is pushed back into the stream and \c PARSE_ERROR is
/// returned. If parsing is not optional and the error is not recoverable (<
/// 0), an exception (std::string) is thrown.
/// \param stream: Source from which to read
/// \param optional: Flag, if node must be found
/// \throw YGP::ParseError: In case of a not recoverable error
//-----------------------------------------------------------------------------
int ParseAttomic::doParse (Xistream& stream, bool optional) throw (YGP::ParseError) {
   TRACE1 ("ParseAttomic::doParse (Xistream&, bool) - " << getDescription ());

   int ch ('\0');
   std::string& buffer = BUFFER;
   unsigned int parsedChars (0);
   buffer = "";

   while (parsedChars < maxCard) {           // While not max. card is reached
      ch = stream.get ();
      TRACE6 ("ParseAttomic::doParse (Xistream&, bool) - " << getDescription ()
              << " -> " << (char)ch << " = 0x" << std::hex << ch << std::dec);

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

      if (report || buffer.empty ())
         buffer += (char)ch;                                   // Store, if OK
      ++parsedChars;
   } // end-while !maximal cardinality
   TRACE6 ("ParseAttomic::doParse (Xistream&, bool) - " << getDescription ()
           << ": Final = '" << buffer << '\'');

   int rc (PARSE_OK);
   if (parsedChars >= minCard) {                          // Cardinalities OK?
      if (report)
         rc = found (buffer.c_str (), buffer.length ());
      else {
         buffer += "...";
         buffer += ch;
      }
      TRACE2 ("ParseAttomic::doParse (Xistream&, bool) - " << getDescription ()
              << ": Found '" << buffer << '\'');
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
         if (buffer.size () > 23)
            buffer.replace (10, buffer.size () - 20, "...");
         error.replace (error.find ("%2"), 2, buffer);
	 throw (YGP::ParseError (error));
      } // end-if mandatory value not found
   } // endif error
   else
      if (skip)
         skipWS (stream);

   buffer = "";
   return rc;
}

//-----------------------------------------------------------------------------
/// Checks if the passed character is valid according to pValue. Valid are
/// all characters specified in pValue. If \c pValue contains a backslash (\\)
/// the next character has a special meaning:
///    - \b 9: ch is valid if it is a digit
///    - \b A: ch is valid if it is alphabetic
///    - \b X: ch is valid if it is alphanumeric
///    - <b>' ' (blank)</b>: ch is valid if it is a white-space
///    - \c 0: ch is valid if it is the zero-character (\\0)
///    - <b>\\</b>: ch is valid if it is the back-slash (\\) itself
///    - \b r: ch is valid if it is carriage-return (\\r)
///    - \b n: ch is valid if it is line-feed (\\n)
///    - <b>*</b>: ch is always valid.
///     - Else: ch is valid if it equal to this char
/// \param ch: Char to check
/// \returns \c int: Result; 1 if ch valid, -1 if it should be just ignored.
///     Else 0
//-----------------------------------------------------------------------------
int ParseAttomic::checkValue (char ch) {
   TRACE8 ("ParseAttomic::checkValue (char) - " << getDescription () << ' ' << ch);
   Check1 (!checkIntegrity ());

   bool asIs (true);
   const char* pHelp = pValue; Check3 (pHelp);
   while (*pHelp) {
      if (*pHelp == ESCAPE) {
         switch (*++pHelp) {
         case '9': if (isdigit (ch)) return asIs;
            break;

         case 'X': if (isdigit (ch)) return asIs;
         case 'A': if (isalpha (ch)) return asIs;
            break;

         case ' ': if (isspace (ch)) return asIs;
            break;

         case '*': return asIs;

         case 'n': if (ch == '\n') return asIs;
            break;

         case 'r': if (ch == '\r') return asIs;
            break;

         case '0': if (ch == '\0') return asIs;
            break;

         case '!':
            asIs = !asIs;
            break;

         case '\0':
            return false;

         default:
            if (ch == *pHelp)
               return asIs;
         } // end-switch
      } // endif ESCAPE-char in pValue
      else
         if (*pHelp == ch)
            return asIs;

      ++pHelp;
   } // end-while valid chars left

   return !asIs;
}

//-----------------------------------------------------------------------------
/// Checks if this object is in a valid state.
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int ParseAttomic::checkIntegrity () const {
   return (pValue ? (maxCard < minCard
                     ? MAX_MIN_ERROR : ParseObject::checkIntegrity ())
           : NO_VALUE);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseText::~ParseText () {
   TRACE9 ("ParseText::~ParseText () - " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Checks if the parsed value is according the abort-list of the object.
/// Valid are every characters NOT in pValue.
/// \param ch: Char to check
/// \returns \c int: Result; true if ch does not match any of those values.
//-----------------------------------------------------------------------------
int ParseText::checkValue (char ch) {
   TRACE8 ("ParseText::checkValue (char) -  " << getDescription () << ": " << ch);
   Check1 (!checkIntegrity ());

   const char* pHelp = pValue; Check3 (pHelp);
   while (*pHelp) {
      if (*pHelp++ == ch)
	 return false;
   } // endwhile chars available

   return true;
}


//-----------------------------------------------------------------------------
/// Constructor; sets the neccessary data of this object.
/// \param abort: List of valid characters
/// \param description: Description of the object (what it parses)
/// \param max: Maximal cardinality, how often can the object be parsed
/// \param min: Minimal cardinality, how often must the object be parsed
/// \param escape: Character which escapes characters in value
/// \param skipWhitespace: Flag if trailing whitespaces should be skipped after sucessfully parsing the object
/// \param reportData: Flag if parsed data should be stored and reported via the virtual found method
/// \pre value != NULL && !ParseObject::checkIntegrity ()
//-----------------------------------------------------------------------------
ParseTextEsc::ParseTextEsc (const char* abort, const char* description,
                            unsigned int max, unsigned int min, char escape,
                            bool skipWhitespace, bool reportData)
   : ParseText (abort, description, max, min, skipWhitespace, reportData)
   , esc (escape), last (!escape) {
   TRACE9 ("Creating ParseTextEsc " << getDescription ());
};

//-----------------------------------------------------------------------------
/// Copy-constructor
/// \param other: Object to clone
//-----------------------------------------------------------------------------
ParseTextEsc::ParseTextEsc (const ParseTextEsc& other)
   : ParseText (other), esc (other.esc), last (!other.esc) {
   TRACE9 ("Copying ParseTextEsc " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseTextEsc::~ParseTextEsc () {
   TRACE9 ("ParseTextEsc::~ParseTextEsc: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
ParseTextEsc& ParseTextEsc::operator= (const ParseTextEsc& other) {
   TRACE8 ("ParseTextExact::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   if (&other != this) {
      esc = other.esc;
      last = !other.esc;
      ParseText::operator= (other);
   } // endif other object

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Checks if the parsed value is according the abort-list of the object.
/// Valid are every characters NOT in pValue or even those if they are
/// preceeded ("escaped") by the character specified by escape (unless this
/// itself is preceeded by itself).
/// \param ch: Char to check
/// \returns \c boolean: Result; 1 if valid, 0 if not or -1 if character
///     should be ignored
//-----------------------------------------------------------------------------
int ParseTextEsc::checkValue (char ch) {
   TRACE8 ("ParseTextEsc::checkValue (char) - " << getDescription () << ' '
           << ch << " - " << last);
   Check1 (!checkIntegrity ());

   const char* pHelp = pValue; Check3 (pHelp);
   while (*pHelp) {
      if ((*pHelp == ch) && (last != esc)) {
         last = !esc;
         return false;
      } // endif
      ++pHelp;
   } // endwhile chars available

   last = ((last == esc) && (ch == esc)) ? !esc : ch;
   return (last == esc) ? -1 : true;
}


//-----------------------------------------------------------------------------
/// Constructor; sets the neccessary data of this object.
/// \param quote: Character limiting the text; the equivalent characters ends parsing.
/// \param description: Description of the object (what it parses)
/// \param max: Maximal cardinality, how often can the object be parsed
/// \param min: Minimal cardinality, how often must the object be parsed
/// \param skipWhitespace: Flag if trailing whitespaces should be skipped after sucessfully parsing the object
/// \param reportData: Flag if parsed data should be stored and reported via the virtual found method
/// \pre !checkIntegrity ()
//-----------------------------------------------------------------------------
ParseQuoted::ParseQuoted (char quote, const char* description,
                          unsigned int max, unsigned int min,
                          bool skipWhitespace, bool reportData)
   : ParseText (pQuote, description, max, min, skipWhitespace, reportData)
   , pos (0) {
   TRACE9 ("ParseQuoted::ParseQuoted (char, const char*, 2x unsigned int, 2x bool)"
           "\n\t- " << getDescription ());

   *pQuote = getClosingChar (quote);
   pQuote[1] = '\0';
   pQuote[2] = quote;
}

//-----------------------------------------------------------------------------
/// Copy-constructor
/// \param other: Object to clone
//-----------------------------------------------------------------------------
ParseQuoted::ParseQuoted (const ParseQuoted& other)
   : ParseText (other), pos (0) {
   TRACE9 ("ParseQuoted::ParseQuoted (const ParseQuoted&) - " << getDescription ());

   *pQuote = *other.pQuote;
   pQuote[1] = '\0';
   pQuote[2] = other.pQuote[2];
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseQuoted::~ParseQuoted () {
   TRACE9 ("ParseQuoted::~ParseQuoted () - " << getDescription ());
}


//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
ParseQuoted& ParseQuoted::operator= (const ParseQuoted& other) {
   TRACE9 ("ParseQuoted::operator= (const ParseQuoted&) - " << getDescription ());
   Check1 (!other.checkIntegrity ());

   if (&other != this) {
      ParseText::operator= ((const ParseText&)other);
      *pQuote = *other.pQuote;
      pQuote[1] = '\0';
      pQuote[2] = other.pQuote[2];
   } // endif other object

   pos = 0;
   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Returns the matching "closing" character to the passed one. This is
/// considered to be:
///    - '>' if ch == '<'
///    - ')' if ch == '('
///    - ']' if ch == '['
///    - '}' if ch == '{'
///    - '´' if ch == '`'
///    - Else: ch
/// \param ch: Char to find match to
/// \returns \c Character "closing" the passed one
//-----------------------------------------------------------------------------
char ParseQuoted::getClosingChar (char ch) {
   static char open[]  = "<([{`";
   static char close[] = ">)]}´";
   Check2 (sizeof (open) == sizeof (close));

   for (unsigned int i (0); i < sizeof (open); ++i)
      if (open[i] == ch)
         return close[i];

   return ch;
}

//-----------------------------------------------------------------------------
/// Checks if the parsed value is according the quote. The quotes themself are
/// also valid, though not included in the output. Valid are every characters
/// NOT in pValue.
/// \param ch: Char to check
/// \returns \c int: Result; true if ch is an ordinary character, -1 if it is
///     quote, 0 after ending quote
//-----------------------------------------------------------------------------
int ParseQuoted::checkValue (char ch) {
   TRACE8 ("ParseQuoted::checkValue (char) - " << getDescription () << ": " << ch);

   int rc (0);
   switch (pos) {
   case 0:
      if (ch == pQuote[2]) {
         rc = -1;
         pos = 1;
      }
      break;

   case -1:
      pos = 0;
      break;

   default:
      rc = ParseText::checkValue (ch);
      if (rc)
         break;

      if (ch == *pQuote)
         rc = pos = -1;
   } // end-switch
   return rc;
}


//-----------------------------------------------------------------------------
/// Constructor; sets the neccessary data of this object.
/// \param quote: Character limiting the text
/// \param description: Description of the object (what it parses)
/// \param max: Maximal cardinality, how often can the object be parsed
/// \param min: Minimal cardinality, how often must the object be parsed
/// \param escape: Character which escapes characters in value
/// \param skipWhitespace: Flag if trailing whitespaces should be skipped after sucessfully parsing the object
/// \param reportData: Flag if parsed data should be stored and reported via the virtual found method
/// \pre !checkIntegrity ()
//-----------------------------------------------------------------------------
ParseQuotedEsc::ParseQuotedEsc (char quote, const char* description,
                                unsigned int max, unsigned int min, char escape,
                                bool skipWhitespace, bool reportData)
   : ParseTextEsc (pQuote, description, max, min, escape, skipWhitespace, reportData)
   , pos (0) {
   TRACE9 ("ParseQuotedEsc::ParseQuotedEsc (char, const char*, 2x unsigned int, char, 2x bool)"
           "\n\t- " << getDescription ());

   *pQuote = ParseQuoted::getClosingChar (quote);
   pQuote[1] = '\0';
   pQuote[2] = quote;
}

//-----------------------------------------------------------------------------
/// Copy-constructor
/// \param other: Object to clone
//-----------------------------------------------------------------------------
ParseQuotedEsc::ParseQuotedEsc (const ParseQuotedEsc& other)
   : ParseTextEsc (other), pos (0) {
   TRACE9 ("ParseQuotedEsc::ParseQuotedEsc (const ParseQuotedEsc&) - " << getDescription ());

   *pQuote = *other.pQuote;
   pQuote[1] = '\0';
   pQuote[2] = other.pQuote[2];
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseQuotedEsc::~ParseQuotedEsc () {
   TRACE9 ("ParseQuotedEsc::~ParseQuotedEsc () - " << getDescription ());
}


//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
ParseQuotedEsc& ParseQuotedEsc::operator= (const ParseQuotedEsc& other) {
   TRACE9 ("ParseQuoted::operator= (const ParseQuoted&) - " << getDescription ());
   Check1 (!other.checkIntegrity ());

   if (&other != this) {
      ParseTextEsc::operator= ((const ParseTextEsc&)other);
      *pQuote = *other.pQuote;
      pQuote[1] = '\0';
      pQuote[2] = other.pQuote[2];
   } // endif other object

   pos = 0;
   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Checks if the parsed value is according the quote. The quotes themself are
/// also valid, though not included in the output. Valid are every characters
/// NOT in pValue.
/// \param ch: Char to check
/// \returns \c int: Result; true if ch is an ordinary character, -1 if it is
///     quote, 0 after ending quote
//-----------------------------------------------------------------------------
int ParseQuotedEsc::checkValue (char ch) {
   TRACE8 ("ParseQuotedEsc::checkValue (char) - " << getDescription () << ": " << ch);

   int rc (0);
   switch (pos) {
   case 0:
      if (ch == pQuote[2]) {
         rc = -1;
         pos = 1;
      }
      break;

   case -1:
      break;

   default:
      rc = ParseTextEsc::checkValue (ch);
      if (rc)
         break;

      if (ch == *pQuote)
         rc = pos = -1;
   } // end-switch
   return rc;
}


//-----------------------------------------------------------------------------
/// Constructor; sets the neccessary data of this object
/// \param value: Sequence of characters to parse in that order
/// \param description: Description of the object (what it parses)
/// \param skipWhitespace: Flag if trailing whitespaces should be skipped
///      after sucessfully parsing the object
/// \param reportData: Flag, if data should be stored and reported
/// \pre value != NULL && !ParseObject::checkIntegrity ()
//-----------------------------------------------------------------------------
ParseExact::ParseExact (const char* value, const char* description,
                        bool skipWhitespace, bool reportData)
   : ParseAttomic (value, description, 1, 1, skipWhitespace, reportData)
   , pos (0) {
   TRACE9 ("Creating ParseExact " << getDescription ());
   maxCard = minCard = strlen (value);     // value !NULL is checked by parent
   Check1 (!checkIntegrity ());
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseExact::~ParseExact () {
   TRACE9 ("ParseExact::~ParseExact: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
ParseExact& ParseExact::operator= (const ParseExact& other) {
   TRACE8 ("ParseExact::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   if (&other != this)
      ParseAttomic::operator= ((const ParseAttomic&)other);
   pos = 0;

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Checks if the passed character is exactly equal (case-sensitive!) than the
/// one in the actual position of the object.
/// \param ch: Char to check
/// \returns \c boolean: Result; true if valid
//-----------------------------------------------------------------------------
int ParseExact::checkValue (char ch) {
   TRACE8 ("ParseExact::checkValue " << getDescription () << ' ' << ch);
   if (pValue[pos] == ch) {     // Valid if ch == act-char; if wrong reset pos
      if (++pos >= maxCard)         // Reset position after successfull serach
         pos = 0;
      return true;
   }

   pos = 0;
   return false;
}

//-----------------------------------------------------------------------------
/// Checks the constraints of the object
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int ParseExact::checkIntegrity () const {
   return pos > (strlen (pValue) + 1) ? POS_ERROR : ParseAttomic::checkIntegrity ();
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseUpperExact::~ParseUpperExact () {
   TRACE9 ("ParseUpperExact::~ParseUpperExact: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Checks if the passed character is exactly equal (not case-sensitive!) than
/// the one in the actual position of the object.
/// \param ch: Char to check
/// \returns \c boolean: Result; true if valid
//-----------------------------------------------------------------------------
int ParseUpperExact::checkValue (char ch) {
   TRACE8 ("ParseUpperExact::checkValue " << getDescription () << ' ' << ch);
   return ParseExact::checkValue ((char)toupper (ch));
}

//-----------------------------------------------------------------------------
/// Checks the constraints of the object
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int ParseUpperExact::checkIntegrity () const {
   for (unsigned int i (0); i < maxCard; ++i)
      if (pValue[i] != toupper (pValue[i]))
	 return VALUE_NOT_UPPERCASE;

   return ParseExact::checkIntegrity ();
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseToText::~ParseToText () {
   TRACE9 ("ParseToText::~ParseToText () - " << getDescription ());
}


//-----------------------------------------------------------------------------
/// Tries to parse the object from the stream.
///
/// Returns \c PARSE_OK if data matching the object is found (and the callback
/// does not report something different).
///
/// It is a soft error (\c PARSE_ERROR) if the minimal cardinality is not
/// fullfilled. If parsing is optional the
/// parsed data is pushed back into the stream and \c PARSE_ERROR is
/// returned, else
/// 0), an exception (std::string) is thrown.

/// \param stream: Source from which to read
/// \param optional: Flag, if node must be found
/// \throw YGP::ParseError: In case of a not recoverable error
//-----------------------------------------------------------------------------
int ParseToText::doParse (Xistream& stream, bool optional) throw (YGP::ParseError) {
   TRACE1 ("ParseToText::doParse (Xistream&, bool) - " << getDescription ());

   std::streampos oldPos (stream.tellg ()), lastPos (oldPos);
   int ch (0);

   unsigned int i (0);
   while (i < maxCard) {
      TRACE3 ("ParseToText::doParse (Xistream&, bool) - Occurence: " << minCard << '-' << i << '-' << maxCard);
      // Inspect stream; search for the first character of the search-string
      while ((ch = stream.get ()) != EOF) {
	 if ((char)ch == *pValue) {
	    TRACE7 ("ParseToText::doParse (Xistream&, bool) - Found 1st char");
	    // If found, try to parse the whole text
	    const char* pAct (pValue);

	    do {
	       // Check if all chars have been matched
	       if (!*++pAct)
		  goto found;
	       // End at EOF
	       if ((ch = stream.get ()) == EOF)
		  goto eof;

	    } while (((char)ch) == *pAct);
	    TRACE8 ("ParseToText::doParse (Xistream&, bool) - No match");
	 } // endif first char found
      } // end-while
      break;

 found:
      TRACE5 ("ParseToText::doParse (Xistream&, bool) - Found");
      lastPos = stream.tellg ();
      ++i;
   }

 eof:
   if (i >= minCard) {
      if (ch == EOF)
	 stream.seekg (lastPos, std::ios::beg);
      return found (pValue, i);
   }
   else {
      if (optional) {
	 stream.seekg (oldPos, std::ios::beg);
	 return PARSE_ERROR;
      }
      else {
         std::string error (_("Expected %1 not found"));
	 error.replace (error.find ("%1"), 2, getDescription ());
	 throw (YGP::ParseError (error));
      }
   }
}


//-----------------------------------------------------------------------------
/// Constructor; sets the neccessary data of this object.
/// \param apObjectList: NULL-terminated array of pointers to objects to parse
/// \param description: Description of the object (what it parses)
/// \param max: Maximal cardinality, how often can the object be parsed
/// \param min: Minimal cardinality, how often must the object be parsed
/// \param skipWhitespace: Flag if whitespaces should bS are skipped
/// \pre apObjectList != NULL && !ParseObject::checkIntegrity ()
//-----------------------------------------------------------------------------
ParseSequence::ParseSequence (ParseObject* apObjectList[],
                              const char* description, unsigned int max,
                              unsigned int min, bool skipWhitespace)
   : ParseObject (description, skipWhitespace)
   , ppList (apObjectList), maxCard (max), minCard (min) {
   TRACE9 ("Creating ParseSequence " << getDescription ());
   Check1 (!checkIntegrity ());
}

//-----------------------------------------------------------------------------
/// Copy-constructor
/// \param other: Object to clone
//-----------------------------------------------------------------------------
ParseSequence::ParseSequence (const ParseSequence& other)
   : ParseObject ((const ParseObject&)other), ppList (other.ppList),
     maxCard (other.maxCard), minCard (other.minCard) {
   TRACE9 ("Copying ParseSequence " << getDescription ());
   Check1 (!checkIntegrity ());
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseSequence::~ParseSequence () {
   TRACE9 ("ParseSequence::~ParseSequence: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
ParseSequence& ParseSequence::operator= (const ParseSequence& other) {
   TRACE8 ("ParseSequence::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   if (&other != this) {
      ParseObject::operator= ((const ParseObject&)other);
      ppList = other.ppList;
      maxCard = other.maxCard;
      minCard = other.minCard;
   } // endif other object

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Tries to parse the objects of the sequence from the stream, as long as
/// every object reports PARSE_OK. It is a soft error (PARSE_ERROR) if the
/// minimal cardinality is not fullfilled. If parsing is not optional and/or
/// the error is not recoverable (< 0), an exception (std::string) is thrown.
/// \param stream: Source from which to read
/// \param optional: Flag, if object must be found
/// \returns \c PARSE_OK if selection found; PARSE_ERROR if not
/// \throw YGP::ParseError: In case of a not recoverable error
//-----------------------------------------------------------------------------
int ParseSequence::doParse (Xistream& stream, bool optional) throw (YGP::ParseError) {
   TRACE1 ("ParseSequence::doParse -> " << getDescription () << ' ' << maxCard);

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
                 << " got error rc = " << rc);

         // Reset error if not hard (<0) and the error happened parsing the
         // first element (but only if mincard is fullfilled)
         if ((rc > 0) && (ppAct == ppList) && (i > minCard))
            rc = PARSE_OK;
         break;
      } // endif error occured
   } // end-while i < maxCard

   if (!rc)               // Report found of object  with sequence-description
      rc = (i < minCard) ? PARSE_ERROR :  found (getDescription (), i);

   if ((rc < 0) || (rc && !(optional && (ppAct == ppList)))) {
      std::string error;
      error = _(*ppAct ? N_("Error in sequence %1; Expected: %2") : N_("Error in sequence %1"));
      error.replace (error.find ("%1"), 2, getDescription ());
      if (*ppAct)
         error.replace (error.find ("%2"), 2, (*ppAct)->getDescription ());
      throw (YGP::ParseError (error));
   }

   TRACE8 ("ParseSequence::doParse -> " << getDescription () << " exiting with rc = " << rc);
   return rc;
}

//-----------------------------------------------------------------------------
/// Checks the constraints of the object
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int ParseSequence::checkIntegrity () const {
   return ((ppList)
           ? maxCard < minCard ? MAX_MIN_ERROR : ParseObject::checkIntegrity ()
	   : INVALID_LIST);
}


//-----------------------------------------------------------------------------
/// Constructor; sets the neccessary data of this object.
/// \param apObjectList: NULL-terminated array of pointers to objects to parse
/// \param description: Description of the object (what it parses)
/// \param max: Maximal cardinality, how often can the object be parsed
/// \param min: Minimal cardinality, how often must the object be parsed
/// \param skipWhitespace: Flag if trailing whitespaces should be skipped after sucessfully parsing the object
/// \pre apObjectList != NULL && !ParseObject::checkIntegrity ()
//-----------------------------------------------------------------------------
ParseSelection::ParseSelection (ParseObject* apObjectList[],
                                const char* description, unsigned int max,
                                unsigned int min, bool skipWhitespace)
   : ParseSequence (apObjectList, description, max, min, skipWhitespace) {
   TRACE9 ("Creating ParseSelection " << getDescription ());
   Check1 (!checkIntegrity ());
}

//-----------------------------------------------------------------------------
/// Copy-constructor
/// \param other: Object to clone
//-----------------------------------------------------------------------------
ParseSelection::ParseSelection (const ParseSelection& other)
   : ParseSequence ((const ParseSequence&)other) {
   TRACE9 ("Copying ParseSelection " << getDescription ());
   Check1 (!checkIntegrity ());
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ParseSelection::~ParseSelection () {
   TRACE9 ("ParseSelection::~ParseSelection: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
ParseSelection& ParseSelection::operator= (const ParseSelection& other) {
   TRACE8 ("ParseSelection::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   if (&other != this)
      ParseSequence::operator= ((const ParseSequence&)other);

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Tries to parse the objects of the selection from the stream, until one
/// object reports PARSE_OK. It is a soft error (PARSE_ERROR) if the minimal
/// cardinality is not fullfilled. If parsing is not optional and/or the error
/// is not recoverable (< 0), an exception (std::string) is thrown.
/// \param stream: Source from which to read
/// \param optional: Flag, if node must be found
/// \returns \c PARSE_OK if selection found; PARSE_ERROR if not
/// \throw YGP::ParseError: In case of a not recoverable error
//-----------------------------------------------------------------------------
int ParseSelection::doParse (Xistream& stream, bool optional) throw (YGP::ParseError) {
   TRACE1 ("ParseSelection::doParse -> " << getDescription () << ' ' << maxCard);

   unsigned int i (0);
   int rc (PARSE_OK);

   while (i++ < maxCard) {
      ParseObject** ppAct = ppList; Check3 (ppAct); Check3 (*ppAct);

      while (*ppAct) {                          // While list contains objects
         if ((rc = (**ppAct).doParse (stream,        // Parse (putback always)
                                      (ppAct + 1) == NULL ? optional : true))
             == 0) {                                   // Break if match found
            TRACE8 ("ParseSelection::doParse -> " << getDescription () << " found entry");
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
         rc = found (getDescription (), i);

   if ((rc < 0) || (rc && !optional)) {
      std::string error;
      error = _("Expected selection %1");
      error.replace (error.find ("%1"), 2, getDescription ());
      throw (YGP::ParseError (error));
   }

   TRACE8 ("ParseSelection::doParse -> " << getDescription ()
           << " exiting with rc=" << rc);
   return rc;
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseEOF::~CBParseEOF () {
   TRACE9 ("CBParseEOF::~CBParseEOF: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameters and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK Requieres : pFoundValue != 0; len == 0
//-----------------------------------------------------------------------------
int CBParseEOF::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue); Check1 (!len);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseSkip::~CBParseSkip () {
   TRACE9 ("CBParseSkip::~CBParseSkip: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseSkip& CBParseSkip::operator= (const CBParseSkip& other) {
   TRACE8 ("ParseSkip::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseSkip::operator= (other);

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameters and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK Requieres : pFoundValue != 0; len == 0
//-----------------------------------------------------------------------------
int CBParseSkip::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue); Check1 (!len);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseAttomic::~CBParseAttomic () {
   TRACE9 ("CBParseAttomic::~CBParseAttomic: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseAttomic& CBParseAttomic::operator= (const CBParseAttomic& other) {
   TRACE8 ("ParseAttomic::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseAttomic::operator= (other);

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameter and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int CBParseAttomic::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseText::~CBParseText () {
   TRACE9 ("CBParseText::~CBParseText: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseText& CBParseText::operator= (const CBParseText& other) {
   TRACE8 ("ParseText::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseText::operator= (other);

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameter and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int CBParseText::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseTextEsc::~CBParseTextEsc () {
   TRACE9 ("CBParseTextEsc::~CBParseTextEsc: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseTextEsc& CBParseTextEsc::operator= (const CBParseTextEsc& other) {
   TRACE8 ("ParseTextEsc::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseTextEsc::operator= (other);

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameter and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int CBParseTextEsc::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseQuoted::~CBParseQuoted () {
   TRACE9 ("CBParseQuoted::~CBParseQuoted () - " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseQuoted& CBParseQuoted::operator= (const CBParseQuoted& other) {
   TRACE8 ("ParseQuoted::operator= (const CBParseQuoted&) - " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseQuoted::operator= (other);

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameter and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int CBParseQuoted::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseQuotedEsc::~CBParseQuotedEsc () {
   TRACE9 ("CBParseQuotedEsc::~CBParseQuotedEsc () - " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseQuotedEsc& CBParseQuotedEsc::operator= (const CBParseQuotedEsc& other) {
   TRACE8 ("ParseQuotedEsc::operator= (const CBParseQuotedEsc&) - " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseQuotedEsc::operator= (other);

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameter and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int CBParseQuotedEsc::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseExact::~CBParseExact () {
   TRACE9 ("CBParseExact::~CBParseExact: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseExact& CBParseExact::operator= (const CBParseExact& other) {
   TRACE8 ("ParseExact::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseExact::operator= (other);

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameter and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int CBParseExact::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseUpperExact::~CBParseUpperExact () {
   TRACE9 ("CBParseUpperExact::~CBParseUpperExact: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseUpperExact& CBParseUpperExact::operator= (const CBParseUpperExact& other) {
   TRACE8 ("CBParseUpperExact::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseUpperExact::operator= (other);

   Check2 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameter and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int CBParseUpperExact::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseSequence::~CBParseSequence () {
   TRACE9 ("CBParseSequence::~CBParseSequence: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseSequence& CBParseSequence::operator= (const CBParseSequence& other) {
   TRACE8 ("ParseSequence::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseSequence::operator= (other);

   Check2 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Calls the defined callback with the passed parameter and returns its
/// result.
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int CBParseSequence::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue);
   return pCallback (pFoundValue, len);
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
CBParseSelection::~CBParseSelection () {
   TRACE9 ("CBParseSelection::~CBParseSelection: " << getDescription ());
}

//-----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to clone
/// \returns \c Reference to this
//-----------------------------------------------------------------------------
CBParseSelection& CBParseSelection::operator= (const CBParseSelection& other) {
   TRACE8 ("CBParseSelection::operator=: " << getDescription ());
   Check1 (!other.checkIntegrity ());

   pCallback = other.pCallback; Check3 (pCallback);
   ParseSelection::operator= (other);

   Check3 (!checkIntegrity ());
   return *this;
}

//-----------------------------------------------------------------------------
/// Callback if an object was found
/// \param pFoundValue: Pointer to found value
/// \param len: Length of found data
/// \returns \c int: Status; 0 OK
//-----------------------------------------------------------------------------
int CBParseSelection::found (const char* pFoundValue, unsigned int len) {
   Check1 (pCallback); Check1 (pFoundValue);
   return pCallback (pFoundValue, len);
}

}
