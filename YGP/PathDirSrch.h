#ifndef YGP_PATHDIRSRCH_H
#define YGP_PATHDIRSRCH_H

//$Id: PathDirSrch.h,v 1.17 2008/05/18 13:21:27 markus Rel $

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


#include <string>

#include <YGP/DirSrch.h>
#include <YGP/PathSrch.h>


namespace YGP {

/**Class to search for files in directories listed in a path. This
   search can be restricted to files matching certain name-criterias
   or by attributes.

   The name-part of the files to search supports UNIX-like wildcards;
   that are the asterisk (*) for any number of any characters, the
   question-mark for any single character and a set of characters in
   brackets (([) and (])). This set can contain a list of characters
   (like [abcde]) or a region (like [a-e]). To invert this set use a
   leading caret (^) or a leading exclamation mark (!), like ([^a-e]).

   The found (and matching) files are retrieved by objects of type File.

   Note: The class does not do any word expansion for the search-path
         (like expanding the tilde (~) to the home-directory)!
*/
class PathDirectorySearch : public DirectorySearch {
 public:
   /// \name Section manager-functions
   //@{
   /// Default constructur; creates an empty object, not ready to search for files
   PathDirectorySearch () : DirectorySearch (), srch (), searchPath (std::string ()) { }
   /// Constructur; creates an object with an path to files in.
   /// \param path Path to search files in
   PathDirectorySearch (const std::string& path) : DirectorySearch (),
	 srch (), searchPath (path) { }
   /// Constructur; creates an object with an path to files in and the files
   /// to search for.
   /// \param path Path to search files in
   /// \param search Specification of files to search in path
   PathDirectorySearch (const std::string& path, const std::string& search)
      : DirectorySearch (), srch (search), searchPath (path) { }
   virtual ~PathDirectorySearch ();
   //@}

   /// Sets/Changes the path to search in.
   void setPath (const std::string& path) { searchPath = path; }

   /// \name Searching
   //@{
   /// Searches for the specified files with the passed attributes in all the
   /// nodes specified by \c path.
   /// \returns const File* Pointer to found file or NULL
   const File* find (const std::string& path, const std::string& search,
	             unsigned long attribs = FILE_NORMAL) {
      setPath (path);
      srch = search;
      return find (attribs); }
   const File* find (const std::string& search,
                     unsigned long attribs = FILE_NORMAL) {
      srch = search;
      searchPath.reset ();
      return find (attribs); }

   virtual const File* find (unsigned long attribs = FILE_NORMAL);
   virtual const File* next ();
   //@}

 protected:
   virtual int checkIntegrity () const;

   enum { NO_PATH = DirectorySearch::LAST, LAST };

 private:
   std::string srch;
   PathSearch  searchPath;
};

}

#endif
