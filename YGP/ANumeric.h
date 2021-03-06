#ifndef YGP_ANUMERIC_H
#define YGP_ANUMERIC_H

//$Id: ANumeric.h,v 1.39 2008/05/18 13:21:27 markus Rel $

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include <string>

#include <ygp-cfg.h>

#ifdef HAVE_LIBGMP
#  include <gmp.h>
   typedef mpz_t numType;
#else
#  ifdef HAVE_UNSIGNED_LONG_LONG
      typedef long long int numType;
#  else
      typedef long int numType;
#  endif
#endif
#include <YGP/AttrVal.h>


// Forward declarations
namespace std {
   class invalid_argument;
}


namespace YGP {

/**Class for numeric attributes. As every AttributValue is supports undefined
   values.

   Furthermore the range is unlimitted (thanks to GNUs Multi Precission
   library. That also means, that if this library is not available, only longs
   (or long longs, if they are supported) are used!)
*/
class ANumeric : public AttributValue {
 public:
   /// Default constructor; creates an undefined number
   ANumeric () : AttributValue (), value () {
#ifdef HAVE_LIBGMP
      mpz_init_set_si (value, 0);
#else
      value = 0;
#endif
}
   /// Copy constructor
   ANumeric (const ANumeric& other) : AttributValue ((const AttributValue&)other), value () {
#ifdef HAVE_LIBGMP
     mpz_init_set (value, other.value);
#else
     value = other.value;
#endif
}
   /// Constructor; initializes the object with the passed value and defines it.
   ANumeric (const unsigned int val) : AttributValue (true), value () {
#ifdef HAVE_LIBGMP
      mpz_init_set_ui (value, (long)val);
#else
      value = (long)val;
#endif
   }
   /// Constructor; initializes the object with the passed value and defines it.
   ANumeric (const unsigned long val) : AttributValue (), value () {
#ifdef HAVE_LIBGMP
      mpz_init_set_ui (value, val);
#else
      value = val;
#endif
      setDefined (); }
   /// Constructor; initializes the object with the passed value and defines it.
   ANumeric (const int val) : AttributValue (), value () {
#ifdef HAVE_LIBGMP
      mpz_init_set_si (value, (long)val);
#else
      value = (long)val;
#endif
      setDefined (); }
   /// Constructor; initializes the object with the passed value and defines it.
   ANumeric (const long val) : AttributValue (), value () {
#ifdef HAVE_LIBGMP
      mpz_init_set_si (value, val);
#else
      value = val;
#endif
      setDefined (); }

   /// Constructor; initializes the object from the passed text and defines it.
   ANumeric (const char* pValue) throw (std::invalid_argument) : AttributValue (), value () {
      operator= (pValue);
   }
   /// Constructor; initializes the object from the passed text and defines it.
   ANumeric (const std::string& str) throw (std::invalid_argument)
      : AttributValue (), value () {
      operator= (str);
   }
   virtual ~ANumeric ();

   /// \name Assignment methods
   //@{
   /// Assigning from a value
   ANumeric& operator= (const int val) { return operator= ((const long)val); }
   ANumeric& operator= (const unsigned int val) {    /// Assigning from a value
      return operator= ((const unsigned long)val); }
   ANumeric& operator= (const unsigned long val) {   /// Assigning from a value
      setDefined ();
#ifdef HAVE_LIBGMP
      mpz_set_ui (value, val);
#else
      value = val;
#endif
      return *this;
   }
   ANumeric& operator= (const long val) {            /// Assigning from a value
      setDefined ();
#ifdef HAVE_LIBGMP
      mpz_set_si (value, val);
#else
      value = val;
#endif
      return *this;
   }
   /// Assign the value from the passed text
   ANumeric& operator= (const std::string& value) throw (std::invalid_argument) {
      return operator= (value.c_str ()); }
   ANumeric& operator= (const char* pValue) throw (std::invalid_argument);
   ANumeric& operator= (const ANumeric& other);
   //@}

   virtual void define ();
   virtual std::string toString () const;
   virtual std::string toUnformattedString () const;
   virtual void readFromStream (std::istream& in) throw (std::invalid_argument);

