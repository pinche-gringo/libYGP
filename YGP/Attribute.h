#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

//$Id: Attribute.h,v 1.12 2003/01/16 16:46:22 markus Exp $

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


#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif


#include <stdio.h>
#include <errno.h>

#include <string>
#include <vector>
#include <stdexcept>

#include <Check.h>
#include <AssParse.h>
#include <AByteArray.h>


// Baseclass for attributes. Derive from it for every type of attribute.
//
// An attribute is defined to be the connection between a name and an
// attribute-value (which is not stored inside the attribute but only
// refereneced).
class IAttribute {
 public:
   virtual ~IAttribute () { }

   bool matches (const char* pName) const {
      Check3 (pName);
      return name == pName; }
   bool matches (const std::string& name_) const {
      return name == name_; }

   virtual bool assignFromString (const char* value) const = 0;
   virtual bool assign (const char* value, unsigned int length) const {
      return assignFromString (value); }

   const std::string& getName () const { return name; }
   virtual std::string getValue () const = 0;
   virtual std::string getFormatedValue () const { return getValue (); }

 protected:
   IAttribute (const char* pName) : name (pName) { Check3 (pName); }
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
      }
      catch (std::invalid_argument&) {
         return false;
      }
      return true;
   }

   virtual bool assign (const char* value, unsigned int length) const {
      return assignFromString (value); }

   T& getAttribute () const { return attr_; }
   virtual std::string getValue () const { return attr_.toUnformatedString (); }
   virtual std::string getFormatedValue () const { return attr_.toString (); }

 private:

   Attribute (const Attribute&);
   const Attribute& operator= (const Attribute&);

   T& attr_;
};


// Specialization of Attribute for character (arrays)
template <> bool Attribute<char>::assignFromString (const char* value) const {
   Check3 (value);
   attr_ = *value;
   return *value && !value[1];
}
template <> std::string Attribute<char>::getValue () const { return std::string (1, attr_); }
template <> std::string Attribute<char>::getFormatedValue () const { return getValue (); }

template <> bool Attribute<char*>::assign (const char* value, unsigned int length) const {
   Check3 (value);
   delete [] attr_;
   attr_ = new char[length + 1];
   if (!attr_)
      return false;
   memcpy (attr_, value, length);
   attr_[length] = '\0';
   return true;
}
template <> bool Attribute<char*>::assignFromString (const char* value) const {
   Check3 (value);
   return assign (value, strlen (value));
}
template <> std::string Attribute<char*>::getValue () const { return attr_; }
template <> std::string Attribute<char*>::getFormatedValue () const { return getValue (); }

template <> bool Attribute<char* const>::assignFromString (const char* value) const {
   Check3 (value);
   strcpy (attr_, value);
   return true;
}
template <> bool Attribute<char* const>::assign (const char* value, unsigned int length) const {
   Check3 (value);
   memcpy (attr_, value, length);
   return true;
}
template <> std::string Attribute<char* const>::getValue () const { return attr_; }
template <> std::string Attribute<char* const>::getFormatedValue () const { return getValue (); }

// Specialization of Attribute for ints
template <> bool Attribute<short>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> std::string Attribute<short>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   return std::string (buffer); }
template <> std::string Attribute<short>::getFormatedValue () const { return getValue (); }

template <> bool Attribute<unsigned short>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> std::string Attribute<unsigned short>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   return std::string (buffer); }
template <> std::string Attribute<unsigned short>::getFormatedValue () const { return getValue (); }

template <> bool Attribute<int>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> std::string Attribute<int>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   return std::string (buffer); }
template <> std::string Attribute<int>::getFormatedValue () const { return getValue (); }

template <> bool Attribute<unsigned int>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> std::string Attribute<unsigned int>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   return std::string (buffer); }
template <> std::string Attribute<unsigned int>::getFormatedValue () const { return getValue (); }

template <> bool Attribute<long>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> std::string Attribute<long>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%ld", attr_);
   return std::string (buffer); }
template <> std::string Attribute<long>::getFormatedValue () const { return getValue (); }

template <> bool Attribute<unsigned long>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> std::string Attribute<unsigned long>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%ld", attr_);
   return std::string (buffer); }
template <> std::string Attribute<unsigned long>::getFormatedValue () const { return getValue (); }

