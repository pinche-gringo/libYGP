//$Id: Entity.cpp,v 1.2 2002/07/15 21:08:59 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Entity
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.3.2002
//COPYRIGHT   : Anticopyright (A) 2002

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


#include "Attribute.h"

#include "Entity.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
Entity::~Entity () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Tries to find an attribute with the specified name in the
//            entity
//Parameters: name: Name of attribute to find
//Returns   : IAttribute*: Pointer to attribute or NULL (if not found)
/*--------------------------------------------------------------------------*/
const IAttribute* Entity::findAttribute (const char* name) const {
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;
   
   return NULL;
}
   
/*--------------------------------------------------------------------------*/
//Purpose   : Tries to find an attribute with the specified name in the
//            section
//Parameters: name: Name of attribute to find
//Returns   : IAttribute*: Pointer to attribute or NULL (if not found)
/*--------------------------------------------------------------------------*/
const IAttribute* Entity::findAttribute (const std::string& name) const {
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;
   
   return NULL;
}
