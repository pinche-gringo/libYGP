#ifndef PARSE_H
#define PARSE_H

//$Id: Parse.h,v 1.10 1999/11/04 20:43:11 Markus Rel $

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


// Note: If this module is compiled with MULTIBUFFER defined, every instance
//       of ParseAttomic uses its own buffer for parsing; else all instances
//       shares the same buffer.

#include <assert.h>

#include <string>
#include <iostream.h>

#include "XStream.h"


// Prototype of callback-function (called if an object is found).
// The return-value specifies how the parsing should be continued:
// 0 ..... Parsing (and callback OK)
// > 0 ... Error while parsing; parsing can be continued (in sequences, ...)
// < 1 ... Error while parsing; abort parsing (unrecoverable error)
// If possible use the error-values in the error-enum
typedef int (*PARSECALLBACK)(const char*);

// Base-class for parsing objects
// Note: None of the member-pointers is freed!
class ParseObject {
 public:
   // Manager-functions
   ParseObject (const char* description, PARSECALLBACK callback = NULL,
                bool skipWhitespace = true);
   ParseObject (const ParseObject& other);
   virtual ~ParseObject ();

   const ParseObject& operator= (const ParseObject& other);

   static void setParseErrorStream (ostream& err) { error = err; }

   // Accessing values
   const char* getDescription () const { return pDescription; }

   void setDescription (const char* desc) { assert (desc); pDescription = desc; }
   void setCallback (PARSECALLBACK pCB) { assert (pCB); pCallback = pCB; }
   void setSkipWS (bool skipWhitespace) { skip = skipWhitespace; }

   // Parsing
   void skipWS (Xistream& stream) const;
   int  parse (Xistream& stream) {
      assert (!checkIntegrity ());
      return doParse (stream, false); }
   virtual int doParse (Xistream& stream, bool optional) = 0;

   enum { PARSE_OK = 0, PARSE_ERROR = 1, PARSE_CB_ERROR = 2,
          PARSE_CB_ABORT = -1 };

   // Possible errors of checkIntegrity
   enum { OK = 0, NO_DESCRIPTION, LAST };

 protected:
   virtual int checkIntegrity () const;

   PARSECALLBACK pCallback;     // Protected to enable sub-classes easy access

   static _IO_ostream_withassign& error;

 private:
   // Prohibited manager functions
   ParseObject ();

   const char* pDescription;
   bool        skip;
};


// Class to check if EOF is parsed
class ParseEOF : public ParseObject {
 public:
   ParseEOF () : ParseObject ("EOF", NULL, false) { }

   virtual int doParse (Xistream& stream, bool) { return stream.eof (); }
};

// Class to parse a attomic value; Base-class of all attomic values
// The pValue parameter has the following semantics:
//    A parsed character is valid if it occures in the pValue-list,
//    where the backslash (\) escapes the next charater to the following
//    meaning:
//       A ... Any alphabethic character is valid
//       X ... Any alphanumeric character is valid
//       9 ... Any number is valid
//       0 ... The zero-character (\0) is valid
//       \ ... The backslash itself is valid
//       blank ( ) ... Any whitespace is valid
//       * ... Any character is valid
//
// Does this remind you at COBOL (at least a wee bit?) Yup, I've to admit
// I was tortured with this bullshit when I was young and dynamic (centuries
// ago). Who'd imagine that I could use that somehow?
class ParseAttomic : public ParseObject {
 public:
   // Manager-functions
   ParseAttomic (const char* value, const char* description,
                 unsigned int max = 1, unsigned min = 1,
                 PARSECALLBACK callback = NULL, bool skipWhitespace = true);
   ParseAttomic (const ParseAttomic& other);
   virtual ~ParseAttomic ();

   const ParseAttomic& operator= (const ParseAttomic& other);

   // Accessing values
   const char*  getValue () const { return pValue; }
   unsigned int getMaxCard () const { return maxCard; }
   unsigned int getMinCard () const { return minCard; }

   void setMaxCard (unsigned int val) { maxCard = val; }
   void setMinCard (unsigned int val) { minCard = val; }
   void setValue (const char* value) {assert (value); pValue = value; }

#ifdef MULTIBUFFER
   static void freeBuffer () { }
#else
   static void freeBuffer ();
#endif

   // Possible errors of checkIntegrity
   enum { MAX_MIN_ERROR = ParseObject::LAST, NO_VALUE, LAST };

 protected:
   const char*  pValue;         // Protected to enable sub-classes easy access

   unsigned int maxCard;
   unsigned int minCard;

   virtual int  checkIntegrity () const;
   virtual bool checkValue (char ch);

   // Parsing
   virtual int doParse (Xistream& stream, bool optional);

 private:
   // Prohibited manager functions
   ParseAttomic ();
};


// Class to parse text til a certain abort-criteria. Parsing of this element
// is stopped, if any of the characters in the abort-parameters is reached
// (or the maximal cardinality is reached).
class ParseText : public ParseAttomic {
 public:
   // Manager-functions
   ParseText (const char* abort, const char* description,
              unsigned int max, unsigned min = 1,
              PARSECALLBACK callback = NULL, bool skipWhitespace = true)
      : ParseAttomic (abort, description, max, min, callback, skipWhitespace) { }
   ParseText (const ParseText& other) : ParseAttomic (other) { }
   ~ParseText () { }

   const ParseText& operator= (const ParseText& other) {
      return (const ParseText&)ParseAttomic::operator= (other); }

