#ifndef PARSE_H
#define PARSE_H

//$Id: Parse.h,v 1.28 2002/12/14 22:46:14 markus Rel $

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


#include <string>
#include <iostream>

#include <Check.h>
#include <XStream.h>


// Classes to parse objects and sequences of objects from a stream. If an
// object is found it is possible to perform an action for this event.
//
// There are 3 kind of parsing objects: Attomic ones, which enables the
// parsing of some kind of data; special ones, to perform special tasks
// and sequences to "glue" together a set of objects.
//
// The following attomic objects exists:
//   - ParseAttomic: Parses a specified number of specified characters.
//   - ParseText: Parses a specified number of characters until one of
//        specified characters is found
//   - ParseTextEsc: As ParseText, but this terminating characters can be
//        escaped
//   - ParseExact: Parses (a part) of the specified characters in that sequence
//   - ParseUpperExact: As ParseExact, but ignores the case
// 
// The following special objects exists:
//   - ParseEOF: "Parses" or matches the end of the file
//   - ParseSkip: Skips the specified number of characters (from the current
//        position).
//
// The following sequences exists:
//   - ParseSequence: All of its children must be found in that order.
//   - ParseSelection: One of its children must be found; stopps parsing if
//        one matches.
//
// All of these classes exists in 2 variants:
//   - The first variant reports matching objects with a virtual function
//     called "found". These classes have the prefix Parse.
//   - The second calls a C-style function if an object was found and has
//     therefore a function-pointer as parameter of the constructor(s). These
//     classes starts with CBParse.
//   - The third calls a C++-style function and has therefore an object and a
//     pointer to a memberfunction as parameter of the constructor(s). These
//     classes starts with OFParse.
//
// Any of this functions must return the following values (which specifies
// how the parsing should be continued):
// 0 ..... Parsing (actually callback) OK
// > 0 ... Error in callback; parsing can be continued (in sequences, ...)
// < 1 ... Error in callback; abort parsing (unrecoverable error)
// If possible use the error-values in the error-enum

// First part: Classes reporting their found-status with a virtual function

// Base-class for parsing objects
// Note: The passed const char*-pointers are only stored and not copied, so
//       ensure that they are valid during the lifetime of each parse-object!
class ParseObject {
 public:
   // Manager-functions
   ParseObject (const char* description, bool skipWhitespace = true);
   ParseObject (const ParseObject& other);
   virtual ~ParseObject ();

   ParseObject& operator= (const ParseObject& other);

   // Accessing values
   const char* getDescription () const { return pDescription; }

   void setDescription (const char* desc) { Check1 (desc); pDescription = desc; }
   void setSkipWS (bool skipWhitespace) { skip = skipWhitespace; }

   // Parsing
   void skipWS (Xistream& stream) const;
   int  parse (Xistream& stream) throw (std::string) {
      Check1 (!checkIntegrity ());
      return doParse (stream, false); }
   virtual int doParse (Xistream& stream, bool optional) = 0;
   virtual int found (const char* pFoundValue, unsigned int);

   enum { PARSE_OK = 0, PARSE_ERROR = 1, PARSE_CB_ERROR = 2,
          PARSE_CB_ABORT = -1 };

   // Possible errors of checkIntegrity
   enum { OK = 0, NO_DESCRIPTION, LAST };

   static void freeBuffer ();

 protected:
   virtual int checkIntegrity () const;

   bool        skip;

 private:
   // Prohibited manager functions
   ParseObject ();

   const char* pDescription;
};


// Class to parse the end of a file (EOF)
class ParseEOF : public ParseObject {
 public:
   ParseEOF () : ParseObject ("EOF", false) { }
   virtual ~ParseEOF ();

   virtual int doParse (Xistream& stream, bool) { return !stream.eof (); }

 private:
   ParseEOF (const ParseEOF&);                 // Not very usefull -> prohibit
   ParseEOF& operator= (const ParseEOF&);
};


