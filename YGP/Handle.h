#ifndef HANDLE_H
#define HANDLE_H

// $Id: Handle.h,v 1.12 2004/12/29 18:21:57 markus Rel $

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

/**Smart-pointer-class with reference-counting; base-class for type-safe
   handles (defined with a template at end of file)

   The object is freed if there are no more references to it.

   As every attribute, handles can be undefined (equivalent to NULL pointers).
*/
class IHandle : public AttributValue {
 public:
   // Manager functions
   /// Default constructor; creates an undefined handle
   IHandle () : AttributValue () { }
   /// Copy constructor; let's this point to the same object than other.
   /// Increases the reference count of the object.
   IHandle (const IHandle& other) : AttributValue (other) { }
   virtual ~IHandle ();

   IHandle& operator= (const IHandle& other);

   virtual void undefine ();

 protected:
   // Interface to RefCount
   virtual void create (void* pValue) = 0; ///< Method to create the object.
   virtual void link () = 0;  ///< Method to link the handle to the object pointing to.
   virtual void unlink () = 0;  ///< Method to unlink the handle from the object pointing to.
   virtual void* getValue () const = 0;  ///< Method to access the object pointed to by the handle.
   void          setValue (void* pValue);  ///< Method to set the object pointed to by the handle. This method cares about the reference count (and unlinks the old and links the new object). 

   /// Method to set the object pointed to by the handle. This method does not
   /// care about the reference count; use with care (or don't use at all!).
   virtual void assignValue (void* pValue) = 0;

   // Interface of AttributValue
   virtual void define ();
};


template <class T> class Handle;

/** Object-pointer with reference-counter.

   The class gives the opportunity to link and unlink to and from an
   object. If no more object is linked, the object destroys itself and the
   referenced object.
*/
template <class T> class RefCount {
   friend class Handle<T>;

 public:
   // Manager-functions
   /// Default constructor; creates a reference counting object pointing
   /// the passed object. The count is set to 1.
   RefCount (T* pData = NULL) : pValue (pData), count (1) { }
   ~RefCount () { }                                             ///< Destructor

   // Connecting other references
   void link () { ++count; }                 ///< Increases the reference count
   /// Decreases the reference count; if the counter is zero, the stored object
   /// is freed
   void unlink () { Check1 (count); if (!--count) delete this; }

 private:
   // Prohibited manager functions:
   RefCount (const RefCount& other);
   RefCount& operator= (const RefCount& other);

   SmartPtr<T> pValue;                       ///< Pointer to the handled object
   unsigned int count;
};


/**Type-safe versions of IHandle (smart pointer with reference-counting)

   The object is freed if there are no more references to it.

   The macro <tt>defineHndl(class)</tt> can be used to create a type for
   the handle. It is defined as

   \verbatim typedef Handle<class> H##class;
   \endverbatim

   <tt>defineHndl(string)</tt> would create a <tt>typedef Handle<std::string>
   Hstring</tt>.

   As every attribute, handles can be undefined (equivalent to NULL pointers).
*/
template <class T> class Handle : public IHandle {
 public:
   /// Default constructor; creates an undefined handle of an object of type \c T
   Handle () : IHandle (), pData (NULL) { }
   /// Constructor which lets the handle point to the passed object. The
   /// handle is defined and the reference-count is set to 1.
   Handle (T* pValue) : IHandle (), pData (new RefCount<T> (pValue)) { setDefined (); }
   /// Copy constructor; let's this point to the same object than other.
   /// Increases the reference count of the object.
   Handle (const Handle& other) : IHandle (other), pData (other.pData) {
      if (isDefined ()) link ();
      Check2 (isDefined () ? pData != NULL : pData == NULL); }
   ~Handle () { if (isDefined ()) unlink (); }  ///< Destructor; unlinks object

   /// Assignment operator; both handles points to the same object and the
   /// reference-counter is increased. The count of the object referenced
   /// before is decreased.
   Handle& operator= (const Handle& other) {
      return (Handle&)IHandle::operator= (other); }

   /// Defines the handle and creates a new object pointed to (with
   /// reference-count 1). If an already defined handle is defined again, its
   /// old object is released (and freed if the reference-count becomes 0) and
   /// a new object is assigned.
   virtual void define () { IHandle::setValue (new T); }

   /// \name Accessing the object
   //@{
   /// Casting to a <tt>T*</tt>
   /// \returns T*: Pointer to the object
   T* ptr        () const { return pData ? (T*)pData->pValue : NULL; }
   T* operator-> () const { return pData->pValue; }  ///< Dereferencing the pointer
   /// T& operator*  () const { return *pData->pValue; } ///< Dereferencing the pointer
   operator T*   () const { return ptr (); }       ///< Casting to a <tt>T*</tt>
   //@}

   /// Cast to derived class.
   ///
   /// The Handle can't be cast with the usual notation so instead you can use
   /// @code
   ///   ptr_derived = Handle<Derived>::cast (ptr_base);
   /// @endcode
   ///
   template <class T_CastFrom>
   static Handle<T> cast (const Handle<T_CastFrom>& src) {
      Handle<T> h;
      if (src.isDefined ()) {
	 h.setDefined ();
	 Check3 (src.pData);
	 h.pData = (RefCount<T>*)(src.pData);
	 h.pData->link ();
      }
      return h;
   }

   /// \name Comparison
   //@{
   /// Comparison-operator; Checks if two handles are equal
   /// \param other: Handle to compare with
   /// \returns bool: True if objects are equal
   bool operator== (const Handle& other) const {
      return pData->pValue == other.pData->pValue; }
   /// Comparison-operator; Checks if two handles are equal
   /// \param other: Handle to compare with
   /// \returns bool: True if objects are equal
   bool operator!= (const Handle& other) const {
      return pData->pValue != other.pData->pValue; }
   //@}

 protected:
   /// \name Interface to RefCount
   //@{
   virtual void create (void* pValue) { pData = new RefCount<T> ((T*)pValue); }
   virtual void link () { Check2 (pData); pData->link (); }
   virtual void unlink () { Check2 (pData); pData->unlink (); }
   virtual void* getValue () const { return pData; }
   virtual void assignValue (void* pValue) { pData = (RefCount<T>*)pValue; }
   //@}

 public:
   RefCount<T>* pData; ///< Don't access! Should be private; but fails to compile
};

#define defineHndl(class)         typedef YGP::Handle<class> H##class

}

#endif
