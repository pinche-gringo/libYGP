#ifndef PATHDIRSRCH_H
#define PATHDIRSRCH_H

//$Id: PathDirSrch.h,v 1.3 1999/09/26 02:18:15 Markus Rel $

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

#include "DirSrch.h"
#include "PathSrch.h"


// Class to search for files in a specified path
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

   //@Section searching
   int find (const std::string& path, const std::string& search,
	     dirEntry& result, unsigned long attribs = 0) {
      setPath (path);
      srch = search;
      return find (result, attribs); }
   int find (const std::string& search, dirEntry& result,
	     unsigned long attribs = 0) {
      srch = search;
      searchPath.reset ();
      return find (result, attribs); }

   virtual int find (dirEntry& result, unsigned long attribs = 0);
   virtual int find ();

 protected:
   enum { NO_PATH = DirectorySearch::LAST, LAST };

 private:
   virtual int checkIntegrity () const;

   void makePath (std::string& path, std::string& file) {
      if (path[path.length () - 1] != DirectorySearch::DIR_SPLIT)
	 path += DirectorySearch::DIR_SPLIT;
      path += file; }

   std::string srch;
   PathSearch  searchPath;
};

#endif
