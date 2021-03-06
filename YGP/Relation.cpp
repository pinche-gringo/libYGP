//$Id: Relation.cpp,v 1.6 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Relation
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.10.2004
//COPYRIGHT   : Copyright (C) 2004 - 2006, 2008, 2009

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


#include "YGP/Trace.h"


#include "YGP/Relation.h"


namespace YGP {


std::map<const char*, IRelation*> RelationManager::relations;


//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
IRelation::IRelation (const char* name) {
   TRACE9 ("IRelation::IRelation (const char*) - " << name);

   RelationManager::add (name, this);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
IRelation::~IRelation () {
   TRACE9 ("IRelation::~IRelation ()");

   RelationManager::remove (this);
}


//-----------------------------------------------------------------------------
/// Removes the passed relation
/// \param relation Relation to remove
//-----------------------------------------------------------------------------
void RelationManager::remove (const IRelation* relation) {
   TRACE9 ("RelationManager::remove (const IRelation*)");
   Check1 (relation);

   for (std::map<const char*, IRelation*>::iterator i (relations.begin ());
	i != relations.end (); ++i)
      if (i->second == relation) {
	 relations.erase (i);
	 return;
      }
   Check3 (0);
}

//-----------------------------------------------------------------------------
/// Retuns the relation with the passed name
/// \param name Name of relation to find
/// \returns IRelation* Relation searched for or NULL
//-----------------------------------------------------------------------------
IRelation* RelationManager::getRelation (const char* name) {
   TRACE9 ("RelationManager::getRelation (const char*) - " << name);
   Check1 (name);

   std::map<const char*, IRelation*>::iterator i (relations.find (name));
   return (i != relations.end ()) ?  i->second : NULL;
}

//-----------------------------------------------------------------------------
/// Retuns the name of the passed relation
/// \param relation Relation to get the name for
/// \returns const char* Name of the passed relation
//-----------------------------------------------------------------------------
const char* RelationManager::getRelationName (const IRelation& relation) {
   std::map<const char*, IRelation*>::iterator i (relations.begin ());
   for (; i != relations.end (); ++i)
      if (i->second == &relation)
	 break;
   Check (i != relations.end ());
   return i->first;
}


}