// Class to skip some characters (from the current position in the stream).
class ParseSkip : public ParseObject {
 public:
   ParseSkip (unsigned int bytes) : ParseObject ("Skip", false), offset (bytes) { }
   ParseSkip (const ParseSkip& other) : ParseObject (other), offset (other.offset) { }
   virtual ~ParseSkip ();

   ParseSkip& operator= (const ParseSkip& other) {
      if (this != &other)
         offset = other.offset;
      return *this; }

   virtual int doParse (Xistream& stream, bool) {
      stream.seekg (offset, ios::cur);
      return PARSE_OK; }

   void setOffset (unsigned int val) { offset = val; }
   unsigned int getOffset () const { return offset; }

 private:
   unsigned int offset;
};


// Class to parse an attomic value; Base-class of all attomic values
// The pValue parameter has the following semantic:
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
// If this remind you at COBOL (at least a wee bit) - well, I've to admit
// I was tortured with this crap when I was young and dynamic (centuries
// ago). Who'd imagine that I could use that somehow?
//
// The members maxCard and minCard specify how many characters the object
// can or must have. If there are less matching characters parsed, the object
// is considered as not found; after reaching the upper border, parsing is
// stopped for this object.
//
// skipWhitespace does exactly that, after an object was parsed successfully.
//
// The parameter reportData specifies, if the parsed data should be stored and
// passed via the found-method. Setting to false disables also putting back
// previously parsed data, so only do that, if parsing always succeeds and you
// don't care about the parsed contents!
class ParseAttomic : public ParseObject {
 public:
   // Manager-functions
   ParseAttomic (const char* value, const char* description,
                 unsigned int max = 1, unsigned int min = 1,
                 bool skipWhitespace = true, bool reportData = true);
   ParseAttomic (const ParseAttomic& other);
   virtual ~ParseAttomic ();

   ParseAttomic& operator= (const ParseAttomic& other);

   // Accessing values
   const char*  getValue () const { return pValue; }
   unsigned int getMaxCard () const { return maxCard; }
   unsigned int getMinCard () const { return minCard; }

   void setMaxCard (unsigned int val) { maxCard = val; }
   void setMinCard (unsigned int val) { minCard = val; }
   void setValue (const char* value) {Check1 (value); pValue = value; }

   // Possible errors of checkIntegrity
   enum { MAX_MIN_ERROR = ParseObject::LAST, NO_VALUE, LAST };

 protected:
   const char*  pValue;         // Protected to enable sub-classes easy access

   unsigned int maxCard;
   unsigned int minCard;

   virtual int  checkIntegrity () const;
   virtual int checkValue (char ch);

   // Parsing
   virtual int doParse (Xistream& stream, bool optional) throw (std::string);

 private:
   // Prohibited manager functions
   ParseAttomic ();

   bool report;
};


// Class to parse text until specified abot-characters are found.
//
// Parsing of this element is stopped, if any of the characters in the
// abort-parameters is found (or the maximal cardinality is reached).
class ParseText : public ParseAttomic {
 public:
   // Manager-functions
   ParseText (const char* abort, const char* description,
              unsigned int max, unsigned int min = 1,
              bool skipWhitespace = true, bool reportData = true)
      : ParseAttomic (abort, description, max, min, skipWhitespace, reportData) { }
   ParseText (const ParseText& other) : ParseAttomic (other) { }
   virtual ~ParseText ();

   ParseText& operator= (const ParseText& other) {
      return (ParseText&)ParseAttomic::operator= (other); }

 protected:
   virtual int checkValue (char ch);

 private:
   // Prohibited manager functions
   ParseText ();
};


