//$Id: Relation.cpp,v 1.1 2004/10/23 06:43:09 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : Relation
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.10.2004
//COPYRIGHT   : Copyright (C) 2004, 2005

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



#define CHECK 9
#define TRACELEVEL 9
#include "YGP/Trace.h"


#include "YGP/Relation.h"


namespace YGP {


std::vector<IRelation*> RelationManager::relations;


//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
IRelation::IRelation () {
   TRACE9 ("IRelation::IRelation ()");

   RelationManager::add (this);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
IRelation::~IRelation () {
   TRACE9 ("IRelation::~IRelation ()");

   RelationManager::remove ( this);
}



//-----------------------------------------------------------------------------
/// Removes all stored relations
//-----------------------------------------------------------------------------
void RelationManager::removeAll () {
   TRACE9 ("RelationManager::removeAll ()");

   for (std::vector<IRelation*>::iterator i (relations.begin ());
	i != relations.end (); ++i)
      delete *i;
}

//-----------------------------------------------------------------------------
/// Removes the passed relation
/// \param relation: Relation to remove
//-----------------------------------------------------------------------------
void RelationManager::remove (const IRelation* relation) {
   TRACE9 ("RelationManager::remove (const IRelation*)");

   std::vector<IRelation*>::iterator i (find (relations.begin (),
					      relations.end (), relation));
   Check3 (i != relations.end ());
   relations.erase (i);
}

}

