#ifndef ABYTEARRAY_H
#define ABYTEARRAY_H

// $Id: AByteArray.h,v 1.1 2001/03/25 09:49:57 markus Exp $

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

class AByteArray : public AttributValue {
 public:
   AByteArray () : AttributValue (), pValue (NULL), len (0), allocated (0) { }
   AByteArray (unsigned int length): AttributValue (true)
      , pValue (new char[length]), len (0), allocated (length) { }
   AByteArray (const char* pSource);
   AByteArray (const char* pSource, unsigned int length);
   AByteArray (const std::string& str);
   AByteArray (const AByteArray& other);
   virtual ~AByteArray ();

   virtual void define ();
   virtual void undefine ();

   // Set-functions
   AByteArray& operator= (const AByteArray& rhs) { assign (rhs); }
   AByteArray& operator= (const std::string& str) { assign (str); }
   AByteArray& operator= (const char* pValue) { assign (pValue); }

   AByteArray& assign (const AByteArray& rhs);
   AByteArray& assign (const std::string& str) { assign (str.data (), str.length ()); }
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

   char operator[] (unsigned int pos) const throw (out_of_range);
   char& operator[] (unsigned int pos) throw (out_of_range);
   const char& at (unsigned int pos) const throw (out_of_range);
   char& at (unsigned int pos) throw (out_of_range);

   int length () const { return len; }
   const char* data () const { return pValue; }
   bool empty () const { return !len; }

   int compare (const AByteArray& other) const;

   friend bool operator< (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) < 0; }
   friend bool operator< (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) < 0; }
   friend bool operator< (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) < 0; }
   friend bool operator< (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) < 0; }
   friend bool operator< (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) < 0; }
   friend bool operator< (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) < 0; }
   friend bool operator< (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) < 0; }

   friend bool operator> (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) > 0; }
   friend bool operator> (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) > 0; }
   friend bool operator> (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) > 0; }
   friend bool operator> (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) > 0; }
   friend bool operator> (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) > 0; }
   friend bool operator> (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) > 0; }
   friend bool operator> (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) > 0; }

   friend bool operator<= (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) <= 0; }
   friend bool operator<= (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) <= 0; }
   friend bool operator<= (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) <= 0; }
   friend bool operator<= (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) <= 0; }
   friend bool operator<= (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) <= 0; }
   friend bool operator<= (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) <= 0; }
   friend bool operator<= (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) <= 0; }

   friend bool operator>= (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) >= 0; }
   friend bool operator>= (const AByteArray& lhs, const std::string& rhs) {
      return lhs.compare (rhs) >= 0; }
   friend bool operator>= (const AByteArray& lhs, const char* rhs) {
      return lhs.compare (rhs) >= 0; }
   friend bool operator>= (const AByteArray& lhs, char rhs) {
      return lhs.compare (rhs) >= 0; }
   friend bool operator>= (const std::string& lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) >= 0; }
   friend bool operator>= (const char* lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) >= 0; }
   friend bool operator>= (char lhs, const AByteArray& rhs) {
      return rhs.compare (lhs) >= 0; }

   friend bool operator== (const AByteArray& lhs, const AByteArray& rhs) {
      return lhs.compare (rhs) > 0; }
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

 protected:
   int checkIntegrity () const;

 private:	
   char* pValue;
   unsigned int len;
   unsigned int allocated;

   static const unsigned int MEMADD;
};

#endif