//$Id: PathDirSrch.cpp,v 1.18 2002/04/09 20:02:49 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : PathDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.18 $
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

#if SYSTEM == UNIX
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
//Returns   : const File*: Pointer to found file-object
//Requires  : pResult != NULL; searchDir already set
/*--------------------------------------------------------------------------*/
const File* PathDirectorySearch::find (unsigned long attribs) {
   assert (checkIntegrity () <= DirectorySearch::LAST);

   TRACE9 ("PathDirectorySearch::find (unsigned long) - Full:  "
           << searchPath.data () << " -> " << srch);

   const File* rc (NULL);
   do {
      // Build filename with next (= first on first call) node of path
      std::string node (searchPath.getNextNode ());
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

   const File* tmp (DirectorySearch::next ());
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
//            UNIX). If this expansion results in more than one node, an
//            excpetion is thrown.
/*--------------------------------------------------------------------------*/
bool PathDirectorySearch::makePath (std::string& path, const std::string& file) {
#if SYSTEM == UNIX
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

   if (path[path.length () - 1] != File::DIRSEPERATOR)
      path += File::DIRSEPERATOR;
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
#if SYSTEM == UNIX
   PathSearch ps (path);
   std::string node (ps.getNextNode ());
   std::string error;

   wordexp_t result = { 0, NULL, 0 };
   int       rc;

   // Check all nodes if they are valid
   while (!node.empty ()) {
      rc = wordexp (node.c_str (), &result, WRDE_REUSE);
      switch (rc) {
      case 0:
         if (result.we_wordc != 1) {
            rc = 1;
            error = _("'%1' expands to more than one file");
          }
          break;

      case WRDE_NOSPACE: error = _("Out of memory"); break;
      case WRDE_BADCHAR: error = _("'%1' contains unquoted invalid character such as `|'"); break;
      case WRDE_SYNTAX: error = _("Syntax error in '%1' (unmatched quoting character?)"); break;
      default: node = _("Unknown error in '%1'");
      } // end-switch wordexp-error
      wordfree (&result);

      if (rc) {
         int pos (error.find ("%1"));
         if (pos != std::string::npos)
             error.replace (pos, 2, node);
         throw std::string (error);
         return;
      }

      node = ps.getNextNode ();
   } // end-while nodes available
#endif
}
