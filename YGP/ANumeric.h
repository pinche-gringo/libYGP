#ifndef ANUMERIC_H
#define ANUMERIC_H

//$Id: ANumeric.h,v 1.1 1999/07/31 00:15:08 Markus Exp $

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "AttrVal.h"


// Class for numeric attributes
// TODO: Calculation-methods
class ANumeric : public AttributValue {
 public:
   ANumeric () : AttributValue (), value (0) { }
   ANumeric (const ANumeric& other)
     : AttributValue ((const AttributValue&)other) {
     value = other.value; }
   ANumeric (const long val) : AttributValue (), value (val) {
      AttributValue::define (); }
   virtual ~ANumeric ();

   ANumeric& operator= (const ANumeric& other);
   ANumeric& operator= (long val) { define (); value = val; }

   virtual void define () { AttributValue::define (); value = 0; }

   operator long () const { return value; }

   friend ostream& operator<< (ostream& out, const ANumeric& outValue);

 private:
   long value;
};

#endif
