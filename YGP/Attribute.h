#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

//$Id: Attribute.h,v 1.40 2008/03/23 13:56:12 markus Exp $

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

#include <map>
#include <string>
#include <vector>
#include <stdexcept>

#include <YGP/Check.h>
#include <YGP/AssParse.h>
#include <YGP/MetaEnum.h>

#include <ygp-cfg.h>

namespace YGP {

/**Baseclass for attributes. Derive from it for every type of attribute.

   An attribute is defined to be the connection between a name and an
   attribute-value (which is not stored inside the attribute but only
   refereneced).
*/
class IAttribute {
 public:
   /// Destructor
   virtual ~IAttribute () { }

   /// Returns a copy of the attribute
   virtual IAttribute* clone () = 0;

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
   /// \param value: Value to assign
   /// \param : Length of passed value
   virtual bool assign (const char* value, unsigned int) const {
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

   /// Copyconstructor; clones the attribute
   IAttribute (const IAttribute& other) : name (other.name) { }

 private:
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
   ~Attribute () { }

   /// Returns a copy of the attribute
   virtual IAttribute* clone () { return new Attribute<T> (*this); }

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
   /// Copyconstructor; clones the attribute
   Attribute (const Attribute& other) : IAttribute ((IAttribute&)other),
      attr_ (other.attr_) { }
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

// Specialization of Attribute for bools
template <> inline bool Attribute<bool>::assignFromString (const char* value) const {
   Check3 (value);
   if (!(strcmp (value, "1") && strcmp (value, "true")
	 && strcmp (value, "True") && strcmp (value, "TRUE")))
      return attr_ = true;
   if (!(strcmp (value, "0") && strcmp (value, "false")
	 && strcmp (value, "False") && strcmp (value, "FALSE"))) {
      attr_ = false;
      return true;
   }
   return false;
}
template <> inline std::string Attribute<bool>::getValue () const {
   return attr_ ? "0" : "1"; }
template <> inline std::string Attribute<bool>::getFormattedValue () const {
   return attr_ ? "True" : "False"; }

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
   snprintf (buffer, sizeof (buffer), "%d", attr_);
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
   snprintf (buffer, sizeof (buffer), "%u", attr_);
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
   snprintf (buffer, sizeof (buffer), "%d", attr_);
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
   snprintf (buffer, sizeof (buffer), "%u", attr_);
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
   snprintf (buffer, sizeof (buffer), "%ld", attr_);
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
   snprintf (buffer, sizeof (buffer), "%ld", attr_);
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
   snprintf (buffer, sizeof (buffer), "%lg", attr_);
   return std::string (buffer); }
template <> inline std::string Attribute<double>::getFormattedValue () const { return getValue (); }

// Specialization of Attribute for strings
/// Assigns the passed text to the std::string
/// \returns \c true on success; \c false otherwise
template <> inline bool Attribute<std::string>::assignFromString (const char* value) const {
   Check3 (value);
   attr_ = value;
   return true;
}
/// Assigns the passed text with known length to the std::string
/// \returns \c true on success; \c false otherwise
template <> inline bool Attribute<std::string>::assign (const char* value, unsigned int length) const {
   Check3 (value);
   attr_.assign (value, length);
   return true;
}
/// Returns the value of the attribute (value)
template <> inline std::string Attribute<std::string>::getValue () const { return attr_; }
/// Returns the value of the attribute (value) as formatted string
template <> inline std::string Attribute<std::string>::getFormattedValue () const { return getValue (); }


/**Template for a list of attributes of a specific type.

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
template <class T, class L=std::vector<T> > class AttributeList : public IAttribute {
 public:
   /// Constructor; creates an attribute list with the specified name, referencing the (vector of) attribute values
   AttributeList (const char* name, L& list) : IAttribute (name), list_ (list) { }
   /// Constructor; creates an attribute list with the specified name, referencing the (vector of) attribute values
   AttributeList (const std::string& name, L& list) : IAttribute (name), list_ (list) { }

   /// Destructor
   ~AttributeList () { }

   virtual IAttribute* clone () { return new AttributeList<T, L> (*this); }

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
   /// Method to assign a value from a character-pointer to a single
   /// (specified) element of the list.
   /// \note The \c offset is not checked vor validity!
   virtual bool assignFromString (unsigned int offset, const char* value) const {
      try {
	 if (offset == list_.size ())
  	    list_.push_back (T ());
	 doAssignFromString (offset, value);
      }
      catch (std::exception& e) {
         return false;
      }
      return true;
   }

   /// Method to assign a value from a character-pointer to a single
   /// (specified) element of the list. See See assignFromString() for details.
   virtual bool assign (unsigned int offset, const char* value, unsigned int length) const {
      return assignFromString (offset, value); }
   /// Method to assign a value from a character-pointer to the attribute
   /// list. See assignFromString() for details.
   virtual bool assign (const char* value, unsigned int length) const {
      return assignFromString (value); }

   /// Returns the value of the attribute list. This is a string of <tt>
   /// [offset]=[value];</tt> entries.
   std::string getValue ()  const {
      std::string help;
      char number[20];
      for (unsigned int i (0); i < list_.size (); ++i) {
         snprintf (number, sizeof (number), "%d", i);
         help += number;
         help += '=';
         help += list_[i];
         help += ';';
      }
      return help;
   }

   /// Checks if the passed value specifies a valid offset
   /// \returns \c true, if the offset is valid
   bool isValidOffset (unsigned int offset) const {
      return offset <= list_.size ();
   }

 private:
   AttributeList (const AttributeList& o) : IAttribute ((const IAttribute&)o),
      list_ (o.list_) { }
   const AttributeList& operator= (const AttributeList&);

   bool doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
      list_.at (offset) = value;
      return true;
   }

   L& list_;
};


/// Specialization of Attribute::doAssignFromString for a single character
template <> inline bool AttributeList<char>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   list_.at (offset) = *value;
   return *value && !value[1];
}

/// Specialization of Attribute::assign for character-arrays
template <> inline bool AttributeList<char*>::assign (unsigned int offset, const char* value, unsigned int length) const {
   Check3 (value);
   try {
      if (list_.size () == offset)
	 list_.push_back (new char [length + 1]);
      else {
	 if (strlen (list_.at (offset)) < length) {
	    delete [] list_[offset];
	    list_[offset] = new char[length + 1];
	    if (!list_[offset])
	       return false;
	 }
      }
      memcpy (list_[offset], value, length);
      list_[offset][length] = '\0';
   }
   catch (std::exception&) {
      return false;
   }
   return true;
}

/// Specialization of Attribute::doAssignFromString for character-arrays
template <> inline bool AttributeList<char*>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   return assign (offset, value, strlen (value));
}

/// Specialization of AttributeList::doAssginFromString for short
template <> inline bool AttributeList<short>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_.at (offset) = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeList::doAssginFromString for unsigned short
template <> inline bool AttributeList<unsigned short>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_.at (offset) = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeList::doAssignFromString for int
template <> inline bool AttributeList<int>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_.at (offset) = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeList::doAssignFromString for unsigned int
template <> inline bool AttributeList<unsigned int>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_.at (offset) = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeList::doAssignFromString for unsigned long
template <> inline bool AttributeList<long>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_.at (offset) = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeList::doAssignFromString for unsigned long
template <> inline bool AttributeList<unsigned long>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_.at (offset) = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeList::doAssignFromString for double
template <> inline bool AttributeList<double>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   list_.at (offset) = strtod (value, &pTail); Check3 (pTail);
   return !(errno || *pTail);
}

/// Assigns the passed text to the list of std::strings
/// \returns \c true on success; \c false otherwise
template <> inline bool AttributeList<std::string>::doAssignFromString (unsigned int offset, const char* value) const throw (std::out_of_range) {
   Check3 (value);
   list_.at (offset) = value;
   return true;
}

/// Assigns the passed text with known length to the list of std::strings
/// \returns \c true on success; \c false otherwise
template <> inline bool AttributeList<std::string>::assign (unsigned int offset, const char* value, unsigned int length) const {
   Check3 (value);
   try {
      if (list_.size () == offset)
	 list_.push_back (std::string (value, length));
      else
	 list_.at (offset).assign (value, length);
   }
   catch (std::exception&) {
      return false;
   }
   return true;
}


/**An attribute to assign an integer-value from a list of string-values
 */
class MetaEnumAttribute : public IAttribute {
 public:
   /// Constructor; creates an attribute with the specified name, referencing the attribute value
   MetaEnumAttribute (const char* name, const MetaEnum& list, unsigned int& attr) : IAttribute (name), attr_ (attr), list_ (list) { }
   /// Constructor; creates an attribute with the specified name, referencing the attribute value
   MetaEnumAttribute (const std::string& name, const MetaEnum& list, unsigned int& attr) : IAttribute (name), attr_ (attr), list_ (list) { }
   /// Destructor
   ~MetaEnumAttribute () { }

