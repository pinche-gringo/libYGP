#ifndef PARSE_H
#define PARSE_H

//$Id: Parse.h,v 1.3 1999/08/25 22:40:21 Markus Exp $

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


// Notes: If this module is compiled with MULTIBUFFER defined, all instances
//        of ParseAttomic uses just one buffer for parsing; else every
//        instance creates a buffer for parsing

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
typedef int (*PFNCALLBACK)(const char*);

// Base-class for parsing objects
// Note: None of the member-pointers is freed!
class ParseObject {
 public:
   // Manager-functions
   ParseObject (const char* description, unsigned int max = 1,
                unsigned int min = 1, PFNCALLBACK callback = NULL,
                bool skipWhitespace = true);
   ParseObject (const ParseObject& other);
   virtual ~ParseObject ();

   const ParseObject& operator= (const ParseObject& other);
   
   // Accessing values
   const char*  getDescription () const { return pDescription; }
   unsigned int getMaxCard () const { return maxCard; }
   unsigned int getMinCard () const { return minCard; }

   void setDescription (const char* desc) { assert (desc); pDescription = desc; }
   void setMaxCard (unsigned int val) { maxCard = val; }
   void setMinCard (unsigned int val) { minCard = val; }
   void setCallback (PFNCALLBACK pCB) { assert (pCB); pCallback = pCB; }
   void setSkipWS (bool skipWhitespace) { skip = skipWhitespace; }

   // Parsing
   void skipWS (Xistream& stream) const;
   int  parse (Xistream& stream) const {
      assert (!checkIntegrity ());
      return doParse (stream, false); }

   enum { PARSE_OK = 0, PARSE_ERROR = 1, PARSE_CB_ERROR = 2,
          PARSE_CB_ABORT = -1 };

   // TODO: Should be protected; but EGCS has a different optinion?!
   virtual int doParse (Xistream& stream, bool optional) const = 0;

 protected:
   virtual int checkIntegrity () const;

   PFNCALLBACK pCallback;       // Protected to enable sub-classes easy access

   unsigned int maxCard;
   unsigned int minCard;

   static ostream& error;

 private:
   // Prohibited manager functions
   ParseObject ();

   const char* pDescription;
   bool        skip;
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
// Does this remain you at COBOL (at least a wee bit?) Yes, I've to admit
// I was tortured with this bullshit when I was young and dynamic (centuries
// ago). Who'd imagine that I could use that somehow?
class ParseAttomic : public ParseObject {
 public:
   // Manager-functions
   ParseAttomic (const char* value, const char* description,
                 unsigned int max = 1, unsigned min = 1,
                 PFNCALLBACK callback = NULL, bool skipWhitespace = true);
   ParseAttomic (const ParseAttomic& other);
   virtual ~ParseAttomic ();

   const ParseAttomic& operator= (const ParseAttomic& other);
   
   // Accessing values
   const char*  getValue () const { return pValue; }
   void setValue (const char* value) {assert (value); pValue = value; }
  
#ifdef MULTIBUFFER
   static void freeBuffer () { }
#else
   static void freeBuffer ();
#endif

 protected:
   const char*  pValue;         // Protected to enable sub-classes easy access

   virtual int  checkIntegrity () const;
   virtual bool checkValue (char ch) const;

   // Parsing
   virtual int doParse (Xistream& stream, bool optional) const;

 private:
   // Prohibited manager functions
   ParseAttomic ();
};


// Class to parse sequences (series of ParseObjects). Every ParseObject
// in this list must be found (in the same order).
class ParseSequence : public ParseObject {
 public:
   ParseSequence (const ParseObject* apObjectList[], const char* description,
                  unsigned int max = 1, unsigned min = 1,
                  PFNCALLBACK callback = NULL, bool skipWhitespace = true);
   ParseSequence (const ParseSequence& other);
   ~ParseSequence ();

   const ParseSequence& operator= (const ParseSequence& other);

 protected:
   virtual int  checkIntegrity () const;

   // Parsing
   virtual int doParse (Xistream& stream, bool optional) const;

 private:
   // Prohibited manager functions
   ParseSequence ();

   const ParseObject** ppList;
};

#endif
