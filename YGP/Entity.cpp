//$Id: Entity.cpp,v 1.17 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Entity
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.17 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.3.2002
//COPYRIGHT   : Copyright (C) 2002 - 2006, 2008

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


#include <iostream>

#include "Check.h"
#include "Trace.h"
#include "AttrParse.h"

#include "Entity.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
Entity::~Entity () {
   std::vector<IAttribute*>::iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i) {
      TRACE9 ("Entity::~Entity () - " << (*i)->getName ().c_str ());
      delete *i;
   }
}


//-----------------------------------------------------------------------------
/// Tries to find an attribute with the specified name.
/// \param name: Name of attribute to find
/// \returns \c IAttribute*: Pointer to attribute or NULL (if not found)
//-----------------------------------------------------------------------------
IAttribute* Entity::findAttribute (const char* name) const {
   Check1 (name);
   std::vector<IAttribute*>::const_iterator i;
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
IAttribute* Entity::findAttribute (const std::string& name) const {
   std::vector<IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

//-----------------------------------------------------------------------------
/// Writes the attributes to a stream.
/// \param out: Stream to write to
/// \param obj: Object to write
/// \returns std::ostream&: The stream
/// \throws Anything that operator<< of the attributes might throw
//-----------------------------------------------------------------------------
std::ostream& operator<< (std::ostream& out, const Entity& obj) throw () {
   std::vector<IAttribute*>::const_iterator i;
   std::string output;
   for (i = obj.attributes.begin (); i != obj.attributes.end (); ++i)
      output += AssignmentParse::makeAssignment ((*i)->getName ().c_str (),
						 (*i)->getValue ());
   return out << output << '\n';
}

//-----------------------------------------------------------------------------
/// Reads the attributes from a stream
/// \param in: Stream to read from
/// \param obj: Object to read
/// \returns std::ostream&: The stream
/// \throws Anything that operator>> of the attributes might throw
//-----------------------------------------------------------------------------
std::istream& operator>> (std::istream& in, Entity& obj) throw () {
   AttributeParse attrs;
   for (std::vector<IAttribute*>::iterator i (obj.attributes.begin ());
	i != obj.attributes.end (); ++i) {
      TRACE9 ("operator>> (std::istream&, Entity& - Attribute " << (*i)->getName ());
      attrs.addAttribute (*(*i)->clone ());
   }

   char buffer[80];
   std::string input;
   do {
      in.clear ();
      in.getline (buffer, sizeof (buffer));
      input.append (buffer, in.gcount ());
   } while (in.fail () && !in.eof ()); // end-do
   TRACE5 ("operator>> (std::istream&, Entity& - Assign from " << input);

   attrs.assignValues (input.c_str ());
   return in;
}

}
