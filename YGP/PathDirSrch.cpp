//$Id: PathDirSrch.cpp,v 1.7 2000/04/11 22:42:45 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : PathDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.7 $
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

#include <errno.h>

#ifdef UNIX
#  include <wordexp.h>
#endif

#define DEBUG 0
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
//Returns   : Status; 0: OK
//Requires  : pResult != NULL; searchDir already set
/*--------------------------------------------------------------------------*/
int PathDirectorySearch::find (dirEntry& result, unsigned long attribs) {
   assert (checkIntegrity () <= DirectorySearch::LAST);

   TRACE5 ("PathDirectorySearch::find (result, attribs): "
           << (const std::string&)searchPath << " -> " << srch);

   int rc = -1;
   do {
      // Build filename with first node of path
      std::string node (searchPath.getNextNode ());
      if (node.empty ())
         return ENOENT;

      if (makePath (node, srch)) {
         TRACE9 ("PathDirectorySearch::find (result, attribs): " << node);
         setFile (node);
         rc = DirectorySearch::find (result, attribs);
      }
   } while (rc);
   return 0;
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

   if (searchPath.getActNode ().empty ())
      return ENOENT;

   int rc (DirectorySearch::find ());
   while (rc) {
      if (searchPath.getActNode ().empty ())
	 return rc;

      TRACE9 ("PathDirectorySearch::find (): searchPath::nextNode () "
              << searchPath.getActNode ());
      rc = DirectorySearch::find ("?");  // Dummy-value to initiate new search
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

/*--------------------------------------------------------------------------*/
//Purpose   : Creates the filename to search (inclusive (expanded) path-info
//Parameters: path: On input it must contain the path where to search the
//                  file; on output it is replaced with the (expanded) file
//                  (inclusive path) to search
//            file: Name of the file to find (only if return = false)
//Returns   : bool: false in case of an error; true otherwise
//Remarks   : If the variable path contains wildcards, they are expanded (only
//            UNIX). If this expansion results in more than one node, an
//            excpetion is thrown.
/*--------------------------------------------------------------------------*/
bool PathDirectorySearch::makePath (std::string& path, const std::string& file) {
#ifdef UNIX
   wordexp_t result;
   int       rc (wordexp (path.c_str (), &result, 0));
   
   if (result.we_wordc == 1)
      path = result.we_wordv[0];
   else {
      wordfree (&result);
      return false;
   }

   wordfree (&result);
#endif

   if (path[path.length () - 1] != DirectorySearch::getSplitChar ())
      path += DirectorySearch::getSplitChar ();
   path += file;
   return true;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the previously defined path
//Parameters: path: Path to check
//Remarks   : Throws an exception describing the error-node of the path (of
//            course only if one node is wrong)
/*--------------------------------------------------------------------------*/
void PathDirectorySearch::checkPath (const std::string& path) throw (std::string) {
#ifndef UNIX
   return;
#endif

   PathSearch ps (path);
   std::string node (ps.getNextNode ());
   std::string temp;

   wordexp_t result = { 0, NULL, 0 };
   int       rc;

   // Check all nodes if they are valid
   while (!node.empty ()) {
      rc = wordexp (node.c_str (), &result, WRDE_REUSE);
      switch (rc) {
      case 0:
         if (result.we_wordc != 1) {
            rc = 1;
            node += " expands to more than one word";
          }
          break;

      case WRDE_NOSPACE: node = "Out of memory"; break;
      case WRDE_BADCHAR: node += " contains unquoted invalid character such as `|'"; break;
      case WRDE_SYNTAX: temp = node; node = "Syntax error in " + temp + " (unmatched quoting character?)"; break;
      default: temp = node; node = "Unknown error in " + temp;
      } // end-switch wordexp-error
      wordfree (&result);
      if (rc) {
	 throw std::string (node);
	 return;
      }

      node = ps.getNextNode ();
   } // end-while nodes available
}
