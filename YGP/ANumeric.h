#ifndef ANUMERIC_H
#define ANUMERIC_H

//$Id: ANumeric.h,v 1.7 1999/10/19 22:48:12 Markus Rel $

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

#include "gmp.h"
#include "AttrVal.h"


// Class for numeric attributes. As every AttributValue is supports undefined
// values. Furthermore the range is unlimitted (thanks to GNUs Multi Precission
// library)
class ANumeric : public AttributValue {
 public:
   ANumeric () : AttributValue () { mpz_init_set_si (value, 0); }
   ANumeric (const ANumeric& other) : AttributValue ((const AttributValue&)other) {
     mpz_init_set (value, other.value); }
   ANumeric (const long val) : AttributValue () {
      mpz_init_set_si (value, val);
      AttributValue::define (); }
   virtual ~ANumeric ();

   ANumeric& operator= (const ANumeric& other);
   ANumeric& operator= (long val) { AttributValue::define ();  mpz_set_si (value, val); }

   virtual void define ();
   virtual std::string toString () const;
   virtual void readFromStream (istream& in);

   // Calculation
   ANumeric& operator += (const ANumeric& lhs);
   ANumeric& operator -= (const ANumeric& lhs);
   ANumeric& operator *= (const ANumeric& lhs);
   ANumeric& operator /= (const ANumeric& lhs);

   friend ANumeric operator+ (const ANumeric& lhs, const ANumeric& rhs);
   friend ANumeric operator- (const ANumeric& lhs, const ANumeric& rhs);
   friend ANumeric operator* (const ANumeric& lhs, const ANumeric& rhs);
   friend ANumeric operator/ (const ANumeric& lhs, const ANumeric& rhs);

   // Comparison
   bool operator== (const ANumeric& other) { return !compare (other); }
   bool operator!= (const ANumeric& other) { return compare (other); }
   bool operator<  (const ANumeric& other) { return compare (other) < 0; }
   bool operator>  (const ANumeric& other) { return compare (other) > 0; }
   bool operator<= (const ANumeric& other) { return compare (other) <= 0; }
   bool operator>= (const ANumeric& other) { return compare (other) >= 0; }
   int compare (const ANumeric& other);

 private: 
   mpz_t value;
};

#endif
