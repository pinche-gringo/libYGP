//$Id: PathDirSrch.cpp,v 1.21 2002/12/07 23:27:36 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : PathDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.21 $
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

#include "File.h"
#include "Check.h"
#include "Trace_.h"
#include "PathDirSrch.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
PathDirectorySearch::~PathDirectorySearch () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Searches for first file matching the input specified by the
//            constructor(s) or the set-methods.
//
//            If a search within a node of the path returns an error, continue
//            with the next node.
//Parameters: attribs: Attributes the file must contain
//Returns   : const File*: Pointer to found file-object
//Requires  : The values to search for must have been set before!
//Remarks   : Every node of the path is tilde-expanded (UNIX-like to the home directory).
/*--------------------------------------------------------------------------*/
const File* PathDirectorySearch::find (unsigned long attribs) {
   TRACE9 ("PathDirectorySearch::find (unsigned long) - "
           << searchPath << " -> " << srch);
   Check1 (checkIntegrity () <= DirectorySearch::LAST);

   const File* rc = NULL;
   do {
      // Build filename with next (= first on first call) node of path
      std::string node = searchPath.getNextExpandedNode ();
      if (node.empty ()) {
	 clearEntry ();
         return NULL;
      }

      if (node[node.length () - 1] != File::DIRSEPARATOR)
         node += File::DIRSEPARATOR;
      node += srch;
      TRACE5 ("PathDirectorySearch::find (unsigned long) - Search for: " << node);
      setSearchValue (node);
      rc = DirectorySearch::find (attribs);
   } while (!rc);
   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the next matching file according to parameters specified
//            in earlier find-calls.
//
//            If a search within a node of the path returns an error, continue
//            with the next node.
//Returns   : const File*: Pointer to found file-object or NULL
//Requires  : The search must have already been started by a find!
//Remarks   : Every node of the path is tilde-expanded (UNIX-like to the home directory).
/*--------------------------------------------------------------------------*/
const File* PathDirectorySearch::next () {
   TRACE9 ("PathDirectorySearch::next (): " << searchPath.data ()  << " -> " << srch);
   Check1 (!checkIntegrity ());

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
//Purpose   : Checks if this object is integer. If yes 0 is returned, else a
//            number describing the error.
//Returns   : int: Status; 0: OK
/*--------------------------------------------------------------------------*/
int PathDirectorySearch::checkIntegrity () const {
   return (((const std::string&)searchPath).empty ()
            ? NO_PATH : DirectorySearch::checkIntegrity ());
}
