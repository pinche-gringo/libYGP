#ifndef ATTRVAL_H
#define ATTRVAL_H

//$Id: AttrVal.h,v 1.2 1999/08/29 20:54:38 Markus Rel $

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

// Base-class for all attribut-values
class AttributValue {
 public:
   bool         isDefined () const { return defined; }
   virtual void undefine () { defined = false; }
  
 protected:
   AttributValue () : defined (false) { }
   AttributValue (const AttributValue& other) : defined (other.defined) { }
   virtual ~AttributValue () { }

   AttributValue& operator= (const AttributValue& other) {
      defined = other.defined; }

   virtual void define () = 0;

 private:
   bool defined;
};

inline void AttributValue::define () {
   defined = true;
}

#endif
