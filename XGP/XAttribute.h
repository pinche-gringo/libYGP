#ifndef XATTRIBUTE_H
#define XATTRIBUTE_H

//$Id: XAttribute.h,v 1.1 2003/07/08 18:05:42 markus Rel $

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


#include <glibmm/ustring.h>

#include <Attribute.h>

// Specialization of Attribute<T> for Glib::ustring
template <> inline bool Attribute<Glib::ustring>::assignFromString (const char* value) const {
   Check3 (value);
   attr_ = value;
   return true;
}
template <> inline bool Attribute<Glib::ustring>::assign (const char* value, unsigned int length) const {
   Check3 (value);
   attr_.assign (value, length);
   return true;
}
template <> inline std::string Attribute<Glib::ustring>::getValue () const { return attr_; }
template <> inline std::string Attribute<Glib::ustring>::getFormattedValue () const { return getValue (); }

#endif
