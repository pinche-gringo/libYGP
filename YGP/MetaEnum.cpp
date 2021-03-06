//$Id: MetaEnum.cpp,v 1.5 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : MetaEnum
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
//AUTHOR      : Markus Schwab
//CREATED     : 06.12.2004
//COPYRIGHT   : Copyright (C) 2004, 2008

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
/// \param value Value to return as string
/// \returns std::string Value as string
/// \remarks value must exist within the enum
/// \throws std::out_of_range
//-----------------------------------------------------------------------------
std::string MetaEnum::operator[] (int value) const throw (std::out_of_range) {
   const_iterator i (values.find (value));
   if (i != values.end ())
      return i->second;
   throw std::out_of_range ("MetaEnum::operator[] (int)");
}

//-----------------------------------------------------------------------------
/// Returns the numeric value of the passed string
/// \param value String Value to return numeric
/// \returns int Numeric value
/// \remarks value must exist within the enum
//-----------------------------------------------------------------------------
int MetaEnum::operator[] (const std::string& value) const throw (std::out_of_range) {
   for (const_iterator i (values.begin ()); i != values.end (); ++i)
      if (i->second == value)
	 return i->first;
   throw std::out_of_range ("MetaEnum::operator[] (const std::string&)");
}

//-----------------------------------------------------------------------------
/// Checks if the passed string-value exists within the enum
/// \param value Value to check for
/// \returns bool
/// \remarks value must exist within the enum
//-----------------------------------------------------------------------------
bool MetaEnum::exists (const std::string& value) const {
   for (const_iterator i (values.begin ()); i != values.end (); ++i)
      if (i->second == value)
	 return true;
   return false;
}

}
