#ifndef ATTRVAL_H
#define ATTRVAL_H

//$Id: AttrVal.h,v 1.22 2003/11/14 20:27:55 markus Rel $

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


namespace YGP {

/**Base-class for all attribut-values

   Attributes can have a value (according to their type) or be undefined
   (having no value at all).

   An attribute is considered defined, if a value is explicitely set or if the
   defined ()-member is called (where the attribute gets a default-value;
   depending on its type) -> every derived object must call 
   AttributValue::define () when setting a value!
 */
class AttributValue {
 public:
   /// Defines the object (setting it to a default value
   bool         isDefined () const { return defined; }
   virtual void undefine () { defined = false; }      ///< Undefines the object

   /// Reads an (unformatted) value from the passed stream. To be implemented
   /// by derived objects (this class does nothing).
   virtual void readFromStream (std::istream&) throw (std::invalid_argument) { }

   /// Converts the attribute into an unformatted string.
   virtual std::string toUnformattedString () const { return ""; }
   /// Converts the attribute into a formatted string.
   virtual std::string toString () const { return toUnformattedString (); }
   /// Converts the attribute into a formatted string.
   void toString (std::string& result) const { result = toString (); }
   /// Converts the attribute into an unformatted string.
   void toUnformattedString (std::string& result) const {
      result = toUnformattedString (); }

   /// Reads the object from the passed stream using the virtual method
   /// readFromStream()
   friend std::istream& operator>> (std::istream& in, AttributValue& inValue) {
      inValue.readFromStream (in); return in; }
   /// Writes the object to the stream using the virtual method
   /// toUnformattedString().
   friend std::ostream& operator<< (std::ostream& out, const AttributValue& outValue) {
      if (outValue.isDefined ())
         out << outValue.toUnformattedString ();
      return out; }

 protected:
   /// Defaultconstructor; the attribute is not defined.
   AttributValue () : defined (false) { }
   /// Constructor; the attribute is defined according the parameter.
   AttributValue (bool define) : defined (define) { }
   /// Copy constructor; assigns the value of \c other.
   AttributValue (const AttributValue& other) : defined (other.defined) { }
   virtual ~AttributValue () { }                               ///< Destructor

   /// Assignment operator; assigns the value of \c other.
   AttributValue& operator= (const AttributValue& other) {
      defined = other.defined; return *this; }

   virtual void define () = 0;
   /// Defines the attribute; non-virtual method to be called inside
   /// define-methods of derived classes.
   void setDefined () { defined = true; }

 private:
   bool defined;
};

//-----------------------------------------------------------------------------
/// Defines the attribute; to be implemented by derived objects.
///
/// Derived objects must call define of their superclass!
//-----------------------------------------------------------------------------
inline void AttributValue::define () {
   setDefined ();
}

}

#endif
