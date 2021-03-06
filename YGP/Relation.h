#ifndef YGP_RELATION_H
#define YGP_RELATION_H

//$Id: Relation.h,v 1.17 2008/06/11 17:53:40 markus Rel $

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include <YGP/Check.h>


namespace YGP {


// Forward declarations
class IRelation;


/**Class to manage the created relations.
 */
class RelationManager {
   friend class IRelation;

 public:
   static IRelation*  getRelation (const char* name);
   static const char* getRelationName (const IRelation& relation);

 private:
   static void remove (const IRelation* relation);
   static void add (const char* name, IRelation* relation) {
      Check1 (relations.find (name) == relations.end ());
      relations[name] = relation; }

   RelationManager ();
   RelationManager (const RelationManager& other);
   const RelationManager& operator= (const RelationManager& other);
   ~RelationManager ();

   static std::map<const char*, IRelation*> relations;
};


/**Functor for comparing boost::shared_ptrs.
 */
template <class _Tp>
struct lessDereferenced : public std::binary_function<_Tp, _Tp, bool> {
   /// Access to the functor
   /// \param __x First object to compare
   /// \param __y Second object to compare
   /// \returns bool True, if the address of the  object __x points to is
   ///     smaller than the address the second objects points to.
   bool operator() (const _Tp& __x, const _Tp& __y) const {
      return __x.get () < __y.get (); }
};


/**Class connecting (relating) objects of two classes (which must be derived
   from boost::shared_ptr).
 */
class IRelation {
 public:
   IRelation (const char* name);
   virtual ~IRelation ();

   /// Returns the name of the relation
   /// \returns const char* Name of the relation
   const char* name () const {
      return RelationManager::getRelationName (*this); }

 private:
   IRelation (const IRelation& other);
   const IRelation& operator= (const IRelation& other);
};


/**1-to-1 relation.

   Each object is connected to exactly one other (if at all).
 */
template <class S, class T>
class Relation1_1 : public IRelation {
 public:
   Relation1_1 (const char* name) : IRelation (name) { } ///< Defaultconstructor
   virtual ~Relation1_1 () { }                                  ///< Destructor

   /// Connects two objects
   /// \param source Parent to relate
   /// \param target Child to relate with parent
   /// \throws std::overflow_error If cardinality would be invalidated
   void relate (const S& source, const T& target) throw (std::overflow_error) {
      Check1 (source); Check1 (target);
      typename std::map<S, T >::const_iterator i (objects.find (source));
      if (i == objects.end ()) {
#if defined (CHECK) && (CHECK > 0)
	 for (typename std::map<S, T >::const_iterator i (objects.begin ());
	      i != objects.end (); ++i)
	    Check (i->second != target);
#endif
	 objects[source] = target;
      }
      else
	 throw std::overflow_error (name ());
   }
   /// Disconnects two objects
   /// \param source Parent to unrelate
   /// \param target Child to unrelate from parent
   void unrelate (const S& source, const T& target) {
      Check1 (source); Check1 (target);
      Check1 (objects.find (source) != objects.end ());
      objects.erase (objects.find (source));
   }

   /// Disconnect all objects
   void unrelateAll () {
      objects.erase (objects.begin (), objects.end ());
   }

