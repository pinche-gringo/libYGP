#ifndef HANDLE_H
#define HANDLE_H

// $Id: Handle.h,v 1.14 2006/04/23 02:59:45 markus Rel $

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
#include <YGP/AttrVal.h>
#include <YGP/SmartPtr.h>


namespace YGP {


template <class T> class Handle;

/** Object-pointer with reference-counter.

   The class gives the opportunity to link and unlink to and from an
   object. If no more object is linked, the object destroys itself and the
   referenced object.
*/
template <class T> class RefCount {
 public:
   // Manager-functions
   /// Default constructor; creates a reference counting object pointing
   /// the passed object. The count is set to 1.
   RefCount (T* pData = NULL) : pValue (pData), count (1) { }
   ~RefCount () { delete pValue;  }                             ///< Destructor

   // Connecting other references
   void link () { ++count; }                 ///< Increases the reference count
   /// Decreases the reference count; if the counter is zero, the stored object
   /// is freed
   void unlink () { Check1 (count); if (!--count) delete this; }

   T* getValue () const { return pValue; }


 private:
   // Prohibited manager functions:
   RefCount (const RefCount& other);
   RefCount& operator= (const RefCount& other);

   T* pValue;                                ///< Pointer to the handled object
   unsigned int count;
};


/**Type-safe versions of a smart pointer with reference-counting

   The object is freed if there are no more references to it.

   The macro <tt>defineHndl(class)</tt> can be used to create a type for
   the handle. It is defined as

   \verbatim typedef Handle<class> H##class;
   \endverbatim

   <tt>defineHndl(string)</tt> would create a <tt>typedef Handle<std::string>
   Hstring</tt>.

   Handles can be undefined (equivalent to NULL pointers).
*/
template <class T> class Handle {
 public:
   /// Default constructor; creates an undefined handle of an object of type \c T
   Handle () : pData (NULL) { }
   /// Constructor which lets the handle point to the passed object. The
   /// handle is defined and the reference-count is set to 1.
   Handle (T* pValue) : pData (new RefCount<T> (pValue)) { }
   /// Copy constructor; let's this point to the same object than other.
   /// Increases the reference count of the object.
   Handle (const Handle<T>& other) : pData (other.pData) {
      if (pData) pData->link (); }
   /// The templated contstructor allows copy construction from any object
   /// that's castable.  Thus, it does downcasts.
   template <class T_CastFrom>
   Handle (const Handle<T_CastFrom>& src) : pData (src.pData) {
      if (pData) pData->link (); }
   ~Handle () { if (pData) pData->unlink (); }  ///< Destructor; unlinks object


   /// Assignment operator; both handles points to the same object and the
   /// reference-counter is increased. The count of the object referenced
   /// before is decreased.
   Handle& operator= (const Handle& other) {
      if (pData != other.pData) {
	 if (pData) pData->unlink ();
	 pData = other.pData;
	 if (pData) pData->link (); }
      return *this;
   }
   /// The templated assignment operator allows copy construction from any
   /// object that's castable.  Thus, it does downcasts.
   template <class T_CastFrom>
   Handle& operator= (const Handle<T_CastFrom> other) {
      if (pData != other.pData) {
	 if (pData) pData->unlink ();
	 pData = other.pData;
	 if (pData) pData->link (); }
   }

   /// Defines the handle and creates a new object pointed to (with
   /// reference-count 1). If an already defined handle is defined again, its
   /// old object is released (and freed if the reference-count becomes 0) and
   /// a new object is assigned.
   void define () {
      if (pData) pData->unlink ();
      pData = new RefCount<T> (new T); }
   bool isDefined () const { return pData; }  ///< Checks if the object is defined
   /// Undefines the object
   void undefine () {
      if (pData) {
	 pData->unlink ();
	 pData = NULL;
      }
   }

   /// \name Accessing the object
   //@{
   /// Casting to a <tt>T*</tt>
   /// \returns T*: Pointer to the object
   T* ptr        () const { return pData ? pData->getValue () : NULL; }
   T* operator-> () const { return pData->getValue (); }    ///< Dereferencing
   operator T*   () const { return ptr (); }      ///< Casting to a <tt>T*</tt>
   RefCount<T>* getData () const { return pData; }  ///< Return RefCount-object
   //@}

   /// (Static) Cast to derived class.
   ///
   /// The Handle can't be cast with the usual notation so instead you can use
   /// @code
   ///   ptr_derived = Handle<Derived>::cast (ptr_base);
   /// @endcode
   ///
   template <class T_CastFrom>
   static Handle<T> cast (const Handle<T_CastFrom>& src) {
      Handle<T> res;
      T* ptr (static_cast<T*> (src.ptr ()));
      if (ptr) {
	 res.pData = (RefCount<T>*)(void*)src.getData ();
	 if (res.pData)
	    res.pData->link ();
      }
      return res;
   }

   /// Dynamic cast to derived class.
   ///
   /// The Handle can't be cast with the usual notation so instead you can use
   /// @code
   ///   ptr_derived = Handle<Derived>::castDynamic (ptr_base);
   /// @endcode
   ///
   template <class T_CastFrom>
   static Handle<T> castDynamic (const Handle<T_CastFrom>& src) {
      Handle<T> res;
      T* ptr (dynamic_cast<T*> (src.ptr ()));
      if (ptr) {
	 res.pData = (RefCount<T>*)(void*)(src.getData ());
	 if (res.pData)
	    res.pData->link ();
      }
      return res;
   }

   /// \name Comparison
   //@{
   /// Comparison-operator; Checks if two handles are equal
   /// \param other: Handle to compare with
   /// \returns bool: True if objects are equal
   bool operator== (const Handle& other) const {
      return ptr () == other.ptr (); }
   /// Comparison-operator; Checks if two handles are equal
   /// \param other: Handle to compare with
   /// \returns bool: True if objects are equal
   bool operator!= (const Handle& other) const {
      return pData->getValue () != other.pData->getValue (); }

   //@}

 private:
   RefCount<T>* pData;               ///< Pointer to (reference-counted) object
};

#define defineHndl(class)         typedef YGP::Handle<class> H##class

}

#endif
