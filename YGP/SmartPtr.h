#ifndef SMARTPTR_H
#define SMARTPTR_H

// $Id: SmartPtr.h,v 1.9 2003/11/14 20:27:55 markus Rel $

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


namespace YGP {

/**Smart-pointer-class; its referenced object is freed when the smart
   pointer-object is released.

   In counterpart to auto_ptr there is no ownership handling (and
   therefore no copy-constructor nor an assignment-operator).

   The macro <tt>definePtr(class)</tt> can be used to create a type for
   the smart pointer. It is defined as

   \verbatim typedef SmartPtr<class> P##class;
   \endverbatim

   The macro <tt>defineConstPtr(class)</tt> is the const version of the macro
   above:

   \verbatim typedef SmartPtr<const class> PC##class;
   \endverbatim

   <tt>definePtr(string)</tt> would create a <tt>typedef SmartPtr<std::string>
   Pstring</tt>.
   
   \note This class is (intented to be) simple. Consider this when using it!
*/
template <class T> class SmartPtr {
 public:
   // Manager functions
   /// Constructor; creates a smart pointer managing the passed pointer
   SmartPtr (T* pValue = NULL) : pData (pValue) { }
   /// Destructor; frees also the managed pointer
   ~SmartPtr () { delete pData; }

   /// \name Accessing
   //@{
   /// Dereferencing the object
   T* operator-> () const { return pData; }
   /// Dereferencing the object
   T& operator*  () const { return *pData; }
   /// Casting to the underlaying type
   operator T*   () const { return pData; }
   //@}

   /// Changing the managed pointer; the old managed object is freed
   SmartPtr& operator= (T* pValue) { delete pData; pData = pValue; return *this;}

 private:
   // Prohibited manager functions
   SmartPtr (const SmartPtr& other);
   // Two smart-pointers pointing to the same object causes chaos -> prohibit
   const SmartPtr& operator= (const SmartPtr& other);

   T* pData;
};

#define definePtr(class)      typedef YGP::SmartPtr<class> P##class
#define defineConstPtr(class) typedef YGP::SmartPtr<const class> PC##class

}

#endif
