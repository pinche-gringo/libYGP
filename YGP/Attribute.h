#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

//$Id: Attribute.h,v 1.4 2001/10/08 23:34:12 markus Exp $

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


#include <errno.h>
#include <assert.h>

#include <string>
#include <stdexcept>


// Baseclass for attributes. Derive from it for every type of attribute.
class IAttribute {
 public:
   virtual ~IAttribute () { }

   bool matches (const char* pName) const {
      assert (pName);
      return name == pName; }
   bool matches (const std::string& compare) const {
      return name == compare; }

   virtual bool assignFromString (const char* value) const = 0;

   const std::string& getName () const { return name; }

 protected:
   IAttribute (const char* pName) : name (pName) { assert (pName); }
   IAttribute (const std::string& name_) : name (name_) { }

 private:
   IAttribute (const IAttribute&);
   const IAttribute& operator= (const IAttribute&);

   const std::string name;
};


// Class representing an attribute. This class is a template to handle differing
// types of attributes.
template <class T> class Attribute : public IAttribute {
 public:
   Attribute (const char* name, T& attr) : IAttribute (name), attr_ (attr) { }
   Attribute (const std::string& name, T& attr) : IAttribute (name), attr_ (attr) { }
   ~Attribute () {  }

   virtual bool assignFromString (const char* value) const {
      try {
         attr_ = value;
         return true;
      }
      catch (invalid_argument& e) {
         return false;
      }
   }

 private:
   Attribute (const Attribute&);
   const Attribute& operator= (const Attribute&);

   T& attr_;
};

// Specialization of Attribute for ints
bool Attribute<char>::assignFromString (const char* value) const {
   assert (value);
   attr_ = *value;
   return *value && !value[1];
}

bool Attribute<char*>::assignFromString (const char* value) const {
   assert (value);
   delete [] attr_;
   attr_ = new char[strlen (value) + 1];
   if (!attr_)
      return false;
   strcpy (attr_, value);
   return true;
}

bool Attribute<char* const>::assignFromString (const char* value) const {
   assert (value);
   strcpy (attr_, value);
   return true;
}

bool Attribute<short>::assignFromString (const char* value) const {
   assert (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); assert (pTail);
   return !(errno || *pTail);
}

bool Attribute<unsigned short>::assignFromString (const char* value) const {
   assert (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtoul (value, &pTail, 10); assert (pTail);
   return !(errno || *pTail);
}

bool Attribute<int>::assignFromString (const char* value) const {
   assert (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); assert (pTail);
   return !(errno || *pTail);
}

bool Attribute<unsigned int>::assignFromString (const char* value) const {
   assert (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtoul (value, &pTail, 10); assert (pTail);
   return !(errno || *pTail);
}

bool Attribute<long>::assignFromString (const char* value) const {
   assert (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); assert (pTail);
   return !(errno || *pTail);
}

bool Attribute<unsigned long>::assignFromString (const char* value) const {
   assert (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtoul (value, &pTail, 10); assert (pTail);
   return !(errno || *pTail);
}

bool Attribute<double>::assignFromString (const char* value) const {
   assert (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtod (value, &pTail); assert (pTail);
   return !(errno || *pTail);
}

// Specialization of Attribute for strings
bool Attribute<std::string>::assignFromString (const char* value) const {
   assert (value);
   attr_ = value;
   return true;
}

#endif
