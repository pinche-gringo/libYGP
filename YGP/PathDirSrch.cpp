//$Id: PathDirSrch.cpp,v 1.26 2003/11/14 20:27:55 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : PathDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.26 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.9.1999
//COPYRIGHT   : Anticopyright (A) 1999 - 2003

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

#include <cerrno>

#include "YGP/Internal.h"

#include "YGP/File.h"
#include "YGP/Check.h"
#include "YGP/Trace_.h"
#include "YGP/PathDirSrch.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
PathDirectorySearch::~PathDirectorySearch () {
}


//-----------------------------------------------------------------------------
/// Searches for first file matching the input specified by the constructor(s)
/// or the set-methods. If a search within a node of the path returns an
/// error, continue with the next node.
/// \param attribs: Attributes the file must contain
/// \returns <tt>const File*</tt>: Pointer to found file-object
/// \pre The values to search for must have been set before!
/// \remarks Every node of the path is tilde-expanded (UNIX-like to the home
///     directory).
//-----------------------------------------------------------------------------
const File* PathDirectorySearch::find (unsigned long attribs) {
   TRACE9 ("PathDirectorySearch::find (unsigned long) - "
           << (std::string)searchPath << " -> " << srch);
   Check1 (checkIntegrity () <= DirectorySearch::LAST);

   const File* rc;
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

//-----------------------------------------------------------------------------
/// Returns the next matching file according to parameters specified in
/// earlier find-calls. If a search within a node of the path returns an
/// error, continue with the next node.
/// \returns <tt>const File*</tt>: Pointer to found file-object or NULL
/// \pre The search must have already been started by a find!
/// \remarks Every node of the path is tilde-expanded (UNIX-like to the home
///     directory).
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Checks if this object is integer. If yes 0 is returned, else a number
/// describing the error.
/// \returns \c int: Status; 0: OK
//-----------------------------------------------------------------------------
int PathDirectorySearch::checkIntegrity () const {
   return (((const std::string&)searchPath).empty ()
            ? NO_PATH : DirectorySearch::checkIntegrity ());
}

}
