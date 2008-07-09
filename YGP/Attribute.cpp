//$Id: Attribute.cpp,v 1.2 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : MetaEnum
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
//AUTHOR      : Markus Schwab
//CREATED     : 2.8.2006
//COPYRIGHT   : Copyright (C) 2006, 2008

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


#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/ANumeric.h>

#include <YGP/Attribute.h>


namespace YGP {

//-----------------------------------------------------------------------------
/// Assigs a string-value to the attribute; This value must be defined in the MetaEnum
/// \param value Value to assign
/// \returns bool True, if value was assigned successfully
//-----------------------------------------------------------------------------
bool MetaEnumAttribute::assignFromString (const char* value) const {
   Check1 (value);
   TRACE1 ("MetaEnumAttribute::assignFromString (const char*) - " << value);
   try {
      attr_ = list_[value];
      TRACE9 ("MetaEnumAttribute::assignFromString (const char*) - Assigned: " << attr_);
   }
   catch (std::out_of_range&) {
      try {
	 ANumeric nr (value);
	 unsigned int iVal (nr);
	 if (list_.exists (iVal)) {
	    attr_ = iVal;
	    TRACE9 ("MetaEnumAttribute::assignFromString (const char*) - Numeric: " << attr_);
	 }
      }
      catch (std::invalid_argument&) {
	 return false;
      }
   }
   return true;
}

}

