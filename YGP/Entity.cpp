//$Id: Entity.cpp,v 1.11 2005/01/10 02:19:05 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : Entity
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.11 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.3.2002
//COPYRIGHT   : Copyright (C) 2002 - 2005

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


#define TRACELEVEL 2
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
   out << output;
   return out;
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
      TRACE1 ("operator>> (std::istream&, Entity& - Attribute " << (*i)->getName ());
      attrs.addAttribute (*(*i)->clone ());
   }

   char buffer[200];
   std::string input;
   do {
      in.clear ();
      in.getline (buffer, sizeof (buffer));
      input.append (buffer, std::cin.gcount ());
   } while (in.fail () && !in.eof ()); // end-do
   TRACE1 ("operator>> (std::istream&, Entity& - Assign from " << input);

   attrs.assignValues (input.c_str ());
   return in;
}

}