   /// Returns a copy of the attribute
   virtual IAttribute* clone () { return new MetaEnumAttribute (*this); }

   virtual bool assignFromString (const char* value) const;
   virtual bool assign (const char* value, unsigned int) const {
      return assignFromString (value); }

   /// Returns a reference to the handled attribute value
   unsigned int& getAttribute () const { return attr_; }
   virtual std::string getValue () const {
      char buffer[20];
      snprintf (buffer, sizeof (buffer), "%d", attr_);
      return std::string (buffer); }
   virtual std::string getFormattedValue () const { return list_[attr_]; }

 private:
   /// Copyconstructor; clones the attribute
   MetaEnumAttribute (const MetaEnumAttribute& other) : IAttribute ((IAttribute&)other),
      attr_ (other.attr_), list_ (other.list_) { }
   const MetaEnumAttribute& operator= (const MetaEnumAttribute&);

   unsigned int& attr_;
   const MetaEnum& list_;
};


/**Template for a map of attributes of a specific type.

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
template <class T, class L=std::map<std::string, T> > class AttributeMap : public IAttribute {
 public:
   /// Constructor; creates an attribute map with the specified name, referencing the (vector of) attribute values
   AttributeMap (const char* name, L& map) : IAttribute (name), map_ (map) { }
   /// Constructor; creates an attribute map with the specified name, referencing the (vector of) attribute values
   AttributeMap (const std::string& name, L& map) : IAttribute (name), map_ (map) { }
   /// Destructor
   ~AttributeMap () { }

   virtual IAttribute* clone () { return new AttributeMap<T, L> (*this); }

   /// Method to assign a value from a character-pointer to the attribute
   /// map.
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
            if (!assignFromString (parse.getActKey (), parse.getActValue ().c_str ()))
                return false;
         }
         catch (std::invalid_argument&) {
            return false;
         }
      }
      return true;
   }

   /// Method to assign a value from a character-pointer to the attribute
   /// map. See assignFromString() for details.
   virtual bool assign (const char* value, unsigned int length) const {
      return assignFromString (value); }

   /// Method to assign a value from a character-pointer to a single
   /// (specified) element of the map.
   /// \note The \c offset is not checked vor validity!
   virtual bool assignFromString (const std::string& offset, const char* value) const {
      try {
         map_[offset] = value;
      }
      catch (std::invalid_argument&) {
         return false;
      }
      return true;
   }

   /// Method to assign a value from a character-pointer to a single
   /// (specified) element of the map. See See assignFromString() for details.
   virtual bool assign (const std::string& offset, const char* value, unsigned int length) const {
      return assignFromString (offset, value); }

   /// Returns the value of the attribute map. This is a string of
   /// <tt>[offset]=[value];</tt> entries.
   std::string getValue ()  const {
      std::string help;
      for (typename L::const_iterator i (map_.begin ()); i != map_.end (); ++i) {
         help += i->first;
         help += '=';
         help += i->second;
         help += ';';
      }
      return help;
   }

 private:
   AttributeMap (const AttributeMap& o) : IAttribute ((const IAttribute&)o),
      map_ (o.map_) { }
   const AttributeMap& operator= (const AttributeMap&);

   L& map_;
};


/// Specialization of Attribute::assignFromString for a single character
template <> inline bool AttributeMap<char>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   map_[offset] = *value;
   return *value && !value[1];
}

/// Specialization of Attribute::assign for character-arrays
template <> inline bool AttributeMap<char*>::assign (const std::string& offset, const char* value, unsigned int length) const {
   Check3 (value);
   delete [] map_[offset];
   map_[offset] = new char[length + 1];
   if (!map_[offset])
      return false;
   memcpy (map_[offset], value, length);
   map_[offset][length] = '\0';
   return true;
}

/// Specialization of Attribute::assignFromString for character-arrays
template <> inline bool AttributeMap<char*>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   return assign (offset, value, strlen (value));
}

/// Specialization of AttributeMap::assginFromString for short
template <> inline bool AttributeMap<short>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   map_[offset] = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeMap::assginFromString for unsigned short
template <> inline bool AttributeMap<unsigned short>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   map_[offset] = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeMap::assginFromString for int
template <> inline bool AttributeMap<int>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   map_[offset] = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeMap::assginFromString for unsigned int
template <> inline bool AttributeMap<unsigned int>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   map_[offset] = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeMap::assginFromString for unsigned long
template <> inline bool AttributeMap<long>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   map_[offset] = strtol (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeMap::assginFromString for unsigned long
template <> inline bool AttributeMap<unsigned long>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   map_[offset] = strtoul (value, &pTail, 10); Check3 (pTail);
   return !(errno || *pTail);
}

/// Specialization of AttributeMap::assginFromString for double
template <> inline bool AttributeMap<double>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   char* pTail = NULL;
   errno = 0;
   map_[offset] = strtod (value, &pTail); Check3 (pTail);
   return !(errno || *pTail);
}

/// Assigns the passed text to the map of std::strings
/// \returns \c true on success; \c false otherwise
template <> inline bool AttributeMap<std::string>::assignFromString (const std::string& offset, const char* value) const {
   Check3 (value);
   map_[offset] = value;
   return true;
}

/// Assigns the passed text with known length to the map of std::strings
/// \returns \c true on success; \c false otherwise
template <> inline bool AttributeMap<std::string>::assign (const std::string& offset, const char* value, unsigned int length) const {
   Check3 (value);
   map_[offset].assign (value, length);
   return true;
}

}

#endif