// Class to parse text til a certain abort-criteria (as in
// ParseText). However, parsing is continued if those characters are
// preceeded by a escape-character (which "escapes" the special
// meaning of that character).
//
// Parsing of this element is stopped, if any of the characters in the
// abort-parameters is found (or the maximal cardinality is reached).
//
// This class enables parsing analogue to strings in C. A C-string is
// terminated with a quote ("), except if the quote follows a single
// backslash (\).  The only difference is that both quote and
// backslash can be specified and therefor be every charcter.
class ParseTextEsc : public ParseText {
 public:
   // Manager-functions
   ParseTextEsc (const char* abort, const char* description,
                 unsigned int max, unsigned int min = 1, char escape = '\\',
                 bool skipWhitespace = true, bool reportData = true);
   ParseTextEsc (const ParseTextEsc& other);
   virtual ~ParseTextEsc ();

   ParseTextEsc& operator= (const ParseTextEsc& other);

 protected:
   virtual int checkValue (char ch);

   char esc;
   char last;
   char prelast;

 private:
   // Prohibited manager functions
   ParseTextEsc ();
};


// Class to parse exactly a certain text (case-sensitive!)
//
// The min- and max-members are not totally wasted; they could be used
// to define the minimal length of certain keywords which would be
// valid even if just written in a short(er) form and/or to include values with
// a zero ('\0') character (although the main reason for them is that I didn't
// want  re-consider the class-hierarchy anymore).
//
// Note: This class uses strlen to get the length of its value so don't use its
// first constructor to check for text with a '\0' inside!
class ParseExact : public ParseAttomic {
 public:
   // Manager-functions
   ParseExact (const char* value, const char* description,
               bool skipWhitespace = true, bool reportData = true);
   ParseExact (const char* value, const char* description,
               unsigned int max, unsigned int min, bool skipWhitespace = true,
               bool reportData = true)
      : ParseAttomic (value, description, max, min, skipWhitespace, reportData)
      , pos (0) { }
   ParseExact (const ParseExact& other) : ParseAttomic (other), pos (0) { }
   virtual ~ParseExact ();

   ParseExact& operator= (const ParseExact& other);

   // Possible errors of checkIntegrity
   enum { POS_ERROR = ParseAttomic::LAST, LAST };

 protected:
   virtual int  checkIntegrity () const;
   virtual int checkValue (char ch);

 private:
   unsigned int pos;

   // Prohibited manager functions
   ParseExact ();
};


// Class to parse exactly a certain text (not case-sensitive!)
//
// The min- and max-members are not totally wasted; they could be used
// to define the minimal length of certain keywords which would be
// valid even if just written in a short(er) form and/or to include values with
// a zero ('\0') character (although the main reason for them is that I didn't
// want  re-consider the class-hierarchy anymore).
//
// Note: This class uses strlen to get the length of its value so don't use its
// first constructor to check for text with a '\0' inside!
class ParseUpperExact : public ParseExact {
 public:
   // Manager-functions
   ParseUpperExact (const char* value, const char* description,
                    bool skipWhitespace = true, bool reportData = true)
      : ParseExact (value, description, skipWhitespace, reportData) { }
   ParseUpperExact (const char* value, const char* description,
                    unsigned int max, unsigned int min,
                    bool skipWhitespace = true, bool reportData = true)
      : ParseExact (value, description, max, min, skipWhitespace, reportData) { }
   ParseUpperExact (const ParseUpperExact& other) : ParseExact (other) { }
   virtual ~ParseUpperExact ();

   ParseUpperExact& operator= (const ParseUpperExact& other) {
      return (ParseUpperExact&)ParseExact::operator= (other); }

 protected:
   // Possible errors of checkIntegrity
   enum { VALUE_NOT_UPPERCASE = ParseExact::LAST, LAST };

   virtual int checkValue (char ch);
   virtual int  checkIntegrity () const;

 private:
   // Prohibited manager functions
   ParseUpperExact ();
};


// Class to parse series of ParseObjects (sequences).
//
// A sequence is only considered parsed successfully, if all of its
// elements are parsed successfully (in the order specified by the
// sequence).
//
// Errors while parsing cause a soft error (meaning parsing can be
// continued) only for the first element; errors for further elements
// cause hard errors (which are not recoverable and parsing is ended).
class ParseSequence : public ParseObject {
 public:
   ParseSequence (ParseObject* apObjectList[], const char* description,
                  unsigned int max = 1, unsigned int min = 1,
                  bool skipWhitespace = true);
   ParseSequence (const ParseSequence& other);
   virtual ~ParseSequence ();

