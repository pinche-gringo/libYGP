#ifndef ENTITY_H
#define ENTITY_H

//$Id: Entity.h,v 1.1 2002/03/23 19:13:06 markus Rel $

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


#include <string>
#include <vector>
#include <algo.h>

#ifndef NO_HANDLE
#  include "Handle.h"
#endif

class IAttribute;

class Entity {
 public:
   Entity () { };
   virtual ~Entity ();

   const IAttribute* findAttribute (const char* name) const;
   const IAttribute* findAttribute (const std::string& name) const;

   void addAttribute (const IAttribute& newAttr) {
#ifndef NDEBUG
      if (find (attributes.begin (), attributes.end (), &newAttr))
	 assert (0);
#endif
      attributes.push_back (&newAttr);
   }

 private:
   std::vector <const IAttribute*> attributes;
};


#ifndef NO_HANDLE
defineHndl (Entity);
#endif

#endif