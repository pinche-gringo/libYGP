#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

//$Id: Attribute.h,v 1.22 2003/07/09 18:12:50 markus Rel $

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


#include <cstdio>
#include <cerrno>

#include <string>
#include <vector>
#include <stdexcept>

#include <Check.h>
#include <AssParse.h>

#include <gzo-cfg.h>

/**Baseclass for attributes. Derive from it for every type of attribute.

   An attribute is defined to be the connection between a name and an
   attribute-value (which is not stored inside the attribute but only
   refereneced).
*/
class IAttribute {
 public:
   /// Destructor
   virtual ~IAttribute () { }

   /// Checks if the passed text matches the name of the attribute
   bool matches (const char* pName) const {
      Check3 (pName);
      return name == pName; }
   /// Checks if the passed text matches the name of the attribute
   bool matches (const std::string& name_) const {
      return name == name_; }

   /// Assigns the passed text to the attribute value
   /// \returns \c true on success; \c false otherwise
   virtual bool assignFromString (const char* value) const = 0;
   /// Assigns the passed text to the attribute value
   virtual bool assign (const char* value, unsigned int length) const {
      return assignFromString (value); }

   /// Returns the name of the attribute
   const std::string& getName () const { return name; }
   /// Returns the value of the attribute (value)
   virtual std::string getValue () const = 0;
   /// Returns the value of the attribute (value) as formatted string
   virtual std::string getFormattedValue () const { return getValue (); }

 protected:
   /// Constructor; creates an attribute with the specified name
   IAttribute (const char* pName) : name (pName) { Check3 (pName); }
   /// Constructor; creates an attribute with the specified name
   IAttribute (const std::string& name_) : name (name_) { }

 private:
   IAttribute (const IAttribute&);
   const IAttribute& operator= (const IAttribute&);

   const std::string name;
};


/**Template representing an attribute of a specific type.

   This class is designed to be used by AttributeValues (and derived types),
   but there exists specializations for:
     - char
     - char*
     - char* const
     - short
     - unsigned short
     - int
     - unsigned int
     - long
     - unsigned long
     - double
     - std::string
*/
template <class T> class Attribute : public IAttribute {
 public:
   /// Constructor; creates an attribute with the specified name, referencing the attribute value
   Attribute (const char* name, T& attr) : IAttribute (name), attr_ (attr) { }
   /// Constructor; creates an attribute with the specified name, referencing the attribute value
   Attribute (const std::string& name, T& attr) : IAttribute (name), attr_ (attr) { }
   /// Destructor
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

   virtual bool assign (const char* value, unsigned int) const {
      return assignFromString (value); }

   /// Returns a reference to the handled attribute value
   T& getAttribute () const { return attr_; }
   virtual std::string getValue () const { return attr_.toUnformattedString (); }
   virtual std::string getFormattedValue () const { return attr_.toString (); }

 private:
   Attribute (const Attribute&);
   const Attribute& operator= (const Attribute&);

   T& attr_;
};


// Specialization of Attribute for character (arrays)
template <> inline bool Attribute<char>::assignFromString (const char* value) const {
   Check3 (value);
   attr_ = *value;
   return *value && !value[1];
}
template <> inline std::string Attribute<char>::getValue () const { return std::string (1, attr_); }
template <> inline std::string Attribute<char>::getFormattedValue () const { return getValue (); }

template <> inline bool Attribute<char*>::assign (const char* value, unsigned int length) const {
   Check3 (value);
   delete [] attr_;
   attr_ = new char[length + 1];
   if (!attr_)
      return false;
   memcpy (attr_, value, length);
   attr_[length] = '\0';
   return true;
}
template <> inline bool Attribute<char*>::assignFromString (const char* value) const {
   Check3 (value);
   return assign (value, strlen (value));
}
template <> inline std::string Attribute<char*>::getValue () const { return attr_; }
template <> inline std::string Attribute<char*>::getFormattedValue () const { return getValue (); }

template <> inline bool Attribute<char* const>::assignFromString (const char* value) const {
   Check3 (value);
   strcpy (attr_, value);
   return true;
}
template <> inline bool Attribute<char* const>::assign (const char* value, unsigned int length) const {
   Check3 (value);
   memcpy (attr_, value, length);
   return true;
}
template <> inline std::string Attribute<char* const>::getValue () const { return attr_; }
template <> inline std::string Attribute<char* const>::getFormattedValue () const { return getValue (); }

// Specialization of Attribute for ints
template <> inline bool Attribute<short>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> inline std::string Attribute<short>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   return std::string (buffer); }
template <> inline std::string Attribute<short>::getFormattedValue () const { return getValue (); }

template <> inline bool Attribute<unsigned short>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> inline std::string Attribute<unsigned short>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   return std::string (buffer); }
template <> inline std::string Attribute<unsigned short>::getFormattedValue () const { return getValue (); }

template <> inline bool Attribute<int>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> inline std::string Attribute<int>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   return std::string (buffer); }
template <> inline std::string Attribute<int>::getFormattedValue () const { return getValue (); }

template <> inline bool Attribute<unsigned int>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> inline std::string Attribute<unsigned int>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   return std::string (buffer); }
template <> inline std::string Attribute<unsigned int>::getFormattedValue () const { return getValue (); }

template <> inline bool Attribute<long>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> inline std::string Attribute<long>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%ld", attr_);
   return std::string (buffer); }
template <> inline std::string Attribute<long>::getFormattedValue () const { return getValue (); }

