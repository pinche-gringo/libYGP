// $Id: Handle.cpp,v 1.1 1999/08/29 20:53:13 Markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Handle
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 29.8.1999
//COPYRIGHT   : Anticopyright (A) 1999

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
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
IHandle::IHandle (void* pValue) : AttributValue () {
   AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
IHandle::~IHandle () {
}

/*--------------------------------------------------------------------------*/
//Purpose     : Assignment-operator
//Parameters  : other: Object to clone
/*--------------------------------------------------------------------------*/
const IHandle& IHandle::operator= (const IHandle& other) {
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
//Purpose     : Changing value of ptr
//Parameters  : pValue: New value where handle shall be pointed to
/*--------------------------------------------------------------------------*/
void IHandle::setValue (void* pValue) {
   if (isDefined ())
      unlink ();
   else
      AttributValue::define ();

   create (pValue);
}

/*--------------------------------------------------------------------------*/
//Purpose     : Undefines the object
/*--------------------------------------------------------------------------*/
void IHandle::undefine () {
   if (isDefined ()) {
      unlink ();
      assignValue (0);
      AttributValue::undefine ();
   }
}

/*--------------------------------------------------------------------------*/
//Purpose     : Defines the object
/*--------------------------------------------------------------------------*/
void IHandle::define () {
   AttributValue::define ();
   create (NULL);
}
