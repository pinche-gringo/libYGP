#ifndef PATHDIRSRCH_H
#define PATHDIRSRCH_H

//$Id: PathDirSrch.h,v 1.8 2001/09/27 22:02:25 markus Exp $

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


#include <assert.h>

#include <string>

#include <DirSrch.h>
#include <PathSrch.h>

// Class to search for files in directories listed in a path. This
// search can be restricted to files matching certain name-criterias
// or by attributes.
//
// The name-part of the files to search supports UNIX-like wildcards;
// that are the asterisk (*) for any number of any characters, the
// question-mark for any single character and a set of characters in
// brackets (([) and (])). This set can contain a list of characters
// (like [abcde]) or a region (like [a-e]). To invert this set use a
// leading caret (^) or a leading exclamation mark (!), like ([^a-e]).
//
// The found (and matching) files are retrieved by objects of type dirEntry.
//
// Note: The class does not do any word expansion for the search-path
//       (like expanding the tilde (~) to the home-directory)!
class PathDirectorySearch : public DirectorySearch {
 public:
   //@Section manager-functions
   PathDirectorySearch () : DirectorySearch (), searchPath ("") { }
   PathDirectorySearch (const std::string& path) : DirectorySearch ()
      , searchPath (path) { }
   PathDirectorySearch (const std::string& path, const std::string& search)
      : DirectorySearch (), srch (search), searchPath (path) { }
   virtual ~PathDirectorySearch ();

   void setPath (const std::string& path) { searchPath = path; }
   static void checkPath (const std::string& path) throw (std::string);

   //@Section searching
   int find (const std::string& path, const std::string& search,
	     dirEntry& result, unsigned long attribs = FILE_NORMAL) {
      setPath (path);
      srch = search;
      return find (result, attribs); }
   int find (const std::string& search, dirEntry& result,
	     unsigned long attribs = FILE_NORMAL) {
      srch = search;
      searchPath.reset ();
      return find (result, attribs); }

   virtual int find (dirEntry& result, unsigned long attribs = FILE_NORMAL);
   virtual int find ();

 protected:
   virtual int checkIntegrity () const;

   enum { NO_PATH = DirectorySearch::LAST, LAST };

 private:

   bool makePath (std::string& path, const std::string& file);

   std::string srch;
   PathSearch  searchPath;
};

#endif
