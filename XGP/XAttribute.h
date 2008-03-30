#ifndef XATTRIBUTE_H
#define XATTRIBUTE_H

//$Id: XAttribute.h,v 1.10 2008/03/30 13:39:17 markus Rel $

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


#include <glibmm/ustring.h>

#include <YGP/Attribute.h>


namespace YGP {

/// Specialization of Attribute<T>::assignFromString for Glib::ustring
template <> inline bool Attribute<Glib::ustring>::assignFromString (const char* value) const {
   Check3 (value);
   attr_ = value;
   return true;
}
/// Specialization of Attribute<T>::getValue for Glib::ustring
template <> inline std::string YGP::Attribute<Glib::ustring>::getValue () const { return attr_; }
/// Specialization of Attribute<T>::getFormattedValue for Glib::ustring
template <> inline std::string YGP::Attribute<Glib::ustring>::getFormattedValue () const { return getValue (); }

}


#endif
