#ifndef SORTDIRSRCH_H
#define SORTDIRSRCH_H

//$Id: SortDirSrch.h,v 1.1 2005/03/06 03:38:13 markus Exp $

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


#include <string.h>

#include <vector>

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
   typedef enum { SORT_NAME, SORT_SIZE, SORT_TIME } Sort;

   SortedDirSearch (Sort sort = SORT_NAME) : Parent (), inFirstFind (false), sort (sort) { }
   SortedDirSearch (const std::string& search, Sort sort = SORT_NAME)
      : Parent (search), inFirstFind (false), sort (sort)  { }
   virtual ~SortedDirSearch () { }

   /// \name Searching
   //@{
   /// Searches for previously specified files with the passed attributes.
   /// \param attribs: Attributes the searched files must have
   /// \returns <tt>const File*</tt>: Pointer to found file or NULL
   virtual const File* find (unsigned long attribs = IDirectorySearch::FILE_NORMAL) {
      inFirstFind = true;
      const File* file (Parent::find (attribs));
      inFirstFind = false;
      while (file = Parent::next ()) {
	 std::cout << "Founding: " << file->name () << '\n';
	 result.push_back (new YGP::File (*file));
      } // endwhile
      std::cout << "Files: " << result.size () << '\n';

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
   Sort sort;
   std::vector<File*> result;
};

}

#endif
