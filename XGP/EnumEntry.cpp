//$Id: EnumEntry.cpp,v 1.1 2005/04/28 22:41:32 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : EnumEntry
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 28.04.2005
//COPYRIGHT   : Copyright (C) 2005

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


#include <YGP/MetaEnum.h>

#include "EnumEntry.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param values: MetaEnum whose values should be shown
//-----------------------------------------------------------------------------
EnumEntry::EnumEntry (const YGP::MetaEnum& values) {
   for (YGP::MetaEnum::const_iterator i (values.begin ()); i != values.end (); ++i)
      append_text (i->second);
   set_active (0);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
EnumEntry::~EnumEntry () {
}

}