template <> inline bool Attribute<unsigned long>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}
template <> inline std::string Attribute<unsigned long>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%ld", attr_);
   return std::string (buffer); }
template <> inline std::string Attribute<unsigned long>::getFormattedValue () const { return getValue (); }

template <> inline bool Attribute<double>::assignFromString (const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtod (value, &pTail); Check3 (pTail);
   return !(errno || *pTail);
}
template <> inline std::string Attribute<double>::getValue () const {
   char buffer[20];
   sprintf (buffer, "%lg", attr_);
   return std::string (buffer); }
template <> inline std::string Attribute<double>::getFormattedValue () const { return getValue (); }

// Specialization of Attribute for strings
template <> inline bool Attribute<std::string>::assignFromString (const char* value) const {
   Check3 (value);
   attr_ = value;
   return true;
}
template <> inline bool Attribute<std::string>::assign (const char* value, unsigned int length) const {
   Check3 (value);
   attr_.assign (value, length);
   return true;
}
template <> inline std::string Attribute<std::string>::getValue () const { return attr_; }
template <> inline std::string Attribute<std::string>::getFormattedValue () const { return getValue (); }


/**Template for s list of attributes of a specific type.

   This class is designed to be used by AttributeValues (and derived types),
   but there exists specializations for:
     - char
     - char*
     - char* const
     - short
     - unsigned short
     - int
     - unsigned int
     - long
     - unsigned long
     - double
     - std::string
*/
template <class T> class AttributeList : public IAttribute {
 public:
   /// Constructor; creates an attribute list with the specified name, referencing the (vector of) attribute values
   AttributeList (const char* name, std::vector<T>& list) : IAttribute (name), list_ (list) { }
   /// Constructor; creates an attribute list with the specified name, referencing the (vector of) attribute values
   AttributeList (const std::string& name, std::vector<T>& list) : IAttribute (name), list_ (list) { }
   /// Destructor
   ~AttributeList () { }

   /// Method to assign a value from a character-pointer to the attribute
   /// list.
   ///
   /// The character-array is supposed to be a sequence of offset=value
   /// entries, which are separated by a semicolon (;). In case of an error
   /// (invalid offset or value) the assigning is stopped; leaving the
   /// previously (valid) entries assigned.
   /// \returns \c true on success; \c false otherwise
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

   /// Method to assign a value from a character-pointer to the attribute
   /// list. See assignFromString() for details.
   virtual bool assign (const char* value, unsigned int length) const {
      return assignFromString (value); }

   /// Method to assign a value from a character-pointer to a single
   /// (specified) element of the list.
   /// \note The \c offset is not checked vor validity!
   virtual bool assignFromString (unsigned int offset, const char* value) const {
      try {
         list_[offset] = value;
      }
      catch (std::invalid_argument&) {
         return false;
      }
      return true;
   }

   /// Method to assign a value from a character-pointer to a single
   /// (specified) element of the list. See See assignFromString() for details.
   virtual bool assign (unsigned int offset, const char* value, unsigned int length) const {
      return assignFromString (offset, value); }

   /// Returns the value of the attribute list. This is a string of <tt>
   /// [offset]=[value];</tt> entries.
   std::string getValue ()  const {
      std::string help;
      char nummer[20];
      for (unsigned int i (0); i < list_.size (); ++i) {
         sprintf (nummer, "%d", i);
         Attribute<T> value (nummer, list_[i]);
         help += value.getName ();
         help += '=';
         help += value.getValue ();
         help += ';';
      }
      return help;
   }

 private:
   AttributeList (const AttributeList&);
   const AttributeList& operator= (const AttributeList&);

   std::vector<T>& list_;
};


// Specialization of Attribute for character (arrays)
template <> inline bool AttributeList<char>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   list_[offset] = *value;
   return *value && !value[1];
}

template <> inline bool AttributeList<char*>::assign (unsigned int offset, const char* value, unsigned int length) const {
   Check3 (value);
   delete [] list_[offset];
   list_[offset] = new char[length + 1];
   if (!list_[offset])
      return false;
   memcpy (list_[offset], value, length);
   list_[offset][length] = '\0';
   return true;
}

template <> inline bool AttributeList<char*>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   return assign (offset, value, strlen (value));
}

#ifndef _MSC_VER
template <> inline bool AttributeList<char* const>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   strcpy (list_[offset], value);
   return true;
}
template <> inline bool AttributeList<char* const>::assign (unsigned int offset, const char* value, unsigned int length) const {
   Check3 (value);
   memcpy (list_[offset], value, length);
   return true;
}
#endif

// Specialization of AttributeList for ints
template <> inline bool AttributeList<short>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> inline bool AttributeList<unsigned short>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> inline bool AttributeList<int>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> inline bool AttributeList<unsigned int>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> inline bool AttributeList<long>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> inline bool AttributeList<unsigned long>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

template <> inline bool AttributeList<double>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_[offset] = strtod (value, &pTail); Check3 (pTail);
   return !(errno || *pTail);
}

// Specialization of AttributeList for strings
template <> inline bool AttributeList<std::string>::assignFromString (unsigned int offset, const char* value) const {
   Check3 (value);
   list_[offset] = value;
   return true;
}

template <> inline bool AttributeList<std::string>::assign (unsigned int offset, const char* value, unsigned int length) const {
   Check3 (value);
   list_[offset].assign (value, length);
   return true;
}

#endif
