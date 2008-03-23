#ifndef METAENUM_H
#define METAENUM_H

//$Id: MetaEnum.h,v 1.5 2008/03/23 17:34:23 markus Exp $

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


#include <map>
#include <string>
#include <stdexcept>


namespace YGP {

/**Class to handle enumerations; in counterpart to C-style enums those values
 * can be accessed by number and name.
 */
class MetaEnum {
 public:
   typedef std::pair<int, std::string>  typePair;
   typedef std::map<int, std::string>   typeEnum;
   typedef typeEnum::iterator           iterator;
   typedef typeEnum::const_iterator     const_iterator;

   /// \name Check for existence
   //@{
   /// Checks if the integer value exists
   /// \param value: Value to check for
   /// \returns bool: True if the value exists
   bool exists (int value) const { return values.find (value) != values.end (); }
   bool exists (const std::string& value) const;
   //@}

   /// Inserts a new item into the object
   /// \param value: Value to insert; create with std::make_pair
   void insert (const typePair& value) { values.insert (value); }

   /// \name Accessing values (which must exist)
   //@{
   std::string operator[] (int value) const throw (std::out_of_range);
   int         operator[] (const std::string& value) const throw (std::out_of_range);
   //@}

   /// Returns an iterator to the first element of the enumeration
   /// \retunrs: Iterator to the first element
   iterator begin () { return values.begin (); }
   /// Returns an iterator to the first element of the enumeration
   /// \retunrs: Iterator to the first element
   const_iterator begin () const { return values.begin (); }
   /// Returns an iterator after the last element of the enumeration
   /// \retunrs: Iterator after the last element
   iterator end () { return values.end (); }
   /// Returns an iterator after the last element of the enumeration
   /// \retunrs: Iterator after the last element
   const_iterator end () const { return values.end (); }

 protected:
   MetaEnum () : values () { }
   virtual ~MetaEnum ();

 private:
   // Prohibited manager methods
   MetaEnum (const MetaEnum& other);
   const MetaEnum& operator= (const MetaEnum& other);

   std::map<int, std::string> values;
};


}

#endif