template <> bool Attribute<double>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtod (value, &pTail); Check3 (pTail);
   return !(errno || *pTail);
}
template <> std::string Attribute<double>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%lg", attr_);
   return std::string (buffer); }
template <> std::string Attribute<double>::getFormatedValue () const { return getValue (); }

// Specialization of Attribute for strings
template <> bool Attribute<std::string>::assignFromString (const char* value) const {
   Check3 (value);
   attr_ = value;
   return true;
}
template <> bool Attribute<std::string>::assign (const char* value, unsigned int length) const {
   Check3 (value);
   attr_.assign (value, length);
   return true;
}
template <> std::string Attribute<std::string>::getValue () const { return attr_; }
template <> std::string Attribute<std::string>::getFormatedValue () const { return getValue (); }

template <> bool Attribute<AByteArray>::assign (const char* value, unsigned int length) const {
   Check3 (value);
   attr_.assign (value, length);
   return true;
}


// Class representing a list of attributes. This class is a template to handle differing
// types of attributes.
template <class T> class AttributeList : public IAttribute {
 public:
   AttributeList (const char* name, std::vector<T>& list) : IAttribute (name), list_ (list) { }
   AttributeList (const std::string& name, std::vector<T>& list) : IAttribute (name), list_ (list) { }
   ~AttributeList () {  }

   virtual bool assignFromString (const char* value) const {
      AssignmentParse parse (value);
      std::string node;
      while (node = parse.getNextNode (), !node.empty ()) {
         try {
            errno = 0;
            char* pEnd = NULL;
            unsigned int offset (strtol (parse.getActKey ().c_str (), &pEnd, 10)); Check3 (pEnd);
            if (errno || *pEnd)
               return false;

            if (!assignFromString (offset, parse.getActValue ().c_str ()))
                return false;
         }
         catch (std::invalid_argument&) {
            return false;
         }
      }
      return true;
   }

   virtual bool assign (const char* value, unsigned int length) const {
      return assignFromString (value); }

   virtual bool assignFromString (unsigned int offset, const char* value) const {
      try {
         list_[offset] = value;
      }
      catch (std::invalid_argument&) {
         return false;
      }
      return true;
   }

   virtual bool assign (unsigned int offset, const char* value, unsigned int length) const {
      return assignFromString (offset, value); }

   std::string getValue ()  const { return ""; }

 private:
   AttributeList (const AttributeList&);
   const AttributeList& operator= (const AttributeList&);

   std::vector<T>& list_;
};


// Specialization of Attribute for character (arrays)
template <> bool AttributeList<char>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   list_[offset] = *value;
   return *value && !value[1];
}

template <> bool AttributeList<char*>::assign (unsigned int offset, const char* value, unsigned int length) const {
   Check3 (value);
   delete [] list_[offset];
   list_[offset] = new char[length + 1];
   if (!list_[offset])
      return false;
   memcpy (list_[offset], value, length);
   list_[offset][length] = '\0';
   return true;
}

template <> bool AttributeList<char*>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   return assign (offset, value, strlen (value));
}

#ifndef _MSC_VER
template <> bool AttributeList<char* const>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   strcpy (list_[offset], value);
   return true;
}
template <> bool AttributeList<char* const>::assign (unsigned int offset, const char* value, unsigned int length) const {
   Check3 (value);
   memcpy (list_[offset], value, length);
   return true;
}
#endif

// Specialization of AttributeList for ints
template <> bool AttributeList<short>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> bool AttributeList<unsigned short>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> bool AttributeList<int>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> bool AttributeList<unsigned int>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> bool AttributeList<long>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> bool AttributeList<unsigned long>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> bool AttributeList<double>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtod (value, &pTail); Check3 (pTail);
   return !(errno || *pTail);
}

// Specialization of AttributeList for strings
template <> bool AttributeList<std::string>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   list_[offset] = value;
   return true;
}

template <> bool AttributeList<std::string>::assign (unsigned int offset, const char* value, unsigned int length) const {
   Check3 (value);
   list_[offset].assign (value, length);
   return true;
}

template <> bool AttributeList<AByteArray>::assign (unsigned int offset, const char* value, unsigned int length) const {
   Check3 (value);
   list_[offset].assign (value, length);
   return true;
}

#endif
