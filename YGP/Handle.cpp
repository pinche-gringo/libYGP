// $Id: Handle.cpp,v 1.6 2002/11/30 05:59:31 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Handle
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 29.8.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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


#include "Handle.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
IHandle::~IHandle () {
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment operator; both handles points to the same object and
//            the reference-counter is increased. The count of the object
//            referenced before is decreased.
//Parameters: other: Object to clone
/*--------------------------------------------------------------------------*/
IHandle& IHandle::operator= (const IHandle& other) {
   if (getValue () != other.getValue ()) {
      if (isDefined ())
         unlink ();

      AttributValue::operator= (other);
      assignValue (other.getValue ());
      if (isDefined ())
         link ();
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Method to set the object pointed to by the handle. This method
//            cares about the reference count (and unlinks the old and links
//            the new object).
//Parameters: pValue: New value the handle shall manage
/*--------------------------------------------------------------------------*/
void IHandle::setValue (void* pValue) {
   if (isDefined ())
      unlink ();
   else
      setDefined ();

   create (pValue);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Undefines the handle; the referenced object is unlinked, if no
//            more handles point to this object, it is freed.
/*--------------------------------------------------------------------------*/
void IHandle::undefine () {
   if (isDefined ()) {
      unlink ();
      assignValue (0);
      AttributValue::undefine ();
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Defines the handle and creates a new object pointed to (with
//            reference-count 1).
/*--------------------------------------------------------------------------*/
void IHandle::define () {
   setDefined ();
   create (NULL);
}
