#ifndef HANDLE_H
#define HANDLE_H

// $Id: Handle.h,v 1.5 2002/03/23 20:44:38 markus Rel $

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


#include <stdio.h>
#include <assert.h>

#include <AttrVal.h>
#include <SmartPtr.h>


// Smart-pointer-class with reference-counting; base-class for type-safe
// classes (defined with template at end of file)
class IHandle : public AttributValue {
 public:
   // Manager functions
   IHandle () : AttributValue () { }
   IHandle (const IHandle& other) : AttributValue (other) { }
   virtual ~IHandle ();

   IHandle& operator= (const IHandle& other);

   virtual void undefine ();

 protected:
   // Interface to RefCount
   virtual void create (void* pValue) = 0;
   virtual void link () = 0;
   virtual void unlink () = 0;
   virtual void* getValue () const = 0;
   void          setValue (void* pValue);
   virtual void assignValue (void* pValue) = 0;

   // Interface of AttributValue
   virtual void define ();
};


// Class for reference-couting
template <class T> class RefCount {
 public:
   // Manager-functions
   RefCount (T* pData = NULL) : count (1), pValue (pData) { }
   ~RefCount () { }

   // Connecting other references
   void link () { ++count; }
   void unlink () { assert (count); if (!--count) delete this; }

   SmartPtr<T> pValue;

 private:
   // Prohibited manager functions:
   RefCount (const RefCount& other);
   RefCount& operator= (const RefCount& other);

   unsigned int count;
};


// Type-safe versions of IHandle (smart pointer with reference-counting)
template <class T> class Handle : public IHandle {
 public:
   // Manager functions
   Handle () : IHandle (), pData (NULL) { }
   Handle (T* pValue) : IHandle (), pData (new RefCount<T> (pValue)) { setDefined (); }
   Handle (const Handle& other) : IHandle (other), pData (other.pData) { 
      if (isDefined ()) link ();
      assert (isDefined () ? pData != NULL : pData == NULL); }
   ~Handle () { if (isDefined ()) unlink (); }

   Handle& operator= (const Handle& other) {
      return (Handle&)IHandle::operator= (other); }

   virtual void define () { IHandle::setValue (new T); }

   // Accessing
   T* operator-> () const { return pData->pValue; }
   T* operator*  () const { return pData->pValue; }
   operator T*   () const { return pData->pValue; }

 protected:
   // Interface to RefCount
   virtual void create (void* pValue) { pData = new RefCount<T> ((T*)pValue); }
   virtual void link () { assert (pData); pData->link (); }
   virtual void unlink () { assert (pData); pData->unlink (); }
   virtual void* getValue () const { return pData; }
   virtual void assignValue (void* pValue) { pData = (RefCount<T>*)pValue; }
  
 private:
   RefCount<T>* pData;

};

#define defineHndl(class)         typedef Handle<class> H##class

#endif
