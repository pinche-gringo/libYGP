#ifndef ENTITY_H
#define ENTITY_H

//$Id: Entity.h,v 1.3 2002/11/10 23:06:43 markus Exp $

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
#include <algorithm>

#ifndef NO_HANDLE
#  include "Handle.h"
#endif

#include <Check.h>
#include <Attribute.h>


// Baseclass for classes holding (a list of) attributes
class Entity {
   friend class INIFile;

 public:
   Entity () { };
   virtual ~Entity ();

   const IAttribute* findAttribute (const char* name) const;
   const IAttribute* findAttribute (const std::string& name) const;

   void addAttribute (const IAttribute& newAttr) {
      Check3 (std::find (attributes.begin (), attributes.end (), &newAttr)
              == attributes.end ());
      attributes.push_back (&newAttr);
   }

#ifdef __STL_MEMBER_TEMPLATES
   // Offer another way to add attributes on plattforms supporting that
   // Note that this is not portable!
   template<class AttrType>
      void addAttribute (const char* name, AttrType& attr) {
      assert (name);
      addAttribute (new Attribute<AttrType> (name, attr));  }
   template<class AttrType> 
      void addAttribute (const std::string& name, AttrType& attr) {
      addAttribute (new Attribute<AttrType> (name, attr));  }
#endif

 private:
   std::vector <const IAttribute*> attributes;
};


#ifndef NO_HANDLE
defineHndl (Entity);
#endif

#endif
