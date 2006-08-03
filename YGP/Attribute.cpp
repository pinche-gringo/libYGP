//$Id: Attribute.cpp,v 1.1 2006/08/03 17:45:29 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : MetaEnum
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 2.8.2006
//COPYRIGHT   : Copyright (C) 2006

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


#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/ANumeric.h>

#include <YGP/Attribute.h>


namespace YGP {

//-----------------------------------------------------------------------------
/// Assigs a string-value to the attribute; This value must be defined in the MetaEnum
/// \param value: Value to assign
/// \returns bool: True, if value was assigned successfully
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

