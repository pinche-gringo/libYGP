//$Id: DynTrace.cpp,v 1.1 2006/05/04 01:17:45 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : DynTrace
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 03.05.2006
//COPYRIGHT   : Copyright (C) 2006

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


#include <YGP/Check.h>

#include "DynTrace.h"

#undef TRACE
#undef TRACE0
#undef TRACE1
#undef TRACE2
#undef TRACE3
#undef TRACE4
#undef TRACE5
#undef TRACE6
#undef TRACE7
#undef TRACE8
#undef TRACE9

#include <YGP/Trace.h>

namespace YGP {


std::map<const char*, DynTrace*, DynTraceManager::lessDereferenced> DynTraceManager::traceObjs;


//-----------------------------------------------------------------------------
/// Constructor
/// \param subsystem: Name of subsystem
//-----------------------------------------------------------------------------
DynTrace::DynTrace (const char* subsystem) : level (-1) {
   TRACE1 ("DynTrace::DynTrace (const std::string&) - " << subsystem);

   DynTraceManager::add (subsystem, this);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
DynTrace::~DynTrace () {
   DynTraceManager::remove (this);
}


//-----------------------------------------------------------------------------
/// Removes the passed relation
/// \param relation: Relation to remove
//-----------------------------------------------------------------------------
void DynTraceManager::remove (const DynTrace* traceObj) {
   TRACE9 ("DynTraceManager::remove (const DynTrace*)");
   Check1 (traceObj);

   for (std::map<const char*, DynTrace*>::iterator i (traceObjs.begin ());
	i != traceObjs.end (); ++i)
      if (i->second == traceObj) {
	 traceObjs.erase (i);
	 return;
      }
   Check3 (0);
}

}
