//$Id: Entity.cpp,v 1.9 2004/11/04 16:31:18 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Entity
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.9 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.3.2002
//COPYRIGHT   : Copyright (C) 2002 - 2004

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


#include "YGP/Entity.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
Entity::~Entity () {
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      delete *i;
}


//-----------------------------------------------------------------------------
/// Tries to find an attribute with the specified name.
/// \param name: Name of attribute to find
/// \returns \c IAttribute*: Pointer to attribute or NULL (if not found)
//-----------------------------------------------------------------------------
const IAttribute* Entity::findAttribute (const char* name) const {
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;
   
   return NULL;
}
   
//-----------------------------------------------------------------------------
/// Tries to find an attribute with the specified name.
/// \param name: Name of attribute to find
/// \returns \c IAttribute*: Pointer to attribute or NULL (if not found)
//-----------------------------------------------------------------------------
const IAttribute* Entity::findAttribute (const std::string& name) const {
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;
   
   return NULL;
}

}
