// $Id: AByteArray.cpp,v 1.8 2002/11/19 21:49:30 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : AByteArray
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.8 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.3.2001
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
#include <string.h>

#include "Trace_.h"

#include "AByteArray.h"

const unsigned int AByteArray::MEMADD = 20;


/*--------------------------------------------------------------------------*/
//Purpose   : Copy-constructor
//Parameters: other: Object to copy
/*--------------------------------------------------------------------------*/
AByteArray::AByteArray (const AByteArray& other)
   : AttributValue ((const AttributValue&) other)
   , pValue (NULL), len (other.len)
   , allocated (other.len) {
   TRACE5 ("AByteArray::AByteArray (const AByteArray&)");

   if (len) {
      pValue = new char [len];
      memcpy (pValue, other.pValue, len);
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor from char-array
//Parameters: pSource: Pointer to the initial value
//            length: Length of the char-array to copy
//Requires  : pSource a valid pointer to at leasat length bytes reserved
/*--------------------------------------------------------------------------*/
AByteArray::AByteArray (const char* pSource, unsigned int length)
   : AttributValue (true), pValue (new char [length]), len (length)
   , allocated (length) {
   TRACE5 ("AByteArray::AByteArray (const char*, unsinged int)");

   assert (pSource);
   memcpy (pValue, pSource, len);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor from ASCIIZ-string
//Parameters: pSource: Pointer to ASCIIZ-string
//Requires  : pSource a valid ASCIIZ-string
/*--------------------------------------------------------------------------*/
AByteArray::AByteArray (const char* pSource) : AttributValue (pSource != NULL)
   , pValue (NULL), len (0), allocated (0) {
   TRACE5 ("AByteArray::AByteArray (const char*)");

   if (pSource) {
      pValue = new char [allocated = len = strlen (pSource)];
      memcpy (pValue, pSource, len);
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor from character
//Parameters: ch: Character to copy to object
/*--------------------------------------------------------------------------*/
AByteArray::AByteArray (char ch) : AttributValue (true)
   , pValue (new char [1]), len (1), allocated (1) {
   TRACE5 ("AByteArray::AByteArray (char)");
   *pValue = ch;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; the string is copied into the object
//Parameters: pSource: Pointer to std::string
/*--------------------------------------------------------------------------*/
AByteArray::AByteArray (const std::string& str) : AttributValue (true)
   , pValue (NULL), len (0), allocated (0) {
   TRACE5 ("AByteArray::AByteArray (const std::string&)");
   pValue = new char [allocated = len = str.length ()];
   memcpy (pValue, str.data (), len);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
AByteArray::~AByteArray () {
   delete [] pValue;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assigns rhs to this
//Parameters: rhs: Object to assign
//Returns   : AByteArray&: Reference to this
/*--------------------------------------------------------------------------*/
AByteArray& AByteArray::assign (const AByteArray& rhs) {
   TRACE5 ("AByteArray::assign (const AByteArray&)");

   if (&rhs != this) {
      AttributValue::operator= ((const AttributValue&) rhs);

      if (rhs.len > len) {
	 delete [] pValue;
	 pValue = new char [allocated = rhs.len];
	 assert (!checkIntegrity ());
      }
      len = rhs.len;
      memcpy (pValue, rhs.pValue, len);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assigns length bytes of the passed character array to the object
//Parameters: pSource: Character-array to assign
//            length: Length of character-array
//Returns   : AByteArray&: Reference to this
//Remarks   : pSource must not be NULL; the object is always defined afterwards
/*--------------------------------------------------------------------------*/
AByteArray& AByteArray::assign (const char* pSource, unsigned int length) {
   TRACE5 ("AByteArray::assign (const char*, unsigned int)");

   assert (pSource);
   setDefined ();

   len = length;
   if (allocated < len) {
      delete [] pValue;
      pValue = new char [allocated = len];
      assert (!checkIntegrity ());
   }
   memcpy (pValue, pSource, len);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assigns the passed character array to the object
//Parameters: pSource: Pointer to ASCIIZ-string to assign
//Returns   : AByteArray&: Reference to this
//Remarks   : If pSource is NULL, this is undefined
/*--------------------------------------------------------------------------*/
AByteArray& AByteArray::assign (const char* pSource) {
   TRACE5 ("AByteArray::assign(const char*)");

   if (pSource)
      assign (pSource, strlen (pSource));
   else
      undefine ();
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Defines the byte-array and sets the value to empty
/*--------------------------------------------------------------------------*/
void AByteArray::define () {
   TRACE5 ("AByteArray::define ()");

   delete [] pValue;
   pValue = new char [len = allocated = 1];
   *pValue = '\0';
   setDefined ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Undefines the byte-array and deletes its contents
/*--------------------------------------------------------------------------*/
void AByteArray::undefine () {
   TRACE5 ("AByteArray::undefine ()");

   delete [] pValue;
   pValue = NULL;
   len = allocated = 0;
   AttributValue::undefine ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends the contents of rhs to the object. The object is
//            afterwards defined if either of the two AByteArrays have been
//            defined.
//Parameters: pSource: Character array to append
//Requires  : pSource must be a valid pointer to a zero-terminated character
//                    array
//Returns   : AByteArray&: Reference to this
/*--------------------------------------------------------------------------*/
AByteArray& AByteArray::append (const char* pSource) {
   TRACE5 ("AByteArray::append (const char*)");

   if (pSource)
      return append (pSource, strlen (pSource));
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends length characters of the array pSource is pointing to to
//            the object. If length is at least one, the object is set to be
//            defined.
//Parameters: pSource: Character array to append
//            length: Number of characters to append
//Requires  : pSource must be a valid pointer to a zero-terminated character
//                    array
//Returns   : AByteArray&: Reference to this
/*--------------------------------------------------------------------------*/
AByteArray& AByteArray::append (const char* pSource, unsigned int length) {
   TRACE5 ("AByteArray::append (const char*, unsigned int)");

   assert (!checkIntegrity ());

   if (length) {
      assert (pSource);

      // Check if array is large enough to hold both values
      if (allocated < (len + length)) {
	 char* pNew = new char [allocated = len + length + MEMADD];
	 memcpy (pNew, pValue, len);
	 delete [] pValue;
	 pValue = pNew;
      }

      memcpy (pValue + len, pSource, length);
      len += length;
      setDefined ();
      assert (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the byte at the specified position
//Parameters: Position to retrieve
//Returns   : Character at position pos
/*--------------------------------------------------------------------------*/
char AByteArray::operator[] (unsigned int pos) const throw (std::out_of_range) {
   TRACE5 ("AByteArray::operator[] (unsigned int) const");
   assert (!checkIntegrity ());

   TRACE9 ("   -> len = " << len);
   TRACE9 ("   -> Value = " << pValue);
   if (pos < len)
      return pValue[pos];
   else
      throw (std::out_of_range ("AByteArray::operator[] (unsigned int) const"));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves a reference to the byte at the specified position
//Parameters: Position to retrieve
//Returns   : Character at position pos
/*--------------------------------------------------------------------------*/
char& AByteArray::operator[] (unsigned int pos) throw (std::out_of_range) {
   TRACE5 ("AByteArray::operator[] (unsigned int)");
   assert (!checkIntegrity ());

   TRACE9 ("   -> len = " << len);
   TRACE9 ("   -> Value = " << pValue);
   if (pos < len)
      return pValue[pos];
   else
      throw (std::out_of_range ("AByteArray::at (unsigned int)"));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves a reference to the byte at the specified position
//Parameters: Position to retrieve
//Returns   : Character at position pos
/*--------------------------------------------------------------------------*/
const char& AByteArray::at (unsigned int pos) const throw (std::out_of_range) {
   TRACE5 ("AByteArray::at (unsigned int) const");
   assert (!checkIntegrity ());

   if (pos < len)
      return pValue[pos];
   else
      throw (std::out_of_range ("AByteArray::at (unsigned int) const"));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves a reference to the byte at the specified position
//Parameters: Position to retrieve
//Returns   : Character at position pos
/*--------------------------------------------------------------------------*/
char& AByteArray::at (unsigned int pos) throw (std::out_of_range) {
   TRACE5 ("AByteArray::at (unsigned int)");
   assert (!checkIntegrity ());

   if (pos < len)
      return pValue[pos];
   else
      throw (std::out_of_range ("AByteArray::at (unsigned int)"));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the internal state of the object for integrity
//            The class has the following invariants:
//              - The length of the data is not larger than the allocated area
//              - If the length is not 0, a value must be allocated
//Returns   : int: 1 if the data is larger than the allocated memory,
//                 2 if the length of the data is not 0, but no memory is reserved
//                 0 if the object is integer
/*--------------------------------------------------------------------------*/
int AByteArray::checkIntegrity () const {
   return (len > allocated) ? 1 : (len && !pValue) ? 2 : 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Concatenates rhs to lhs and returns the result
//Parameters: lhs: Left-hand-side of concatenation
//            rhs: Right-hand-side of concatenation
//Returns   : AByteArray: Concatinated byte-array
/*--------------------------------------------------------------------------*/
AByteArray operator+ (const AByteArray& lhs, const AByteArray& rhs) {
   TRACE5 ("AByteArray::operator+ (const AByteArray&, const AByteArray&)");
   TRACE7 ("   -> lhs.len = " << lhs.len);
   TRACE7 ("   -> rhs.len = " << rhs.len);

   if (lhs.isDefined () || rhs.isDefined ()) {
    AByteArray temp (lhs.len + rhs.len);
    if (lhs.isDefined ())
       memcpy (temp.pValue, lhs.pValue, lhs.len);

    if (rhs.isDefined ())
       memcpy (temp.pValue + lhs.len, rhs.pValue, rhs.len);

    temp.len = lhs.len + rhs.len;
    TRACE7 ("   -> temp.len = " << temp.len);
    return temp;
   }
   else {
      AByteArray temp;
      return AByteArray ();
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Concatenates rhs to lhs and returns the result
//Parameters: lhs: Left-hand-side of concatenation
//            rhs: Right-hand-side of concatenation
//Returns   : AByteArray: The concatinated byte-array
//Requires  : lhs must be a valid pointer to a zero-terminated character array
/*--------------------------------------------------------------------------*/
AByteArray operator+ (const char* lhs, const AByteArray& rhs) {
   TRACE5 ("AByteArray::operator+ (const char*, const AByteArray&)");

   AByteArray temp (lhs);
   return temp.append (rhs);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Concatenates rhs to lhs and returns the result
//Parameters: lhs: Left-hand-side of concatenation
//            rhs: Right-hand-side of concatenation
//Returns   : AByteArray: Concatinated byte-array
/*--------------------------------------------------------------------------*/
AByteArray operator+ (char lhs, const AByteArray& rhs) {
   TRACE5 ("AByteArray::operator+ (char, const AByteArray&)");

   AByteArray temp (lhs);
   return temp.append (rhs);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Concatenates rhs to lhs and returns the result
//Parameters: lhs: Left-hand-side of concatenation
//            rhs: Right-hand-side of concatenation
//Returns   : AByteArray: Concatinated byte-array
/*--------------------------------------------------------------------------*/
AByteArray operator+ (const std::string& lhs, const AByteArray& rhs) {
   TRACE5 ("AByteArray::operator+ (const std::string&, const AByteArray&)");

   AByteArray temp (lhs);
   return temp.append (rhs);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Concatenates rhs to lhs and returns the result
//Parameters: lhs: Left-hand-side of concatenation
//            rhs: Right-hand-side of concatenation
//Returns   : AByteArray: Concatinated byte-array
/*--------------------------------------------------------------------------*/
AByteArray operator+ (const AByteArray& lhs, const std::string& rhs) {
   TRACE5 ("AByteArray::operator+ (const AByteArray&, const std::string&)");

   AByteArray temp (lhs);
   return temp.append (rhs);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Concatenates rhs to lhs and returns the result
//Parameters: lhs: Left-hand-side of concatenation
//            rhs: Right-hand-side of concatenation
//Returns   : AByteArray: Concatinated byte-array
//Requires  : rhs must be a valid pointer to a zero-terminated character array
/*--------------------------------------------------------------------------*/
AByteArray operator+ (const AByteArray& lhs, const char* rhs) {
   TRACE5 ("AByteArray::operator+ (const AByteArray&, const char*)");

   AByteArray temp (lhs);
   return temp.append (rhs);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Concatenates rhs to lhs and returns the result
//Parameters: lhs: Left-hand-side of concatenation
//            rhs: Right-hand-side of concatenation
//Returns   : AByteArray: Concatinated byte-array
/*--------------------------------------------------------------------------*/
AByteArray operator+ (const AByteArray& lhs, char rhs) {
   TRACE5 ("AByteArray::operator+ (const AByteArray&, char)");

   AByteArray temp (lhs);
   return temp.append (rhs);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares the values of two byte-arrays. Undefined values are
//            considered to be smaller than every value; if both values
//            are undefined, they are considered as equal
//Parameters: other: Object to compare
//Returns   : int; <0: this < other; >0: this > other; 0 else
/*--------------------------------------------------------------------------*/
int AByteArray::compare (const AByteArray& other) const {
   TRACE5 ("AByteArray::compare (const AByteArray&)");

   assert (!checkIntegrity ()); assert (!other.checkIntegrity ());

   // Both objects are defined -> Compare them
   if (isDefined ()) {
      if (other.isDefined ()) {
         TRACE6 ("   -> Comparing to defined values");
         TRACE9 ("   -> this = pValue = " << pValue);
         TRACE9 ("   -> other.pValue = " << other.pValue);

         int rc (memcmp (pValue, other.pValue,
                         (len > other.len) ? other.len : len));
         if (!rc)
            rc = len - other.len;        // Equal: Longer string is bigger
         return rc; 
      }
      else
         return 1;
   }
   return other.isDefined ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares the byte array with a character array. Undefined values
//            are considered to be smaller then any character array.
//Parameters: other: Character array to compare
//Returns   : int; <0: this < other; >0: other < this; 0 else
//Requires  : other must be a valid pointer to a zero-terminated character array
/*--------------------------------------------------------------------------*/
int AByteArray::compare (const char* other) const {
   TRACE5 ("AByteArray::compare (const char*)");

   assert (!checkIntegrity ());

   if (isDefined ()) {
      TRACE6 ("   -> Comparing two defined values");
      TRACE9 ("   -> this = pValue = " << pValue);
      TRACE9 ("   -> other = " << other);
      return other ? strncmp (pValue, other, len) : 1;
   }

   return other != NULL;             // NULL-pointer: Return equal; else bigger
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares the byte array with a character array. Undefined values
//            are considered to be smaller then any character array.
//Parameters: other: Character array to compare
//            length: Number of bytes to compare
//Returns   : int; <0: this < other; >0: other < this; 0 else
//Requires  : other must be a valid pointer to a character array with at
//            least length elements
/*--------------------------------------------------------------------------*/
int AByteArray::compare (const char* other, unsigned int length) const {
   TRACE5 ("AByteArray::compare (const char*, unsigned int)");

   assert ((length && other) || !length);
   assert (!checkIntegrity ());

   // Both objects are defined -> Compare them
   if (isDefined ()) {
      TRACE6 ("   -> Comparing two defined values");
      TRACE9 ("   -> this = pValue = " << pValue);
      TRACE9 ("   -> other = " << other);

      int rc (memcmp (pValue, other, (len > length) ? length : len));
      if (!rc)
         rc = len - length;         // Chars are equal: Longer string is bigger
      return rc; 
   }

   return other != NULL;             // NULL-pointer: Return equal; else bigger
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares the byte array with a single character. Undefined values
//            are considered to be smaller then any character.
//Parameters: other: Character array to compare
//Returns   : int; <0: this < other; >0: other < this; 0 else
/*--------------------------------------------------------------------------*/
int AByteArray::compare (const char other) const {
   TRACE5 ("AByteArray::compare (const char)");

   assert (!checkIntegrity ());

   // Both objects are defined -> Compare them
   if (isDefined () && len > 0) {
      TRACE6 ("   -> Comparing two defined values");
      TRACE9 ("   -> this = pValue = " << pValue);
      TRACE9 ("   -> other = " << other);
      int ch (pValue[0] - other);

      if (ch)
         return ch;                // Characters differ: Return result of comp.

      return 1;
   }

   return -1;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the object into a string
//Returns   : std::string: Converted string
/*--------------------------------------------------------------------------*/
std::string AByteArray::toString () const {
   return std::string (pValue, len);
}
