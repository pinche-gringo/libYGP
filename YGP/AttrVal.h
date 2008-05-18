#ifndef YGP_ATTRVAL_H
#define YGP_ATTRVAL_H

//$Id: AttrVal.h,v 1.26 2008/05/18 13:21:27 markus Rel $

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
   /// Checks if the object is defined
   bool         isDefined () const { return defined; }
   virtual void undefine () { defined = false; }      ///< Undefines the object

   /// Reads a value from the passed stream. To be implemented by derived
   /// objects (this class does nothing).
   virtual void readFromStream (std::istream&) throw (std::invalid_argument) = 0;

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

   /// Defines the objects (sets to the default value)
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