   /// \name Convertion
   //@{
   static std::string toString (int value) {    /// Convertion to a std::string
      ANumeric temp (value); return temp.toString (); }
   static std::string toString (unsigned int value) { /// Convertion to a std::string
      ANumeric temp (value); return temp.toString (); }
   static std::string toString (short value) {  /// Convertion to a std::string
      ANumeric temp (value); return temp.toString (); }
   static std::string toString (unsigned short value) {     /// Convertion to a std::string
      ANumeric temp (value); return temp.toString (); }
   static std::string toString (long value) {   /// Convertion to a std::string
      ANumeric temp (value); return temp.toString (); }
   static std::string toString (unsigned long value) {  /// Convertion to a std::string
      ANumeric temp (value); return temp.toString (); }
   /// Convertion to a number
   operator long int () const {
#ifdef HAVE_LIBGMP
      return mpz_get_si (value);
#else
      return value;
#endif
   }
   /// Convertion to a number
   operator unsigned long int () const {
#ifdef HAVE_LIBGMP
      return mpz_get_si (value);
#else
      return static_cast<unsigned long int> (value);
#endif
   }
   /// Convertion to a number
   operator int () const {
#ifdef HAVE_LIBGMP
      return static_cast<int> (mpz_get_si (value));
#else
      return static_cast<int> (value);
#endif
   }
   /// Convertion to a number
   operator unsigned int () const {
#ifdef HAVE_LIBGMP
      return static_cast<unsigned int> (mpz_get_si (value));
#else
      return static_cast<unsigned int> (value);
#endif
   }
   /// Convertion to a number
   operator short int () const {
#ifdef HAVE_LIBGMP
      return static_cast<short int> (mpz_get_si (value));
#else
      return static_cast<short int> (value);
#endif
   }
   /// Convertion to a number
   operator unsigned short int () const {
#ifdef HAVE_LIBGMP
      return static_cast<unsigned short int> (mpz_get_si (value));
#else
      return static_cast<unsigned short int> (value);
#endif
   }
   //@}

   /// \name Calculation
   //@{
   ANumeric& operator += (const ANumeric& rhs);
   ANumeric& operator -= (const ANumeric& rhs);
   ANumeric& operator *= (const ANumeric& rhs);
   ANumeric& operator /= (const ANumeric& rhs);

   friend ANumeric operator+ (const ANumeric& lhs, const ANumeric& rhs);
   friend ANumeric operator- (const ANumeric& lhs, const ANumeric& rhs);
   friend ANumeric operator* (const ANumeric& lhs, const ANumeric& rhs);
   friend ANumeric operator/ (const ANumeric& lhs, const ANumeric& rhs);
   //@}

   /// \name Comparison
   //@{
   /// Checks if two objects are equal. See compare() for details.
   bool operator== (const ANumeric& other) const { return !compare (other); }
   /// Checks if two objects are not equal. See compare() for details.
   bool operator!= (const ANumeric& other) const { return compare (other) != 0; }
   /// Checks if one object is smaller than another. See compare() for details.
   bool operator<  (const ANumeric& other) const { return compare (other) < 0; }
   /// Checks if one object is bigger than another. See compare() for details.
   bool operator>  (const ANumeric& other) const { return compare (other) > 0; }
   /// Checks if one object is smaller than or equan to another. See compare() for details.
   bool operator<= (const ANumeric& other) const { return compare (other) <= 0; }
   /// Checks if one object is bigger than or equan to another. See compare() for details.
   bool operator>= (const ANumeric& other) const { return compare (other) >= 0; }
   int compare (const ANumeric& other) const;

   /// Checks if two objects are equal. See compare() for details.
   bool operator== (int other) const { return !compare (other); }
   /// Checks if two objects are not equal. See compare() for details.
   bool operator!= (int other) const { return compare (other) != 0; }
   /// Checks if one object is smaller than another. See compare() for details.
   bool operator<  (int other) const { return compare (other) < 0; }
   /// Checks if one object is bigger than another. See compare() for details.
   bool operator>  (int other) const { return compare (other) > 0; }
   /// Checks if one object is smaller than or equan to another. See compare() for details.
   bool operator<= (int other) const { return compare (other) <= 0; }
   /// Checks if one object is bigger than or equan to another. See compare() for details.
   bool operator>= (int other) const { return compare (other) >= 0; }
   //@}

 private:
   numType value;
};

}

#endif
