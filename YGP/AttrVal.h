#ifndef ATTRVAL_H
#define ATTRVAL_H

//$Id: AttrVal.h,v 1.10 2000/03/21 23:29:08 Markus Rel $

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


#include <iostream.h>

#include <string>


// Forward declarations

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
      defined = other.defined; return *this; }
   virtual void define () = 0;

   virtual std::string toString () const { return ""; }
   void toString (std::string& result) const { result = toString (); }

   virtual void readFromStream (istream&) { }

   friend ostream& operator>> (istream& in, AttributValue& inValue) {
      inValue.readFromStream (in); }
   friend ostream& operator<< (ostream& out, const AttributValue& outValue) {
      if (outValue.isDefined ())
         out.operator<< (outValue.toString ().c_str ());
      return out; }

 private:
   bool defined;
};

inline void AttributValue::define () {
   defined = true;
}

#endif