   ParseSequence& operator= (const ParseSequence& other);

   // Accessing values
   unsigned int getMaxCard () const { return maxCard; }
   unsigned int getMinCard () const { return minCard; }

   void setMaxCard (unsigned int val) { maxCard = val; }
   void setMinCard (unsigned int val) { minCard = val; }

   // Possible errors of checkIntegrity
   enum { MAX_MIN_ERROR = ParseObject::LAST, INVALID_LIST, LAST };

 protected:
   virtual int checkIntegrity () const;

   // Parsing
   virtual int doParse (Xistream& stream, bool optional) throw (std::string);

   ParseObject** ppList;

   unsigned int maxCard;
   unsigned int minCard;

private:
   // Prohibited manager functions
   ParseSequence ();
};


// Class to parse a selection of one ParseObject out of a list.
//
// If an object matching the parsed intput is found, the sequence is
// considered as parsed successfully.
class ParseSelection : public ParseSequence {
 public:
   ParseSelection (ParseObject* apObjectList[], const char* description,
                   unsigned int max = 1, unsigned int min = 1,
                   bool skipWhitespace = true);
   ParseSelection (const ParseSelection& other);
   virtual ~ParseSelection ();

   ParseSelection& operator= (const ParseSelection& other);

 protected:
   // Parsing
   virtual int doParse (Xistream& stream, bool optional) throw (std::string);

 private:
   // Prohibited manager functions
   ParseSelection ();
};


// Second part: Classes having an parameter for a callback-function

// Prototype of callback-function (called if an object is found).
// The return-value specifies how the parsing should be continued:
// 0 ..... Parsing (and callback OK)
// > 0 ... Error while parsing; parsing can be continued (in sequences, ...)
// < 1 ... Error while parsing; abort parsing (unrecoverable error)
// If possible use the error-values in the error-enum
typedef int (*PARSECALLBACK)(const char*, unsigned int);

// Class to check if EOF is parsed
class CBParseEOF : public ParseEOF {
 public:
   CBParseEOF (PARSECALLBACK callback) : ParseEOF (), pCallback (callback) {
      Check1 (pCallback); }
   virtual ~CBParseEOF ();

   void setCallback (PARSECALLBACK callback) { pCallback = callback; Check1 (pCallback); }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len);

 private:
   CBParseEOF (const CBParseEOF&);             // Not very usefull -> prohibit
   CBParseEOF& operator= (const CBParseEOF&);

   PARSECALLBACK pCallback;
};


// Class to skip some characters (from the current position in the stream).
class CBParseSkip : public ParseSkip {
 public:
   CBParseSkip (unsigned int bytes, PARSECALLBACK callback) : ParseSkip (bytes)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseSkip (const CBParseSkip& other) :  ParseSkip (other)
      , pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~CBParseSkip ();

   CBParseSkip& operator= (const CBParseSkip&);

   void setCallback (PARSECALLBACK callback) { pCallback = callback; Check1 (pCallback); }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len);

 private:
   PARSECALLBACK pCallback;
};


// Class to parse a attomic value with callback-function if object found
class CBParseAttomic : public ParseAttomic {
 public:
   // Manager-functions
   CBParseAttomic (const char* value, const char* description,
                   PARSECALLBACK callback, unsigned int max = 1,
                   unsigned int min = 1, bool skipWhitespace = true)
      : ParseAttomic (value, description, max, min, skipWhitespace, true)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseAttomic (const CBParseAttomic& other) :  ParseAttomic (other)
      , pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~CBParseAttomic ();

   CBParseAttomic& operator= (const CBParseAttomic& other);

   void setCallback (PARSECALLBACK callback) { pCallback = callback; Check1 (pCallback); }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len);

 private:
   // Prohibited manager functions
   CBParseAttomic ();

   PARSECALLBACK pCallback;
};


