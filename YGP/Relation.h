#ifndef RELATION_H
#define RELATION_H

//$Id: Relation.h,v 1.2 2004/10/23 18:53:19 markus Exp $

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


#include <map>
#include <vector>
#include <stdexcept>

#include <YGP/Check.h>
#include <YGP/Handle.h>


namespace YGP {


// Forward declarations
class IRelation;


/**Class to manage the created relations.
 */
class RelationManager {
   friend class IRelation;

 private:
   static void add (IRelation* relation) {
      Check1 (find (relations.begin (), relations.end (), relation)
	      == relations.end ());
      relations.push_back (relation); }

   RelationManager ();
   RelationManager (const RelationManager& other);
   const RelationManager& operator= (const RelationManager& other);
   ~RelationManager ();

   static void removeAll ();
   static void remove (const IRelation* relation);

   static std::vector<IRelation*> relations;
};


/**Functor for comparing handles (which actually compares the pointers the
   handle refers to).
 */
template <class _Tp>
struct lessDereferenced : public std::binary_function<_Tp, _Tp, bool> {
   bool operator() (const _Tp& __x, const _Tp& __y) const {return *__x < *__y; }
};


/**Class connecting (relating) object of two classes (which must be derived
   from YGP::IHandle.
 */
class IRelation {
 public:
   IRelation ();
   virtual ~IRelation ();

 private:
   IRelation (const IRelation& other);
   const IRelation& operator= (const IRelation& other);
};


/**1-to-1 relation.

   Each object is connected to exactly one other (if at all).
 */
template <class S, class T>
class Relation1_1 : public IRelation {
   Relation1_1 () { }                                   ///< Defaultconstructor
   virtual ~Relation1_1 () { }                                  ///< Destructor

   void relate (S source, T target) {
      Check1 (objects.find (source) == objects.end ());
#if defined (CHECK) && (CHECK > 0)
      for (typename std::map<S, T >::iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 Check (i->second != target);
#endif
      objects[source] = target;
   }

   bool isRelated (const S& owner, const T& object) {
      typename std::map<S, T >::iterator i (objects.find (owner));
      return ((i != objects.end ()) ? (*i == object) : false);
   }

   T& getObject (const S& owner) const {
      Check1 (objects.find (owner));
      return objects[owner];
   }
   S& getParent (const T& object) const {
      for (typename std::map<S, T >::iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 if (i->second == object)
	    return i->first;
      Check1 (0);
   }

 private:
   Relation1_1 (const Relation1_1& other);
   const Relation1_1& operator= (const Relation1_1& other);

   std::map<S, T, lessDereferenced<S> > objects;
};


/**1-to-N relation without a limit for N.

   Each object can be connected to various others.
 */
template <class S, class T>
class Relation1_N : public IRelation {
 public:
   /// Creates an 1-to-n relation.
   /// \param maxRelated: The number of elements which can be related to the object
   Relation1_N () { }
   virtual ~Relation1_N () { }                                  ///< Destructor

   void relate (S source, T target) {
#if defined (CHECK) && (CHECK > 0)
      for (typename std::map<S, std::vector<T> >::iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 Check (find (i->second.begin (), i->second.end (), target)
		== i->second.end ());
#endif
      objects[source].push_back (target);
   }

   bool isRelated (const S& owner, const T& object) {
      typename std::map<S, std::vector<T> >::iterator i (objects.find (owner));
      return ((i != objects.end ())
	      ? (find (i->second.begin (), i->second.end (), object)
		 != i->second.end ())
	      : false);
   }

   std::vector<T>& getObjects (const S& owner) {
      Check1 (objects.find (owner) != objects.end ());
      return objects[owner];
   }
   S getParent (const T& object) const {
      for (typename std::map<S, std::vector<T> >::const_iterator i (objects.begin ());
	   i != objects.end (); ++i) {
	 typename std::vector<T>::const_iterator o (find (i->second.begin (),
							  i->second.end (), object));
	 if (o != i->second.end ())
	    return i->first;
      }
      Check1 (0);
   }

 protected:
   std::map<S, std::vector<T>, lessDereferenced<S> > objects;

 private:
   Relation1_N (const Relation1_N& other);
   const Relation1_N& operator= (const Relation1_N& other);
};


/**1-to-N relation with a limited N.

   Each object can be connected to various others.
 */
template <class S, class T>
class Relation1_X : public Relation1_N<S, T> {
 public:
   /// Creates an 1-to-n relation.
   /// \param maxRelated: The number of elements which can be related to the object
   Relation1_X (unsigned int maxRelated) : cRelated (maxRelated){ }
   virtual ~Relation1_X () { }                                  ///< Destructor

   void relate (S source, T target) throw (std::overflow_error) {
      typename std::map<S, std::vector<T> >::iterator i
	 (Relation1_N<S, T>::objects.find (source));
      if (i != Relation1_N<S, T>::objects.end ()) {
	 std::cout << "Adding to " << i->second.size () << " objects\n";
	 if ((cRelated != -1U) && (i->second.size () >= cRelated))
	    throw std::overflow_error ("");
      }
      Relation1_N<S, T>::relate (source, target);
   }

 private:
   Relation1_X (const Relation1_X& other);
   const Relation1_X& operator= (const Relation1_X& other);

   unsigned int cRelated;
};


/**N-to-M relation.

   Each object can be connected to various others, which themself can be
   related to various of the first.
 */
template <class S, class T>
class RelationN_M : public IRelation {
   RelationN_M () { }                                   ///< Defaultconstructor
   virtual ~RelationN_M () { }                                  ///< Destructor

 private:
   RelationN_M (const RelationN_M& other);
   const RelationN_M& operator= (const RelationN_M& other);

   std::vector<S> objects;
   std::vector<T> related;
};


}

#endif
