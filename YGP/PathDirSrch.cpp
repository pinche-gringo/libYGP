//$Id: PathDirSrch.cpp,v 1.20 2002/05/24 06:52:49 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : PathDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.20 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.9.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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

#include <errno.h>

#include "Internal.h"

#include "Trace_.h"
#include "PathDirSrch.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
PathDirectorySearch::~PathDirectorySearch () {
   cleanup ();
}


/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the first file which matches the search-criteria
//Parameters: pResult: Buffer where to place the result
//Returns   : const File*: Pointer to found file-object
//Requires  : pResult != NULL; searchDir already set
/*--------------------------------------------------------------------------*/
const File* PathDirectorySearch::find (unsigned long attribs) {
   assert (checkIntegrity () <= DirectorySearch::LAST);

   TRACE9 ("PathDirectorySearch::find (unsigned long) - Full:  "
           << searchPath.data () << " -> " << srch);

   const File* rc = NULL;
   do {
      // Build filename with next (= first on first call) node of path
      std::string node = searchPath.getNextExpandedNode ();
      if (node.empty ()) {
	 clearEntry ();
         return NULL;
      }

      if (makePath (node, srch)) {
         TRACE5 ("PathDirectorySearch::find (unsigned long) - Node: "
                 << node);
         setSearchValue (node);
         rc = DirectorySearch::find (attribs);
      } // endif
   } while (!rc);
   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the next file which matches the search-criteria
//Returns   : const File*: Pointer to found file-object
//Requires  : pResult != NULL; searchDir already set
/*--------------------------------------------------------------------------*/
const File* PathDirectorySearch::next () {
   TRACE9 ("PathDirectorySearch::next (): " << searchPath.data ()  << " -> " << srch);

   assert (!checkIntegrity ());

   TRACE5 ("PathDirectorySearch::next () - searchPath::actNode ()="
	   << searchPath.getActNode ());

   const File* tmp = DirectorySearch::next ();
   while (!tmp) {
      if (searchPath.getActNode ().empty ()) {
         clearEntry ();
         return NULL;
      } // endif nodes available

      tmp = find (attr);
   } // end-while
   return tmp;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the status of the object
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int PathDirectorySearch::checkIntegrity () const {
   return (((const std::string&)searchPath).empty ()
            ? NO_PATH : DirectorySearch::checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Creates the filename to search (inclusive (expanded) path-info
//Parameters: path: On input it must contain the path where to search the
//                  file; on output it is replaced with the (expanded) file
//                  (inclusive path) to search
//            file: Name of the file to find (only if return = false)
//Returns   : bool: false in case of an error; true otherwise
//Remarks   : If the variable path contains wildcards, they are expanded (only
//            UNIX).
/*--------------------------------------------------------------------------*/
bool PathDirectorySearch::makePath (std::string& path, const std::string& file) {
   if (path[path.length () - 1] != File::DIRSEPARATOR)
      path += File::DIRSEPARATOR;
   path += file;
   return true;
}