// Class to parse text til a certain abort-criteria with callback-found
// (called if an object was found)
class CBParseText : public ParseText {
 public:
   // Manager-functions
   CBParseText (const char* abort, const char* description, PARSECALLBACK callback,
                unsigned int max, unsigned int min = 1, bool skipWhitespace = true)
      : ParseText (abort, description, max, min, skipWhitespace, true)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseText (const CBParseText& other) : ParseText (other)
      , pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~CBParseText ();

   CBParseText& operator= (const CBParseText& other);

   void setCallback (PARSECALLBACK callback) { pCallback = callback; Check1 (pCallback); }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len);

 private:
   PARSECALLBACK pCallback;

   // Prohibited manager functions
   CBParseText ();
};


// Class to parse text til a certain abort-criteria (as in CBParseText). This
// abort-characters can be escaped
class CBParseTextEsc : public ParseTextEsc {
 public:
   // Manager-functions
   CBParseTextEsc (const char* abort, const char* description, PARSECALLBACK callback,
                   unsigned int max, unsigned int min = 1, char escape = '\\',
                   bool skipWhitespace = true)
      : ParseTextEsc (abort, description, max, min, escape, skipWhitespace, true)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseTextEsc (const CBParseTextEsc& other) : ParseTextEsc (other)
      , pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~CBParseTextEsc ();

   CBParseTextEsc& operator= (const CBParseTextEsc& other);

   void setCallback (PARSECALLBACK callback) { pCallback = callback; Check1 (pCallback); }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len);

 private:
   PARSECALLBACK pCallback;

   // Prohibited manager functions
   CBParseTextEsc ();
};


// Class to parse exactly a certain text (case-sensitive!). If an object is
// found the passed callback is called
class CBParseExact : public ParseExact {
 public:
   // Manager-functions
   CBParseExact (const char* value, const char* description, PARSECALLBACK callback,
                 bool skipWhitespace = true)
      : ParseExact (value, description, skipWhitespace, true)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseExact (const char* value, const char* description, PARSECALLBACK callback,
                 unsigned int max, unsigned int min, bool skipWhitespace = true)
      : ParseExact (value, description, max, min, skipWhitespace, true)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseExact (const CBParseExact& other) : ParseExact (other)
      , pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~CBParseExact ();

   CBParseExact& operator= (const CBParseExact& other);

   void setCallback (PARSECALLBACK callback) { pCallback = callback; Check1 (pCallback); }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len);

 private:
   PARSECALLBACK pCallback;

   // Prohibited manager functions
   CBParseExact ();
};


// Class to parse exactly a certain text (not case-sensitive!). When an object
// is found the passed callback is called.
class CBParseUpperExact : public ParseUpperExact {
 public:
   // Manager-functions
   CBParseUpperExact (const char* value, const char* description,
                      PARSECALLBACK callback, bool skipWhitespace = true)
      : ParseUpperExact (value, description, skipWhitespace, true)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseUpperExact (const char* value, const char* description,
                      PARSECALLBACK callback, unsigned int max, unsigned int min,
                      bool skipWhitespace = true)
      : ParseUpperExact (value, description, max, min, skipWhitespace, true)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseUpperExact (const CBParseUpperExact& other) : ParseUpperExact (other)
      , pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~CBParseUpperExact ();

   CBParseUpperExact& operator= (const CBParseUpperExact& other);

   void setCallback (PARSECALLBACK callback) { pCallback = callback; Check1 (pCallback); }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len);

 private:
   PARSECALLBACK pCallback;

   // Prohibited manager functions
   CBParseUpperExact ();
};


// Class to parse sequences (series of ParseObjects). Every ParseObject
// in this list must be found (in the same order).
class CBParseSequence : public ParseSequence {
 public:
   CBParseSequence (ParseObject* apObjectList[], const char* description,
                    PARSECALLBACK callback, unsigned int max = 1,
                    unsigned int min = 1, bool skipWhitespace = true)
      : ParseSequence (apObjectList, description, max, min, skipWhitespace)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseSequence (const CBParseSequence& other) : ParseSequence (other)
      , pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~CBParseSequence ();

