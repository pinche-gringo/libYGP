#ifndef ANUMERIC_H
#define ANUMERIC_H

//$Id: ANumeric.h,v 1.26 2003/02/22 18:22:02 markus Exp $

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

#include <gzo-cfg.h>

#ifdef HAVE_LIBGMP
#  include <gmp.h>
   typedef mpz_t numType;
#else
#  ifdef __GNUC__
      typedef long long int numType;
#  else
      typedef long int numType;
#  endif
#endif
#include <AttrVal.h>


// Forward declarations
namespace std {
   class invalid_argument;
}


// Class for numeric attributes. As every AttributValue is supports undefined
// values. Furthermore the range is unlimitted (thanks to GNUs Multi Precission
// library. That also means, that if this library is not available, only longs
// are used!)
class ANumeric : public AttributValue {
 public:
   ANumeric () : AttributValue () {
#ifdef HAVE_LIBGMP
      mpz_init_set_si (value, 0);
#else
      value = 0;
#endif
}
   ANumeric (const ANumeric& other) : AttributValue ((const AttributValue&)other) {
#ifdef HAVE_LIBGMP
     mpz_init_set (value, other.value);
#else
     value = other.value;
#endif
}
   ANumeric (const unsigned int val) : AttributValue () {
#ifdef HAVE_LIBGMP
      mpz_init_set_ui (value, (long)val);
#else
      value = (long)val;
#endif
      setDefined (); }
   ANumeric (const unsigned long val) : AttributValue () {
#ifdef HAVE_LIBGMP
      mpz_init_set_ui (value, val);
#else
      value = val;
#endif
      setDefined (); }
   ANumeric (const int val) : AttributValue () {
#ifdef HAVE_LIBGMP
      mpz_init_set_si (value, (long)val);
#else
      value = (long)val;
#endif
      setDefined (); }
   ANumeric (const long val) : AttributValue () {
#ifdef HAVE_LIBGMP
      mpz_init_set_si (value, val);
#else
      value = val;
#endif
      setDefined (); }
   ANumeric (const char* pValue) throw (std::invalid_argument) : AttributValue () {
      operator= (pValue); }
   ANumeric (const std::string& value) throw (std::invalid_argument)
      : AttributValue () { operator= (value.c_str ()); }
   virtual ~ANumeric ();

   ANumeric& operator= (const ANumeric& other);
   ANumeric& operator= (const int val) { return operator= ((const long)val); }
   ANumeric& operator= (const unsigned int val) { return operator= ((const unsigned long)val); }
   ANumeric& operator= (const unsigned long val) { setDefined ();
#ifdef HAVE_LIBGMP
      mpz_set_ui (value, val);
#else
      value = val;
#endif
      return *this;
   }
   ANumeric& operator= (const long val) { setDefined ();
#ifdef HAVE_LIBGMP
      mpz_set_si (value, val);
#else
      value = val;
#endif
      return *this;
   }
   ANumeric& operator= (const char* pValue) throw (std::invalid_argument);
   ANumeric& operator= (const std::string& value) throw (std::invalid_argument) {
      operator= (value.c_str ()); }

   virtual void define ();
   virtual std::string toString () const;
   virtual std::string toUnformatedString () const;
   virtual void readFromStream (std::istream& in) throw (std::invalid_argument);
   static std::string toString (int value) { ANumeric temp (value); return temp.toString (); }
   static std::string toString (unsigned int value) { ANumeric temp (value); return temp.toString (); }
   static std::string toString (short value) { ANumeric temp (value); return temp.toString (); }
   static std::string toString (unsigned short value) { ANumeric temp (value); return temp.toString (); }
   static std::string toString (long value) { ANumeric temp (value); return temp.toString (); }
   static std::string toString (unsigned long value) { ANumeric temp (value); return temp.toString (); }
   operator long int () const {
#ifdef HAVE_LIBGMP
      return mpz_get_si (value);
#else
      return value;
#endif
   }

   // Calculation
   ANumeric& operator += (const ANumeric& rhs);
   ANumeric& operator -= (const ANumeric& rhs);
   ANumeric& operator *= (const ANumeric& rhs);
   ANumeric& operator /= (const ANumeric& rhs);

   friend ANumeric operator+ (const ANumeric& lhs, const ANumeric& rhs);
   friend ANumeric operator- (const ANumeric& lhs, const ANumeric& rhs);
   friend ANumeric operator* (const ANumeric& lhs, const ANumeric& rhs);
   friend ANumeric operator/ (const ANumeric& lhs, const ANumeric& rhs);

   // Comparison
   bool operator== (const ANumeric& other) { return !compare (other); }
   bool operator!= (const ANumeric& other) { return compare (other) != 0; }
   bool operator<  (const ANumeric& other) { return compare (other) < 0; }
   bool operator>  (const ANumeric& other) { return compare (other) > 0; }
   bool operator<= (const ANumeric& other) { return compare (other) <= 0; }
   bool operator>= (const ANumeric& other) { return compare (other) >= 0; }
   int compare (const ANumeric& other);

 private:
   numType value;
};

#endif
