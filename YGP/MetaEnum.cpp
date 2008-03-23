//$Id: MetaEnum.cpp,v 1.3 2008/03/23 13:56:12 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : MetaEnum
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 06.12.2004
//COPYRIGHT   : Copyright (C) 2004, 2008

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


#include <algorithm>

#include <YGP/Check.h>

#include <YGP/MetaEnum.h>


namespace YGP {

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
MetaEnum::~MetaEnum () {
}


//-----------------------------------------------------------------------------
/// Returns the string-value of the passed value
/// \param value: Value to return as string
/// \returns std::string: Value as string
/// \remarks value must exist within the enum
/// \throws std::out_of_range
//-----------------------------------------------------------------------------
std::string MetaEnum::operator[] (int value) const throw (std::out_of_range) {
   std::map<int, std::string>::const_iterator i (values.find (value));
   if (i != values.end ())
      return i->second;
   throw std::out_of_range ("MetaEnum::operator[] (int)");
}

//-----------------------------------------------------------------------------
/// Returns the numeric value of the passed string
/// \param value: String Value to return numeric
/// \returns int: Numeric value
/// \remarks value must exist within the enum
//-----------------------------------------------------------------------------
int MetaEnum::operator[] (const std::string& value) const throw (std::out_of_range) {
   for (std::map<int, std::string>::const_iterator i (values.begin ());
	i != values.end (); ++i)
      if (i->second == value)
	 return i->first;
   throw std::out_of_range ("MetaEnum::operator[] (const std::string&)");
}

//-----------------------------------------------------------------------------
/// Checks if the passed string-value exists within the enum
/// \param value: Value to check for
/// \returns bool
/// \remarks value must exist within the enum
//-----------------------------------------------------------------------------
bool MetaEnum::exists (const std::string& value) const {
   for (std::map<int, std::string>::const_iterator i (values.begin ());
	i != values.end (); ++i)
      if (i->second == value)
	 return true;
   return false;
}

}