   CBParseSequence& operator= (const CBParseSequence& other);

   void setCallback (PARSECALLBACK callback) { pCallback = callback; Check1 (pCallback); }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len);

 private:
   PARSECALLBACK pCallback;

   // Prohibited manager functions
   CBParseSequence ();
};


// Class to parse selections (list of ParseObjects where just one entry must
// be valid). Every ParseObject).
class CBParseSelection : public ParseSelection {
 public:
   CBParseSelection (ParseObject* apObjectList[], const char* description,
                     PARSECALLBACK callback, unsigned int max = 1,
                     unsigned int min = 1, bool skipWhitespace = true)
      : ParseSelection (apObjectList, description, max, min, skipWhitespace)
      , pCallback (callback) { Check1 (pCallback); }
   CBParseSelection (const CBParseSelection& other) : ParseSelection (other)
      , pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~CBParseSelection ();

   CBParseSelection& operator= (const CBParseSelection& other);

   void setCallback (PARSECALLBACK callback) { pCallback = callback; Check1 (pCallback); }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len);

 private:
   PARSECALLBACK pCallback;

   // Prohibited manager functions
   CBParseSelection ();
};


// Third part: Classes having parameters for an object and a member-function
//             of this object as callback-values

// The return-value specifies how the parsing should be continued:
// 0 ..... Parsing (and callback OK)
// > 0 ... Error while parsing; parsing can be continued (in sequences, ...)
// < 1 ... Error while parsing; abort parsing (unrecoverable error)
// If possible use the error-values in the error-enum

// Class to check if EOF is parsed
template <class T> class OFParseEOF : public ParseEOF {
   typedef int (T::*PTCALLBACK)(const char*, unsigned int);

 public:
   OFParseEOF (T& objToNotify, PTCALLBACK callback) : ParseEOF ()
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   virtual ~OFParseEOF () { }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len) {
      Check1 (pCallback); Check1 (pFoundValue);
      return (object.*pCallback) (pFoundValue, len); }

 private:
   OFParseEOF (const OFParseEOF&);             // Not very usefull -> prohibit
   OFParseEOF& operator= (const OFParseEOF&);

   T&         object;
   PTCALLBACK pCallback;
};


// Class to skip some characters (from the current position in the stream)
template <class T> class OFParseSkip : public ParseSkip {
   typedef int (T::*PTCALLBACK)(const char*, unsigned int);

 public:
   OFParseSkip (unsigned int bytes, T& objToNotify, PTCALLBACK callback)
      : ParseSkip (), object (objToNotify), pCallback (callback) {
      Check1 (pCallback); }
   OFParseSkip (const OFParseSkip&) : ParseSkip (other), object (other.object)
      , pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~OFParseSkip () { }

   OFParseSkip& operator= (const OFParseSkip&) {
      if (this != &other) {
         pCallback = other.pCallback; Check1 (pCallback);
	 ParseSkip::operator= (other);
      }
      return *this; }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len) {
      Check1 (pCallback); Check1 (pFoundValue);
      return (object.*pCallback) (pFoundValue, len); }

 private:
   T&         object;
   PTCALLBACK pCallback;
};


// Class to parse a attomic value with callback-function if object found
template <class T> class OFParseAttomic : public ParseAttomic {
   typedef int (T::*PTCALLBACK)(const char*, unsigned int);

 public:
   // Manager-functions
   OFParseAttomic (const char* value, const char* description, T& objToNotify,
                   PTCALLBACK callback, unsigned int max = 1,
                   unsigned int min = 1, bool skipWhitespace = true)
      : ParseAttomic (value, description, max, min, skipWhitespace, true)
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   OFParseAttomic (const OFParseAttomic& other) :  ParseAttomic (other)
      , object (other.object), pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~OFParseAttomic () { }

   OFParseAttomic& operator= (const OFParseAttomic& other) {
      if (this != &other) {
         pCallback = other.pCallback; Check1 (pCallback);
	 ParseAttomic::operator= (other);
      }
      return *this; }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len) {
      Check1 (pCallback); Check1 (pFoundValue);
      return (object.*pCallback) (pFoundValue, len); }

 private:
   // Prohibited manager functions
   OFParseAttomic ();

   T&         object;
   PTCALLBACK pCallback;
};


