#ifndef SMARTPTR_H
#define SMARTPTR_H

// $Id: SmartPtr.h,v 1.6 2002/05/09 06:59:30 markus Rel $

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


// Smart-pointer-class; its referenced object is freed when the smart
// pointer-object is realeased.
//
// In counterpart to auto_ptr there is no ownership handling (and
// therefor no copy-constructor nor an assignment-operator).
// 
// Note: This class is (intented to be) simple. Consider this when using it!
template <class T> class SmartPtr {
 public:
   // Manager functions
   SmartPtr (T* pValue = NULL) : pData (pValue) { }
   ~SmartPtr () { delete pData; }

   // Accessing
   T* operator-> () const { return pData; }
   T& operator*  () const { return *pData; }
   operator T*   () const { return pData; }

   // Changing the value
   SmartPtr& operator= (T* pValue) { delete pData; pData = pValue; }

 private:
   // Prohibited manager functions
   SmartPtr (const SmartPtr& other);
   // Two smart-pointers pointing to the same object causes chaos -> prohibit
   const SmartPtr& operator= (const SmartPtr& other);

   T* pData;
};

#define definePtr(class)      typedef SmartPtr<class> P##class
#define defineConstPtr(class) typedef SmartPtr<const class> PC##class

#endif
