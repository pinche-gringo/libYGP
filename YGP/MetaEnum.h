#ifndef METAENUM_H
#define METAENUM_H

//$Id: MetaEnum.h,v 1.2 2005/01/20 05:27:02 markus Rel $

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
class MetaEnum : public std::map<int, std::string> {
 public:
   /// \name Check for existence
   //@{
   /// Checks if the integer value exists
   /// \param value: Value to check for
   /// \returns bool: True if the value exists
   bool exists (int value) { return find (value) != end (); }
   bool exists (const std::string& value);
   //@}

   /// \name Accessing values (which must exist)
   //@{
   std::string operator[] (int value) throw (std::out_of_range);
   int         operator[] (const std::string& value) throw (std::out_of_range);
   //@}

 protected:
   MetaEnum () { }
   virtual ~MetaEnum ();

 private:
   // Prohibited manager methods
   MetaEnum (const MetaEnum& other);
   const MetaEnum& operator= (const MetaEnum& other);
};


}

#endif
