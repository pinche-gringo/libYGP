#ifndef SMARTPTR_H
#define SMARTPTR_H

// $Id: SmartPtr.h,v 1.1 1999/08/29 20:53:00 Markus Exp $

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

#include <iostream.h>


// Smart-pointer-class; the memory is freed within the destructor
// It contains (different to auto_ptr no copy-constructur and assignment-
// operator to avoid the problems with ownership. If you need two pointers
// pointing to the same area, use the operator T*-method (or auto_ptr).
// Note: SmartPtr is no AttributValue to keep its overhead minimized
template <class T> class SmartPtr {
 public:
   // Manager functions
   SmartPtr (T* pValue = NULL) : pData (pValue) { }
   ~SmartPtr () { delete pData; }

   // Accessing
   T* operator-> () const { return pData; }
   T* operator*  () const { return pData; }
   operator T*   () const { return pData; }

 private:
   // Prohibited manager functions
   SmartPtr (const SmartPtr& other);
   const SmartPtr& operator= (const SmartPtr& other);

   T* pData;
};

#define definePtr(class)     typedef SmartPtr<class> P##class

#endif
