#ifndef YGP_SORTDIRSRCH_H
#define YGP_SORTDIRSRCH_H

//$Id: SortDirSrch.h,v 1.6 2008/06/11 17:53:40 markus Rel $

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


#include <cstring>

#include <vector>
#include <algorithm>

#include <YGP/DirSrch.h>


namespace YGP {

// Foreward declarations
class File;


/**Class to search for files in a certain directory. This search can
   be restricted to files matching certain name-criterias or by
   attributes.

   The name-part of the files to search supports UNIX-like wildcards; that are
   the asterisk (*) for any number of any characters, the question-mark for
   any single character and a set of characters in brackets (([) and
   (])). This set can contain a list of characters (like [abcde]) or a region
   (like [a-e]). To invert this set use a leading caret (^) or a leading
   exclamation mark (!), like ([^a-e]).

   The found (and matching) files are retrieved by objects of type File.

   The result of the search is sorted alphabetically (by default; this can be
   changed to either size or last-access time).
*/
template <class Parent = YGP::DirectorySearch>
class SortedDirSearch : public Parent {
 public:
   SortedDirSearch () : Parent (), inFirstFind (false) { }
   /// Constructor from a search-string
   /// \param search: Files to search for
   SortedDirSearch (const std::string& search)
      : Parent (search), inFirstFind (false) { }
   virtual ~SortedDirSearch () {
      for (std::vector<File*>::iterator i (result.begin ()); i != result.end (); ++i)
         delete *i;
   }

   /// \name Searching
   //@{
   /// Returns the first found file matching \c spec, having the attributes \c attribs
   /// \param spec: Files to search for
   /// \param attribs: Attributes the searched files must have
   /// attribs
   virtual const File* find (const std::string& spec, unsigned long attribs = IDirectorySearch::FILE_NORMAL) {
      Parent::setSearchValue (spec);
      return find (attribs); }
   /// Searches for previously specified files with the passed attributes.
   /// \param attribs: Attributes the searched files must have
   /// \returns <tt>const File*</tt>: Pointer to found file or NULL
   virtual const File* find (unsigned long attribs = IDirectorySearch::FILE_NORMAL) {
      inFirstFind = true;
      const File* file (Parent::find (attribs));
      inFirstFind = false;
      while (file) {
	 result.push_back (new YGP::File (*file));
	 file = Parent::next ();
      } // endwhile

      std::sort (result.begin (), result.end (), &SortedDirSearch::compareFiles);
      return result.size () ? *result.begin () : NULL;
   }
   /// Method to find the next file matching the  previously specified values.
   /// \returns <tt>const File*</tt>: Pointer to found file or NULL
   virtual const File* next () {
      if (inFirstFind)
         return Parent::next ();

      if (result.size ()) {
	 delete *result.begin ();
	 result.erase (result.begin ());
      }
      return result.size () ? *result.begin () : NULL;
   }
   //@}

 protected:
   /// Method to find the next file matching the  previously specified values.
   /// \returns <tt>const File*</tt>: Pointer to found file or NULL
   static bool compareFiles (const File* a, const File* b) {
      return strcmp (a->name (), b->name ()) < 0;
   }

 private:
   SortedDirSearch (const SortedDirSearch& other);
   const SortedDirSearch& operator= (const SortedDirSearch& other);

   bool inFirstFind;
   std::vector<File*> result;
};

}

#endif
