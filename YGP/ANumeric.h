#ifndef ANUMERIC_H
#define ANUMERIC_H

//$Id: ANumeric.h,v 1.2 1999/09/05 01:33:54 Markus Rel $

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
   operator std::string () const;

   friend ostream& operator<< (ostream& out, const ANumeric& outValue);

 private: 
   long value;
};

#endif