// Class to parse text til a certain abort-criteria with callback-found
// (called if an object was found)
template <class T> class OFParseText : public ParseText {
   typedef int (T::*PTCALLBACK)(const char*, unsigned int);

 public:
   // Manager-functions
   OFParseText (const char* abort, const char* description,
		T& objToNotify, PTCALLBACK callback, unsigned int max,
                unsigned int min = 1, bool skipWhitespace = true)
      : ParseText (abort, description, max, min, skipWhitespace, true)
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   OFParseText (const OFParseText& other) : ParseText (other)
      , object (other.object), pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~OFParseText () { }

   OFParseText& operator= (const OFParseText& other) {
      if (this != &other) {
         pCallback = other.pCallback; Check1 (pCallback);
	 ParseText::operator= (other);
      }
      return *this; }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len) {
      Check1 (pCallback); Check1 (pFoundValue);
      return (object.*pCallback) (pFoundValue, len); }

 private:
   PTCALLBACK pCallback;
   T&         object;

   // Prohibited manager functions
   OFParseText ();
};


// Class to parse text til a certain abort-criteria (as in OFParseText). This
// abort-characters can be escaped
template <class T> class OFParseTextEsc : public ParseTextEsc {
   typedef int (T::*PTCALLBACK)(const char*, unsigned int);

 public:
   // Manager-functions
   OFParseTextEsc (const char* abort, const char* description,
                   T& objToNotify, PTCALLBACK callback,
                   unsigned int max, unsigned int min = 1, char escape = '\\',
                   bool skipWhitespace = true)
      : ParseTextEsc (abort, description, max, min, escape, skipWhitespace, true)
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   OFParseTextEsc (const OFParseTextEsc& other) : ParseTextEsc (other)
      , object (other.object), pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~OFParseTextEsc () { }

   OFParseTextEsc& operator= (const OFParseTextEsc& other) {
      if (this != &other) {
         pCallback = other.pCallback; Check1 (pCallback);
	 ParseText::operator= (other);
      }
      return *this; }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len) {
      Check1 (pCallback); Check1 (pFoundValue);
      return (object.*pCallback) (pFoundValue, len); }

 private:
   T&         object;
   PTCALLBACK pCallback;

   // Prohibited manager functions
   OFParseTextEsc ();
};


// Class to parse exactly a certain text (case-sensitive!). If an object is
// found the passed callback is called
template <class T> class OFParseExact : public ParseExact {
   typedef int (T::*PTCALLBACK)(const char*, unsigned int);

 public:
   // Manager-functions
   OFParseExact (const char* value, const char* description, T& objToNotify,
                 PTCALLBACK callback, bool skipWhitespace = true)
      : ParseExact (value, description, skipWhitespace, true)
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   OFParseExact (const char* value, const char* description, T& objToNotify,
                 PTCALLBACK callback, unsigned int max, unsigned int min,
                 bool skipWhitespace = true)
      : ParseExact (value, description, max, min, skipWhitespace, true)
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   OFParseExact (const OFParseExact& other) : ParseExact (other)
      , object (other.object), pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~OFParseExact () { }

   OFParseExact& operator= (const OFParseExact& other) {
      if (this != &other) {
         pCallback = other.pCallback; Check1 (pCallback);
	 ParseText::operator= (other);
      }
      return *this; }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len) {
      Check1 (pCallback); Check1 (pFoundValue);
      return (object.*pCallback) (pFoundValue, len); }

 private:
   T&         object;
   PTCALLBACK pCallback;

   // Prohibited manager functions
   OFParseExact ();
};