   /// Checks if the passed object is related to any other (within this relation)
   /// \param owner Parent which should be checked for relations
   bool isRelated (const S& owner) const {
      Check1 (owner);
      return objects.find (owner) != objects.end ();
   }
   /// Checks if the passed object is related to any other (within this relation)
   /// \param object Child which should be checked for being related
   bool isRelated (const T& object) const {
      Check1 (object);
      for (typename std::map<S, T >::const_iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 if (i->second == object)
	    return true;
      return false;
   }
   /// Checks if the passed objects are related to each other (within this relation)
   /// \param owner Parent which should be checked for being related
   /// \param object Child which should be checked for relations
   bool isRelated (const S& owner, const T& object) const {
      Check1 (owner); Check1 (object);
      typename std::map<S, T >::const_iterator i (objects.find (owner));
      return ((i != objects.end ()) ? (i->second == object) : false);
   }

   /// Returns the child which is related with the passed object
   /// \param owner Parent whose object should be returned
   /// \returns T The related object
   /// \remarks The object must be related
   T& getObject (const S& owner) {
      Check1 (owner);
      Check1 (objects.find (owner));
      return objects[owner];
   }
   /// Returns the child which is related with the passed object
   /// \param owner Parent whose object should be returned
   /// \returns const T The related object
   /// \remarks The object must be related
   const T& getObject (const S& owner) const {
      Check1 (owner);
      Check1 (objects.find (owner));
      return *objects.find (owner);
   }
   /// Returns the parent of the passed object
   /// \param object Child whose parent should be returned
   /// \returns S The related parent
   /// \throws std::invalid_argument If the passed object is not related
   S& getParent (const T& object) throw (std::invalid_argument) {
      Check1 (object);
      for (typename std::map<S, T >::iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 if (i->second == object)
	    return i->first;
      throw std::invalid_argument ("Relation1_1::getParent");
   }
   /// Returns the parent of the passed object
   /// \param object Child whose parent should be returned
   /// \returns S The related parent
   /// \throws std::invalid_argument If the passed object is not related
   const S& getParent (const T& object) const throw (std::invalid_argument) {
      Check1 (object);
      for (typename std::map<S, T >::const_iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 if (i->second == object)
	    return i->first;
      throw std::invalid_argument ("Relation1_1::getParent");
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
   /// \param name Name of relation
   Relation1_N (const char* name) : IRelation (name) { }        ///< Defaultctr
   virtual ~Relation1_N () { }                                  ///< Destructor

   /// Connects two objects
   /// \param source Parent to relate
   /// \param target Child to relate with parent
   void relate (const S& source, const T& target) {
      Check1 (source); Check1 (target);
#if defined (CHECK) && (CHECK > 0)
      for (typename std::map<S, std::vector<T> >::const_iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 Check (find (i->second.begin (), i->second.end (), target)
		== i->second.end ());
#endif
      objects[source].push_back (target);
   }
   /// Connects an object with a bunch of objects, replacing the previously
   /// related objects.
   /// \param source Parent to relate
   /// \param targets Children to relate with parent
   void relate (const S& source, const std::vector<T>& targets) {
      Check1 (source); Check1 (targets.size ());
#if defined (CHECK) && (CHECK > 0)
      for (typename std::map<S, std::vector<T> >::const_iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 for (typename std::vector<T>::const_iterator t (targets.begin ());
	      t != targets.end (); ++t)
	    Check (find (i->second.begin (), i->second.end (), *t)
		   == i->second.end ());
#endif
      objects[source] = targets;
   }

   /// Disconnects two objects
   /// \param source Parent to unrelate
   /// \param target Child to unrelate from parent
   void unrelate (const S& source, const T& target) {
      Check1 (source); Check1 (target);
      typename std::map<S, std::vector<T> >::iterator i (objects.find (source));
      Check1 (i != objects.end ());

      typename std::vector<T>::iterator j
	 (find (i->second.begin (), i->second.end (), target));
      Check1 (j != i->second.end ());
      i->second.erase (j);
      if (i->second.empty ())
	 objects.erase (i);
   }

   /// Disconnect all objects from a parent
   /// \param source Parent to unrelate
   void unrelateAll (const S& source) {
      Check1 (source);
      typename std::map<S, std::vector<T> >::iterator i (objects.find (source));
      Check1 (i != objects.end ());
      objects.erase (i);
   }

   /// Disconnect all objects
   void unrelateAll () {
      objects.erase (objects.begin (), objects.end ());
   }

   /// Checks if the passed object is related to any other (within this relation)
   /// \param owner Parent which should be checked for relations
   bool isRelated (const S& owner) const {
      Check1 (owner);
      return objects.find (owner) != objects.end ();
   }
   /// Checks if the passed object is related to any other (within this relation)
   /// \param object Child which should be checked for being related
   bool isRelated (const T& object) const {
      Check1 (object);
      for (typename std::map<S, std::vector<T> >::const_iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 if (find (i->second.begin (), i->second.end (), object)
	     != i->second.end ())
	    return true;
      return false;
   }
   /// Checks if the passed object is related to any other (within this relation)
   /// \param owner Parent which should be checked for relations
   /// \param object Child which should be checked for being related
   bool isRelated (const S& owner, const T& object) const {
      Check1 (owner); Check1 (object);
      typename std::map<S, std::vector<T> >::const_iterator i (objects.find (owner));
      return ((i != objects.end ())
	      ? (find (i->second.begin (), i->second.end (), object)
		 != i->second.end ())
	      : false);
   }

   /// Returns the childs which are related with the passed object
   /// \param owner Parent whose object should be returned
   /// \returns T The related object
   /// \remarks The object must be related
   const std::vector<T>& getObjects (const S& owner) const {
      Check1 (owner);
      Check1 (objects.find (owner) != objects.end ());
      return objects.find (owner)->second;
   }
   /// Returns the parent of the passed object
   /// \param object Child whose parent should be returned
   /// \returns S The related parent
   /// \throws std::invalid_argument If the passed object is not related
   S getParent (const T& object) throw (std::invalid_argument) {
      Check1 (object);
      for (typename std::map<S, std::vector<T> >::const_iterator i (objects.begin ());
	   i != objects.end (); ++i) {
	 typename std::vector<T>::const_iterator o (find (i->second.begin (),
							  i->second.end (), object));
	 if (o != i->second.end ())
	    return i->first;
      }
      throw std::invalid_argument ("Relation1_N::getParent");
   }
   /// Returns the parent of the passed object
   /// \param object Child whose parent should be returned
   /// \returns S The related parent
   /// \throws std::invalid_argument If the passed object is not related
   const S getParent (const T& object) const throw (std::invalid_argument) {
      Check1 (object);
      for (typename std::map<S, std::vector<T> >::const_iterator i (objects.begin ());
	   i != objects.end (); ++i) {
	 typename std::vector<T>::const_iterator o (find (i->second.begin (),
							  i->second.end (), object));
	 if (o != i->second.end ())
	    return i->first;
      }
      throw std::invalid_argument ("Relation1_N::getParent");
   }

 protected:
   /// Objects withing the relation; mapping parent objects to a list
   /// of child objects
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
   /// \param name Name of relation
   /// \param maxRelated The number of elements which can be related to the object
   Relation1_X (const char* name, unsigned int maxRelated)
      : Relation1_N<S, T> (name), cRelated (maxRelated) { }
   virtual ~Relation1_X () { }                                  ///< Destructor

   /// Connects two objects
   /// \param source Parent to relate
   /// \param target Child to relate with parent
   /// \throws std::overflow_error If cardinality would be invalidated
   void relate (S source, T target) throw (std::overflow_error) {
      Check1 (source); Check1 (target);
      typename std::map<S, std::vector<T> >::iterator i
	 (Relation1_N<S, T>::objects.find (source));
      if (i != Relation1_N<S, T>::objects.end ()) {
	 if ((cRelated != -1U) && (i->second.size () >= cRelated))
	    throw std::overflow_error (IRelation::name ());
      }
      Relation1_N<S, T>::relate (source, target);
   }
   /// Connects an object with a bunch of objects, replacing the previously
   /// related objects.
   /// \param source Parent to relate
   /// \param targets Children to relate with parent
   /// \throws std::overflow_error If cardinality would be invalidated
   void relate (const S& source, const std::vector<T>& targets) throw (std::overflow_error) {
      Check1 (source); Check1 (targets.size ());
      if (targets.size () >= cRelated)
	 throw std::overflow_error (IRelation::name ());

      Relation1_N<S, T>::relate (source, targets);
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
 public:
   /// Creates an 1-to-n relation.
   /// \param name Name of relation
   RelationN_M (const char* name) : IRelation (name) { }        ///< Defaultctr
   virtual ~RelationN_M () { }                                  ///< Destructor

   /// Connects two objects
   /// \param source Parent to relate
   /// \param target Child to relate with parent
   void relate (const S& source, const T& target) {
      Check1 (source); Check1 (target);
      objects[source].push_back (target);
      parents[target].push_back (source);
   }
   /// Connects an object with a bunch of objects, replacing the previously
   /// related objects.
   /// \param source Parent to relate
   /// \param targets Children to relate with parent
   void relate (const S& source, const std::vector<T>& targets) {
      Check1 (source); Check1 (targets.size ());

      // Unrelate old entries (remove from parents)
      typename std::map<S, std::vector<T> >::iterator i (objects.find (source));
      if (i != objects.end ()) {
	 for (typename std::vector<T>::iterator j (i->second.begin ()); j != i->second.end (); ++j) {
	    Check1 (parents.find (*j) != parents.end ());
	    typename std::map<T, std::vector<S> >::iterator o (parents.find (*j));
	    Check1 (std::find (o->second.begin (), o->second.end (), source) != o->second.end ());
	    o->second.erase (std::find (o->second.begin (), o->second.end (), source));

	    if (o->second.empty ())
	       parents.erase (o);
	 }
	 i->second = targets;
      }
      else
	 objects.insert (std::pair<S, std::vector<T> > (source, targets));

      // Relate new entries (add to parents)
      for (typename std::vector<T>::const_iterator t (targets.begin ()); t != targets.end (); ++t)
	 parents[*t].push_back (source);
   }
   /// Connects an object with a bunch of objects, replacing the previously
   /// related objects.
   /// \param sources Parents to relate
   /// \param target Child to relate with parents
   void relate (const std::vector<S>& sources, const T& target) {
      Check1 (sources.size ()); Check1 (target);

      // Unrelate old entries (remove from objects)
      typename std::map<T, std::vector<S> >::iterator i (parents.find (target));
      if (i != parents.end ()) {
	 for (typename std::vector<S>::iterator j (i->second.begin ()); j != i->second.end (); ++j) {
	    Check1 (objects.find (*j) != objects.end ());
	    typename std::map<S, std::vector<T> >::iterator o (objects.find (*j));
	    Check1 (std::find (o->second.begin (), o->second.end (), target) != o->second.end ());
	    o->second.erase (std::find (o->second.begin (), o->second.end (), target));

	    if (o->second.empty ())
	       objects.erase (o);
	 }
	 i->second = sources;
      }
      else
	 parents.insert (std::pair<T, std::vector<S> > (target, sources));

      // Relate new entries (add to parents)
      for (typename std::vector<S>::const_iterator s (sources.begin ()); s != sources.end (); ++s)
	 objects[*s].push_back (target);
   }

   /// Disconnects two objects
   /// \param source Parent to unrelate
   /// \param target Child to unrelate from parent
   void unrelate (const S& source, const T& target) {
      Check1 (source); Check1 (target);
      typename std::map<S, std::vector<T> >::iterator i (objects.find (source));
      Check1 (i != objects.end ());

      typename std::vector<T>::iterator j (find (i->second.begin (), i->second.end (), target));
      Check1 (j != i->second.end ());
      i->second.erase (j);
      if (i->second.empty ())
	 objects.erase (i);

      typename std::map<T, std::vector<S> >::iterator k (parents.find (target));
      Check1 (k != parents.end ());

      typename std::vector<S>::iterator l (find (k->second.begin (), k->second.end (), source));
      Check1 (l != k->second.end ());
      k->second.erase (l);
      if (k->second.empty ())
	 parents.erase (k);
   }

   /// Disconnect all objects from a parent
   /// \param source Parent to unrelate
   void unrelateAll (const S& source) {
      Check1 (source);
      typename std::map<S, std::vector<T> >::iterator i (objects.find (source));
      Check1 (i != objects.end ());

      for (typename std::vector<T>::iterator j (i->second.begin ()); j != i->second.end (); ++j) {
	 Check1 (parents.find (*j) != parents.end ());
	 typename std::map<T, std::vector<S> >::iterator o (parents.find (*j));
	 Check1 (std::find (o->second.begin (), o->second.end (), source) != o->second.end ());
	 o->second.erase (std::find (o->second.begin (), o->second.end (), source));

	 if (o->second.empty ())
	    parents.erase (o);
      }
      objects.erase (i);
   }

   /// Disconnect all parents from an object
   /// \param target Parent to unrelate
   void unrelateAll (const T& target) {
      Check1 (target);
      typename std::map<T, std::vector<S> >::iterator i (parents.find (target));
      Check1 (i != parents.end ());

      for (typename std::vector<S>::iterator j (i->second.begin ()); j != i->second.end (); ++j) {
	 Check1 (objects.find (*j) != objects.end ());
	 Check1 (find (objects[*j].begin (), objects[*j].end (), target) != objects[*j].end ());
	 objects[*j].erase (find (objects[*j].begin (), objects[*j].end (), target));
      }
      parents.erase (i);
   }

   /// Disconnect all objects
   void unrelateAll () {
      objects.erase (objects.begin (), objects.end ());
      parents.erase (parents.begin (), parents.end ());
   }

   /// Checks if the passed object is related to any other (within this relation)
   /// \param owner Parent which should be checked for relations
   bool isRelated (const S& owner) const {
      Check1 (owner);
      return objects.find (owner) != objects.end ();
   }
   /// Checks if the passed object is related to any other (within this relation)
   /// \param object Child which should be checked for being related
   bool isRelated (const T& object) const {
      Check1 (object);
      return parents.find (object) != parents.end ();
   }
   /// Checks if the passed object is related to any other (within this relation)
   /// \param owner Parent which should be checked for relations
   /// \param object Child which should be checked for being related
   bool isRelated (const S& owner, const T& object) const {
      Check1 (owner); Check1 (object);
      typename std::map<S, std::vector<T> >::const_iterator i (objects.find (owner));
      return ((i != objects.end ())
	      ? (find (i->second.begin (), i->second.end (), object)
		 != i->second.end ())
	      : false);
   }

   /// Returns the childs which are related with the passed object
   /// \param owner Parent whose object should be returned
   /// \returns T The related object
   /// \remarks The object must be related
   const std::vector<T>& getObjects (const S& owner) const {
      Check1 (owner);
      typename std::map<S, std::vector<T> >::const_iterator i (objects.find (owner));
      Check1 (i != objects.end ());
      return i->second;
   }
   /// Returns the parents of the passed object
   /// \param object Child whose parent should be returned
   /// \returns S The related parent
   /// \throws std::invalid_argument If the passed object is not related
   const std::vector<S>& getParents (const T& object) const throw (std::invalid_argument) {
      Check1 (object);
      for (typename std::map<S, std::vector<T> >::const_iterator i (objects.begin ());
	   i != objects.end (); ++i) {
	 typename std::vector<T>::const_iterator o (find (i->second.begin (),
							  i->second.end (), object));
	 if (o != i->second.end ()) {
	    typename std::map<T, std::vector<S> >::const_iterator p
	       (parents.find (*o));
	    Check3 (p != parents.end ());
	    return p->second;
	 }
      }
      throw std::invalid_argument ("RelationN_M::getParent");
   }

 protected:
   /// Objects withing the relation; mapping parent objects to a list
   /// of child objects
   std::map<S, std::vector<T>, lessDereferenced<S> > objects;
   /// Parents withing the relation; mapping child objects to a list
   /// of parent objects
   std::map<T, std::vector<S>, lessDereferenced<T> > parents;

 private:
   RelationN_M (const RelationN_M& other);
   const RelationN_M& operator= (const RelationN_M& other);
};


}

#endif
