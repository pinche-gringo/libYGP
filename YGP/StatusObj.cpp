//$Id: StatusObj.cpp,v 1.4 2004/01/15 06:26:30 markus Rel $

//PROJECT     : Cardgames
//SUBSYSTEM   : <FILLIN>
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
//AUTHOR      : Markus Schwab
//CREATED     : 19.11.2003
//COPYRIGHT   : Copyright (C) 2003, 2004

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


#include <YGP/StatusObj.h>


namespace YGP {

//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
StatusObject::StatusObject () : tp (UNDEFINED) {
}

//-----------------------------------------------------------------------------
/// Constructor from a type and a message
/// \param t: Type of the object
/// \param message: Message of the object
//-----------------------------------------------------------------------------
StatusObject::StatusObject (type t, const std::string& message)
    : tp (INFO), msg (message) {
}

//-----------------------------------------------------------------------------
/// Copy constructor
/// \param other: Object to copy
//-----------------------------------------------------------------------------
StatusObject::StatusObject (const StatusObject& other)
    : tp (other.tp), msg (other.msg)
      , child (other.child ? new StatusObject (*other.child) : NULL) {
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
StatusObject::~StatusObject () {
}


//-----------------------------------------------------------------------------
/// Assignment operator
/// \param other: Object to copy the values from
//-----------------------------------------------------------------------------
StatusObject& StatusObject::operator= (const StatusObject& other) {
   if (&other != this) {
      tp = other.tp;
      msg = other.msg;
      child = other.child ? new StatusObject (*other.child) : NULL;
   }

   return *this;
}

//----------------------------------------------------------------------------
/// Sets a new status message (old messages are preserved)
/// \param t: Type of the object
/// \param message: Message of the object
/// \remarks - The message is appended to existing ones
///     - If the passed type is more important than the old one, it is changed 
//----------------------------------------------------------------------------
void StatusObject::setMessage (type t, const std::string& message) {
   msg += message;
   if (t > tp)
      tp = t;
}    

//----------------------------------------------------------------------------
/// Sets a new status message (old messages are preserved as "details")
/// \param message: Message of the object
/// \remarks - The message replaces existing ones
///     - The old messages are accessable as "details"
//----------------------------------------------------------------------------
void StatusObject::generalize (const std::string& message) {
    if (child)
       child->msg += msg;
    else
       child = new StatusObject (tp, msg);

    msg = message;
}

}
