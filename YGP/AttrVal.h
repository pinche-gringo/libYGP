#ifndef ATTRVAL_H
#define ATTRVAL_H

//$Id: AttrVal.h,v 1.16 2002/10/10 05:46:52 markus Rel $

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


#include <iostream>

#include <string>

#include <stdexcept>


// Base-class for all attribut-values
class AttributValue {
 public:
   bool         isDefined () const { return defined; }
   virtual void undefine () { defined = false; }

 protected:
   AttributValue () : defined (false) { }
   AttributValue (bool define) : defined (define) { }
   AttributValue (const AttributValue& other) : defined (other.defined) { }
   virtual ~AttributValue () { }

   AttributValue& operator= (const AttributValue& other) {
      defined = other.defined; return *this; }
   virtual void define () = 0;
   void setDefined () { defined = true; }

   virtual std::string toString () const { return toUnformatedString (); }
   void toString (std::string& result) const { result = toString (); }
   virtual std::string toUnformatedString () const { return ""; }
   void toUnformatedString (std::string& result) const {
      result = toUnformatedString (); }

   virtual void readFromStream (std::istream&) throw (std::invalid_argument) { }

   friend std::ostream& operator>> (std::istream& in, AttributValue& inValue) {
      inValue.readFromStream (in); }
   friend std::ostream& operator<< (std::ostream& out, const AttributValue& outValue) {
      if (outValue.isDefined ())
         out.operator<< (outValue.toUnformatedString ().c_str ());
      return out; }

 private:
   bool defined;
};

inline void AttributValue::define () {
   setDefined ();
}

#endif
