//$Id: DirSrch.cpp,v 1.34 2001/10/02 23:03:51 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : DirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.34 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.7.1999
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


#include <gzo-cfg.h>

#if SYSTEM == UNIX
#  include <unistd.h>
#elif SYSTEM == WINDOWS
#  include <ctype.h>
#  include <string.h>
#endif

#include <errno.h>

#define DEBUG 0
#include "File.h"
#include "Trace_.h"
#include "DirSrch.h"
#include "FileRExp.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
DirectorySearch::DirectorySearch () : IDirectorySearch (), searchDir (1, '.')
#if SYSTEM == UNIX
     , pDir (NULL)
#else
     , hSearch (INVALID_HANDLE_VALUE)
#endif
{
   TRACE9 ("DirectorySearch::DirectorySearch ()");
   searchDir += File::DIRSEPERATOR;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: search: Path (and files) to search
/*--------------------------------------------------------------------------*/
DirectorySearch::DirectorySearch (const std::string& search)
   : IDirectorySearch ()
#if SYSTEM == UNIX
      , pDir (NULL)
#else
      , hSearch (INVALID_HANDLE_VALUE)
#endif
{
   TRACE9 ("DirectorySearch::DirectorySearch (const std::string&) - " << search.c_str ());
   setSearchValue (search);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
DirectorySearch::~DirectorySearch () {
   cleanup ();
}


/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the first file which matches the search-criteria
//Parameters: result: Buffer where to place the result
//Returns   : const File*: Pointer to created file-object
//Requires  : searchDir already set
/*--------------------------------------------------------------------------*/
const File* DirectorySearch::find (unsigned long attribs) {
   TRACE9 ("DirectorySearch::find (unsigned long)");
   cleanup ();

   attr = convertToSysAttribs (attribs);

   TRACE5 ("DirectorySearch::find (result, attribs) " << searchDir.c_str ()
	   << searchFile.c_str ());

   pEntry = new File;
   pEntry->path_ = searchDir;
   assert (!checkIntegrity ());

#if SYSTEM == UNIX
   pDir = opendir (searchDir.c_str ());
   if (!pDir) {
      clearEntry ();
      return NULL;
   }
#else
#  if SYSTEM == WINDOWS
   std::string temp (searchDir + '*');
   hSearch = FindFirstFile (temp.c_str (), pEntry);
   if (hSearch == INVALID_HANDLE_VALUE) {
      clearEntry ();
      return NULL;
   }
#  else
#     error Not implemented!
#  endif
#endif

#if SYSTEM == UNIX
   return next ();
#else
#  if SYSTEM == WINDOWS
   TRACE8 ("DirectorySearch::find (result, attribs) - found " << pEntry->name ());

   FileRegularExpr regExp (searchFile.c_str ());
   assert (!regExp.checkIntegrity ());

   // Attribut-handling: Files having attrs not specified here are not ret.
   unsigned long attr_ = ~(attr | FILE_ATTRIBUTE_ARCHIVE);

   return ((pEntry->attributes () & attr_)
           || !regExp.matches (pEntry->name ()))
          ? next () : pEntry;
#  else
#     error Not implemented!
#  endif
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the next file which matches the search-criteria
//Returns   : const File*: Pointer to created file-object
//Requires  : searchDir, pEntry  already set
/*--------------------------------------------------------------------------*/
const File* DirectorySearch::next () {
   TRACE9 ("DirectorySearch::next ()");

   assert (!checkIntegrity ());
   FileRegularExpr regExp (searchFile.c_str ());
   assert (!regExp.checkIntegrity ());

#if SYSTEM == UNIX
   assert (pDir);

   std::string workfile (pEntry->path_);
   std::string temp;
   struct dirent* pDirEnt;
   while ((pDirEnt = readdir (pDir)) != NULL) {            // Files available?
      TRACE8 ("DirectorySearch::next () - found " << pDirEnt->d_name);

      if ((!(attr & FILE_HIDDEN)) && (*pDirEnt->d_name == '.'))
         continue;

      if (regExp.matches (pDirEnt->d_name)) {
	 temp = workfile + pDirEnt->d_name;

	 if (!stat (temp.c_str (), &pEntry->status)) {
            // Do attributes match?
            TRACE9 ("DirectorySearch::next (): " << pDirEnt->d_name << " (" << hex
                    << attr << ") -> Mode: " << hex << pEntry->status.st_mode);
            if ((attr & pEntry->status.st_mode) == pEntry->status.st_mode) {
               pEntry->entry = *pDirEnt;
               pEntry->userExec = !access (temp.c_str (), X_OK);
               TRACE1 ("DirectorySearch::next () - match " << pEntry->name ());
               return pEntry;
            } // endif attributs OK
	 } // endif stat succeeded
      } // endif filename OK
   } // end-while files available

   clearEntry ();
   return NULL;
#else
#  if SYSTEM == WINDOWS
   // Attribut-handling: Files having attrs not specified here are not ret.
   unsigned long attr_ = ~(attr | FILE_ATTRIBUTE_ARCHIVE);

   while (FindNextFile (hSearch, pEntry))
      if (!(pEntry->attributes () & attr_)
          && regExp.matches (pEntry->name ())) {
         TRACE1 ("DirectorySearch::find () - match " << pEntry->name ());
         return pEntry;
      }

   clearEntry ();
   return NULL;
#  else
#      error Not implemented yet!
#   endif
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the status of the object
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int DirectorySearch::checkIntegrity () const {
   TRACE9 ("DirectorySearch::checkIntegrity () const");

   return searchDir.empty () ? NO_DIR : searchFile.empty () ? NO_FILE :
                                        pEntry ? 
                                        pEntry->path_.empty () : NO_ENTRY;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Splits the search-string in its directory- and filepart; it
//            prepares the internal data also for a new search
//Parameters: search: Files to find
/*--------------------------------------------------------------------------*/
void DirectorySearch::setSearchValue (const std::string& search) {
   TRACE8 ("DirectorySearch::setSearchValue (const std::string&) - " << search);
   assert (!search.empty ());

   pEntry = NULL;          // New search-value means new search with new result

   searchDir = '.';
   searchDir += File::DIRSEPERATOR;
   searchFile = search;

   unsigned int len (search.length () - 1);
   if (searchFile[len] == File::DIRSEPERATOR)
      searchFile.replace (len, 1, 0, '\0');

   len = searchFile.rfind (File::DIRSEPERATOR);
   if (len != std::string::npos) {
      searchDir = searchFile;
      TRACE9 ("DirectorySearch::setSearchValue - 1: " << searchDir);
      searchDir.replace (len + 1, searchDir.length (), 0, '\0');
      TRACE9 ("DirectorySearch::setSearchValue - 2: " << searchDir);
      searchFile.replace (0, len + 1, 0, '\0');
      TRACE9 ("DirectorySearch::setSearchValue - 3: " << searchFile);
   }
   TRACE9 ("DirectorySearch::setSearchValue - checkIntegrity () = " << checkIntegrity ());
   assert (checkIntegrity () <= NO_ENTRY);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Cleanup of data
/*--------------------------------------------------------------------------*/
void DirectorySearch::cleanup () {
   TRACE9 ("DirectorySearch::cleanup ()");
   clearEntry ();

#if SYSTEM == UNIX
#  ifdef CLOSEDIR_VOID
   closedir (pDir);
#  else
   if (pDir) {
      int rc (closedir (pDir)); assert (!rc);
   }
#  endif
   pDir = NULL;
#elif SYSTEM == WINDOWS
   FindClose (hSearch);
   hSearch = INVALID_HANDLE_VALUE;
#else
#  error Not implemented yet!
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the searchDir is really a direcory
//Parameters: dir: Directory whose validity should be checked
//Returns   : bool: True if the directory exists
/*--------------------------------------------------------------------------*/
bool DirectorySearch::isValid (const std::string& dir) {
   TRACE8 ("DirectorySearch::isValid (const std::string&) - " << dir.c_str ());

   struct stat file;

#if SYSTEM == WINDOWS
   std::string temp (dir);
   if (temp[temp.length () - 1] == File::DIRSEPERATOR)
      temp.replace (temp.length () - 1, 1, 0, '\0');
   return (!stat (temp.c_str (), &file) && (file.st_mode & S_IFDIR));
#endif
   return (!stat (dir.c_str (), &file) && (file.st_mode & S_IFDIR));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the searchDir is really a direcory
//Parameters: dir: Directory whose validity should be checked
//Returns   : bool: True if the directory exists
/*--------------------------------------------------------------------------*/
bool DirectorySearch::isValid () const {
   return isValid (searchDir);
}
