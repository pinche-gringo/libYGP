#ifndef ABYTEARRAY_H
#define ABYTEARRAY_H

// $Id: AByteArray.h,v 1.6 2003/06/13 17:49:45 markus Rel $

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
#include <stdexcept>

#include <AttrVal.h>

// Class for raw character-values. As every AttributValue is supports
// undefined values.
//
// In counterpart to the std::string-class this class distinguishes between
// an empty value and not defined (as every AttributValue).
class AByteArray : public AttributValue {
   // See the hedaer of the check integrity method for its invariants
 public:
   AByteArray () : AttributValue (), pValue (NULL), len (0), allocated (0) { }
   AByteArray (int length): AttributValue (true)
      , pValue (new char[length]), len (0), allocated (length) { }
   AByteArray (unsigned int length): AttributValue (true)
      , pValue (new char[length]), len (0), allocated (length) { }
   AByteArray (char ch);
   AByteArray (const char* pSource);
   AByteArray (const char* pSource, unsigned int length);
   AByteArray (const std::string& str);
   AByteArray (const AByteArray& other);
   virtual ~AByteArray ();

   virtual void define ();
   virtual void undefine ();

   // Set-functions
   AByteArray& operator= (const AByteArray& rhs) { return assign (rhs); }
   AByteArray& operator= (const std::string& str) { return assign (str); }
   AByteArray& operator= (const char* pValue) { return assign (pValue); }
   AByteArray& operator= (char ch) { return assign (ch); }

   AByteArray& assign (const AByteArray& rhs);
   AByteArray& assign (const std::string& str) { return assign (str.data (), str.length ()); }
   AByteArray& assign (char ch) { return assign (&ch, 1); }
   AByteArray& assign (const char* pValue);
   AByteArray& assign (const char* pValue, unsigned int len);

   friend AByteArray operator+ (const AByteArray& lhs, const AByteArray& rhs);
   friend AByteArray operator+ (const char* lhs, const AByteArray& rhs);
   friend AByteArray operator+ (char lhs, const AByteArray& rhs);
   friend AByteArray operator+ (const std::string& lhs, const AByteArray& rhs);
   friend AByteArray operator+ (const AByteArray& lhs, const std::string& rhs);
   friend AByteArray operator+ (const AByteArray& lhs, const char* rhs);
   friend AByteArray operator+ (const AByteArray& lhs, char rhs);

   AByteArray& operator+= (const AByteArray& rhs) { return append (rhs); }
   AByteArray& operator+= (const char* rhs) { return append (rhs); }
   AByteArray& operator+= (const std::string& rhs) { return append (rhs); }
   AByteArray& operator+= (const char rhs) { return append (rhs); }

   AByteArray& append (const char* pSource, unsigned int length);
   AByteArray& append (const AByteArray& o) { return append (o.pValue, o.len); }
   AByteArray& append (const char* pSource);
   AByteArray& append (const char ch) { return append (&ch, 1); }
   AByteArray& append (const std::string& str) {
      return append (str.c_str (), str.length ()); }

   char operator[] (unsigned int pos) const throw (std::out_of_range);
   char& operator[] (unsigned int pos) throw (std::out_of_range);
   const char& at (unsigned int pos) const throw (std::out_of_range);
   char& at (unsigned int pos) throw (std::out_of_range);

   int length () const { return len; }
   const char* data () const { return pValue; }
   bool empty () const { return !len; }

   int compare (const AByteArray& other) const;
   int compare (const char* other) const;
   int compare (const char* other, unsigned int length) const;
   int compare (const char other) const;
   int compare (const std::string& other) const { return compare (other.data (), other.length ()); }

   friend bool operator< (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) < 0; }
   friend bool operator< (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) < 0; }
   friend bool operator< (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) < 0; }
   friend bool operator< (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) < 0; }
   friend bool operator< (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) >= 0; }
   friend bool operator< (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) >= 0; }
   friend bool operator< (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) >= 0; }

   friend bool operator> (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) > 0; }
   friend bool operator> (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) > 0; }
   friend bool operator> (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) > 0; }
   friend bool operator> (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) > 0; }
   friend bool operator> (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) <= 0; }
   friend bool operator> (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) <= 0; }
   friend bool operator> (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) <= 0; }

   friend bool operator<= (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) <= 0; }
   friend bool operator<= (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) <= 0; }
   friend bool operator<= (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) <= 0; }
   friend bool operator<= (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) <= 0; }
   friend bool operator<= (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) > 0; }
   friend bool operator<= (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) > 0; }
   friend bool operator<= (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) > 0; }

   friend bool operator>= (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) >= 0; }
   friend bool operator>= (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) >= 0; }
   friend bool operator>= (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) >= 0; }
   friend bool operator>= (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) >= 0; }
   friend bool operator>= (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) < 0; }
   friend bool operator>= (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) < 0; }
   friend bool operator>= (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) < 0; }

   friend bool operator== (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) == 0; }
   friend bool operator== (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) == 0; }
   friend bool operator== (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) == 0; }
   friend bool operator== (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) == 0; }
   friend bool operator== (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) == 0; }
   friend bool operator== (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) == 0; }
   friend bool operator== (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) == 0; }

   friend bool operator!= (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) != 0; }
   friend bool operator!= (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) != 0; }
   friend bool operator!= (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) != 0; }
   friend bool operator!= (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) != 0; }
   friend bool operator!= (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) != 0; }
   friend bool operator!= (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) != 0; }
   friend bool operator!= (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) != 0; }

   virtual std::string toString () const;

 protected:
   int checkIntegrity () const;

 private:
   char* pValue;
   unsigned int len;
   unsigned int allocated;

   static const unsigned int MEMADD;
};

#endif
