#ifndef RELATION_H
#define RELATION_H

//$Id: Relation.h,v 1.1 2004/10/23 06:43:09 markus Exp $

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

   bool isRelated (S source, T target) {
      return ((objects.find (source) != objects.end ())
	      ? (objects[source] == target) : false);
   }

 private:
   Relation1_1 (const Relation1_1& other);
   const Relation1_1& operator= (const Relation1_1& other);

   std::map<S, T> objects;
};


/**1-to-N relation.

   Each object can be connected to various others
 */
template <class S, class T>
class Relation1_N : public IRelation {
 public:
   /// Creates an 1-to-n relation.
   /// \param maxRelated: The number of elements which can be related to the object
   Relation1_N (unsigned int maxRelated) : cRelated (maxRelated){ }
   virtual ~Relation1_N () { }                                  ///< Destructor

   void relate (S source, T target) {
      Check1 (objects.find (source) == objects.end ());
#if defined (CHECK) && (CHECK > 0)
      for (typename std::map<S, std::vector<T> >::iterator i (objects.begin ());
	   i != objects.end (); ++i)
	 Check (find (i->second.begin (), i->second.end (), target)
		== i->second.end ());
#endif
      objects[source].push_back (target);
   }

   bool isRelated (S source, T target) {
      return ((objects.find (source) != objects.end ())
	      ? (find (objects[source].begin (), objects[source].end (), target)
		 != objects[source].end ())
	      : false);
   }

 private:
   Relation1_N (const Relation1_N& other);
   const Relation1_N& operator= (const Relation1_N& other);

   std::map<S, std::vector<T> > objects;
   unsigned int                 cRelated;
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