// Class to parse exactly a certain text (not case-sensitive!). When an object
// is found the passed callback is called.
template <class T> class OFParseUpperExact : public ParseUpperExact {
   typedef int (T::*PTCALLBACK)(const char*, unsigned int);

 public:
   // Manager-functions
   OFParseUpperExact (const char* value, const char* description,
                      T& objToNotify, PTCALLBACK callback,
                      bool skipWhitespace = true)
      : ParseUpperExact (value, description, skipWhitespace, true)
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   OFParseUpperExact (const char* value, const char* description,
                      T& objToNotify, PTCALLBACK callback, unsigned int max,
                      unsigned int min, bool skipWhitespace = true)
      : ParseUpperExact (value, description, max, min, skipWhitespace, true)
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   OFParseUpperExact (const OFParseUpperExact& other) : ParseUpperExact (other)
      , object (other.object), pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~OFParseUpperExact () { }

   OFParseUpperExact& operator= (const OFParseUpperExact& other) {
      if (this != &other) {
         pCallback = other.pCallback; Check1 (pCallback);
	 ParseText::operator= (other);
      }
      return *this; }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len) {
      Check1 (pCallback); Check1 (pFoundValue);
      return (object.*pCallback) (pFoundValue, len); }

 private:
   T&         object;
   PTCALLBACK pCallback;

   // Prohibited manager functions
   OFParseUpperExact ();
};


// Class to parse sequences (series of ParseObjects). Every ParseObject
// in this list must be found (in the same order).
template <class T> class OFParseSequence : public ParseSequence {
   typedef int (T::*PTCALLBACK)(const char*, unsigned int);

 public:
   OFParseSequence (ParseObject* apObjectList[], const char* description,
                    T& objToNotify, PTCALLBACK callback, unsigned int max = 1,
                    unsigned int min = 1, bool skipWhitespace = true)
      : ParseSequence (apObjectList, description, max, min, skipWhitespace)
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   OFParseSequence (const OFParseSequence& other) : ParseSequence (other)
      , object (other.object), pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~OFParseSequence () { }

   OFParseSequence& operator= (const OFParseSequence& other) {
      if (this != &other) {
         pCallback = other.pCallback; Check1 (pCallback);
	 ParseText::operator= (other);
      }
      return *this; }

 protected:
   virtual int found (const char* pFoundValue, unsigned int len) {
      Check1 (pCallback); Check1 (pFoundValue);
      return (object.*pCallback) (pFoundValue, len); }

 private:
   T&         object;
   PTCALLBACK pCallback;

   // Prohibited manager functions
   OFParseSequence ();
};


// Class to parse selections (list of ParseObjects where just one entry must
// be valid). Every ParseObject).
template <class T> class OFParseSelection : public ParseSelection {
   typedef int (T::*PTCALLBACK)(const char*, unsigned int);

 public:
   OFParseSelection (ParseObject* apObjectList[], const char* description,
                     T& objToNotify, PTCALLBACK callback, unsigned int max = 1,
                     unsigned int min = 1, bool skipWhitespace = true)
      : ParseSelection (apObjectList, description, max, min, skipWhitespace, true)
      , object (objToNotify), pCallback (callback) { Check1 (pCallback); }
   OFParseSelection (const OFParseSelection& other) : ParseSelection (other)
      , object (other.object), pCallback (other.pCallback) { Check1 (pCallback); }
   virtual ~OFParseSelection () { }

   OFParseSelection& operator= (const OFParseSelection& other) {
      if (this != &other) {
         pCallback = other.pCallback; Check1 (pCallback);
	 ParseText::operator= (other);
      }
      return *this; }


 protected:
   virtual int found (const char* pFoundValue, unsigned int len) {
      Check1 (pCallback); Check1 (pFoundValue);
      return (object.*pCallback) (pFoundValue, len); }

 private:
   T&         object;
   PTCALLBACK pCallback;

   // Prohibited manager functions
   OFParseSelection ();
};

#endif
