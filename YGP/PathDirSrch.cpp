//$Id: PathDirSrch.cpp,v 1.1 1999/09/26 01:52:26 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : PathDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.9.1999
//COPYRIGHT   : Anticopyright (A) 1999

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


#define DEBUG 0
#include "Trace.h"
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
//Returns   : Status; 0: OK
//Requires  : pResult != NULL; searchDir already set
/*--------------------------------------------------------------------------*/
int PathDirectorySearch::find (dirEntry& result, unsigned long attribs) {
   assert (checkIntegrity () <= DirectorySearch::LAST);

   TRACE5 ("PathDirectorySearch::find (result, attribs): "
           << (const std::string&)searchPath << " -> " << srch);

   // Build filename with first node of path
   std::string node (searchPath.getNextNode ());
   if (node.empty ())
#ifdef UNIX
      return ENOENT;
#else
#  ifdef WINDOWS
      return WinGetLastError ();
#  endif
#endif

   makePath (node, srch);
   
   TRACE9 ("PathDirectorySearch::find (result, attribs): " << node);
   setFile (node);
   return DirectorySearch::find (result, attribs) ? find () : 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the next file which matches the search-criteria
//Returns   : Status; 0: OK
//Requires  : pResult != NULL; searchDir already set
/*--------------------------------------------------------------------------*/
int PathDirectorySearch::find () {
   TRACE5 ("PathDirectorySearch::find (): " << (const std::string&)searchPath
           << " -> " << srch);

   assert (!checkIntegrity ());

   int rc (DirectorySearch::find ());
   while (rc) {
      if (searchPath.getActNode ().empty ())
	 return rc;

      TRACE9 ("PathDirectorySearch::find (): searchPath::nextNode () "
              << searchPath.getActNode ());
      rc = DirectorySearch::find ("?");   // Dummy-value to initial new search
   }
   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the status of the object
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int PathDirectorySearch::checkIntegrity () const {
   return (((const std::string&)searchPath).empty ()
	   ? NO_PATH : DirectorySearch::checkIntegrity ());
}
