#ifndef ATTRVAL_H
#define ATTRVAL_H

//$Id: AttrVal.h,v 1.5 1999/10/12 00:22:56 Markus Rel $

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
   virtual std::string toString () const { return ""; }

   void toString (std::string& value) { value = toString (); }

   friend ostream& operator<< (ostream& out, const AttributValue& outValue) {
      if (outValue.isDefined ())
         out << outValue.toString ();
      return out; }

 private:
   bool defined;
};

inline void AttributValue::define () {
   defined = true;
}

#endif