 protected:
   virtual bool checkValue (char ch);
};


// Class to parse text til a certain abort-criteria (as in ParseText). This
// abort-characters can be escaped
class ParseTextEsc : public ParseText {
 public:
   // Manager-functions
   ParseTextEsc (const char* abort, const char* description,
                 unsigned int max, unsigned min = 1, char escape = '\\',
                 PARSECALLBACK callback = NULL, bool skipWhitespace = true);
   ParseTextEsc (const ParseTextEsc& other);
   ~ParseTextEsc () { }

   const ParseTextEsc& operator= (const ParseTextEsc& other);

 protected:
   virtual bool checkValue (char ch);

   char esc;
   char last;
};


// Class to parse exactly a certain text (case-sensitive!)
// The min/max-parameters of the second constructor may seem a wee bit
// useless, but with them its possible to parse for example an ID where just
// the first min chars must match (PARAMETER or PARAM or ...). Well,  the main
// reason for this behaviour is, that I don't want to re-consider the
// class-hierarchy anymore!
// Note: This class uses strlen to get the length of value so don't use it to
// check for text with a '\0' inside!
class ParseExact : public ParseAttomic {
 public:
   // Manager-functions
   ParseExact (const char* value, const char* description,
               PARSECALLBACK callback = NULL, bool skipWhitespace = true);
   ParseExact (const char* value, const char* description,
               unsigned int max, unsigned min,
               PARSECALLBACK callback = NULL, bool skipWhitespace = true)
      : ParseAttomic (value, description, max, min, callback, skipWhitespace)
      , pos (0) { }
   ParseExact (const ParseExact& other) : ParseAttomic (other), pos (0) { }
   virtual ~ParseExact () { }

   const ParseExact& operator= (const ParseExact& other);

   // Possible errors of checkIntegrity
   enum { POS_ERROR = ParseAttomic::LAST, LAST };

 protected:
   virtual int  checkIntegrity () const;
   virtual bool checkValue (char ch);

 private:
   unsigned int pos;
};


// Class to parse exactly a certain text (not case-sensitive!)
// The min/max-parameters of the second constructor may seem a wee bit
// useless, but with them its possible to parse for example an ID where just
// the first min chars must match (PARAMETER or PARAM or ...). Well, I've to
// admit, the main reason for this behaviour is, that I don't want to consider
// the class-hierarchy anymore!
// Note: This class uses strlen to get the length of value so don't use it to
// check for text with a '\0' inside!
class ParseUpperExact : public ParseExact {
 public:
   // Manager-functions
   ParseUpperExact (const char* value, const char* description,
                    PARSECALLBACK callback = NULL, bool skipWhitespace = true)
      : ParseExact (value, description, callback, skipWhitespace) { }
   ParseUpperExact (const char* value, const char* description,
                    unsigned int max, unsigned min,
                    PARSECALLBACK callback = NULL, bool skipWhitespace = true)
      : ParseExact (value, description, max, min, callback, skipWhitespace) { }
   ParseUpperExact (const ParseExact& other) : ParseExact (other) { }
   virtual ~ParseUpperExact () { }

   const ParseUpperExact& operator= (const ParseExact& other) {
      return (const ParseUpperExact&)ParseExact::operator= (other); }

 protected:
   // Possible errors of checkIntegrity
   enum { VALUE_NOT_UPPERCASE = ParseExact::LAST, LAST };

   virtual bool checkValue (char ch);
   virtual int  checkIntegrity () const;
};


// Class to parse sequences (series of ParseObjects). Every ParseObject
// in this list must be found (in the same order).
class ParseSequence : public ParseObject {
 public:
   ParseSequence (ParseObject* apObjectList[], const char* description,
                  unsigned int max = 1, unsigned min = 1,
                  PARSECALLBACK callback = NULL, bool skipWhitespace = true);
   ParseSequence (const ParseSequence& other);
   ~ParseSequence ();

   const ParseSequence& operator= (const ParseSequence& other);

   // Accessing values
   unsigned int getMaxCard () const { return maxCard; }
   unsigned int getMinCard () const { return minCard; }

   void setMaxCard (unsigned int val) { maxCard = val; }
   void setMinCard (unsigned int val) { minCard = val; }

   // Possible errors of checkIntegrity
   enum { MAX_MIN_ERROR = ParseObject::LAST, INVALID_LIST, LAST };

 protected:
   virtual int checkIntegrity () const;

   void writeError (Xistream& stream) const {
      error << "Expected '" << getDescription () << "'\n";
   }

   // Parsing
   virtual int doParse (Xistream& stream, bool optional);

   ParseObject** ppList;

   unsigned int maxCard;
   unsigned int minCard;

private:
   // Prohibited manager functions
   ParseSequence ();
};


// Class to parse selections (list of ParseObjects where just one entry must
// be valid). Every ParseObject).
class ParseSelection : public ParseSequence {
 public:
   ParseSelection (ParseObject* apObjectList[], const char* description,
                   unsigned int max = 1, unsigned min = 1,
                   PARSECALLBACK callback = NULL, bool skipWhitespace = true);
   ParseSelection (const ParseSelection& other);
   ~ParseSelection ();

   const ParseSelection& operator= (const ParseSelection& other);

 protected:
   // Parsing
   virtual int doParse (Xistream& stream, bool optional);

 private:
   // Prohibited manager functions
   ParseSelection ();
};

#endif
