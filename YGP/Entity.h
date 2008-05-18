#ifndef YGP_ENTITY_H
#define YGP_ENTITY_H

//$Id: Entity.h,v 1.14 2008/05/18 13:21:27 markus Rel $

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


#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif

#include <string>
#include <vector>
#include <iosfwd>
#include <algorithm>

#ifndef NO_HANDLE
#  include "Handle.h"
#endif

#include <YGP/Check.h>
#include <YGP/Attribute.h>


namespace YGP {

/**Baseclass for classes holding (a list of) attributes

   The descrutor frees all managed attributes, so make sure to create
   them on the heap.

   Note that you can automatically manage the handled attributes with the
   utility \c mgeni (in the bin subdirectory).
*/
class Entity {
   friend class INIFile;

 public:
   /// Default constructor
   Entity () : attributes () { };
   virtual ~Entity ();

   IAttribute* findAttribute (const char* name) const;
   IAttribute* findAttribute (const std::string& name) const;

   /// Adds a new attribute to the entity.
   void addAttribute (IAttribute& newAttr) {
      Check3 (std::find (attributes.begin (), attributes.end (), &newAttr)
              == attributes.end ());
      attributes.push_back (&newAttr);
   }

   friend std::ostream& operator<< (std::ostream& out, const Entity& obj) throw ();
   friend std::istream& operator>> (std::istream& in, Entity& obj) throw ();


#ifdef __STL_MEMBER_TEMPLATES
   /// Add a copy of an attribute with a specific type to the entity.
   /// \note This is not portable!
   template <class AttrType>
      void addAttribute (const char* name, const AttrType& attr) {
      Check1 (name);
      addAttribute (new Attribute<AttrType> (name, attr));  }
   /// Add a copy of an attribute with a specific type to the entity.
   /// \note This is not portable!
   template <class AttrType>
      void addAttribute (const std::string& name, const AttrType& attr) {
      addAttribute (new Attribute<AttrType> (name, attr));  }
#endif

 private:
   std::vector <IAttribute*> attributes;
};


#ifndef NO_HANDLE
/// Defines a handle (smart pointer with reference counting) to an Entity
defineHndl (Entity);
#endif

}

#endif
