//$Id: DirSrch.cpp,v 1.56 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : DirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.56 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.7.1999
//COPYRIGHT   : Copyright (C) 1999 - 2005, 2007, 2008

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


#include <ygp-cfg.h>

#if SYSTEM == UNIX
#  include <unistd.h>
#elif SYSTEM == WINDOWS
#  include <sys/types.h>
#  include <sys/stat.h>
#endif

#include <cerrno>

#include "YGP/File.h"
#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/FileRExp.h"

#include "YGP/DirSrch.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Defaultconstructor
//-----------------------------------------------------------------------------
DirectorySearch::DirectorySearch () : IDirectorySearch (), searchDir (1, '.'),
				      searchFile (), attr (0), offStrip (0)
#if SYSTEM == UNIX
     , pDir (NULL)
#else
     , hSearch (INVALID_HANDLE_VALUE)
#endif
{
   TRACE9 ("DirectorySearch::DirectorySearch ()");
   searchDir += File::DIRSEPARATOR;
}

//-----------------------------------------------------------------------------
/// Constructor; creates an object with the files to search. The parameter may
/// include path-information.
/// \param search: Files (including the path) to search for.
//-----------------------------------------------------------------------------
DirectorySearch::DirectorySearch (const std::string& search)
   : IDirectorySearch (), searchDir (), searchFile (), attr (0), offStrip (0),
#if SYSTEM == UNIX
     pDir (NULL)
#else
     hSearch (INVALID_HANDLE_VALUE)
#endif
{
   TRACE9 ("DirectorySearch::DirectorySearch (const std::string&) - " << search.c_str ());
   setSearchValue (search);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
DirectorySearch::~DirectorySearch () {
   cleanup ();
}


//-----------------------------------------------------------------------------
/// Retrieves the first file matching the previously specified
/// search-criteria.
/// \param attribs: Attributes the file must contain
/// \returns <tt>const File*</tt>: Pointer to found file-object or NULL
/// \pre The search values must have been specified!
//-----------------------------------------------------------------------------
const File* DirectorySearch::find (unsigned long attribs) {
   TRACE9 ("DirectorySearch::find (unsigned long)");
   cleanup ();

   attr = attribs;

   TRACE5 ("DirectorySearch::find (unsigned long) " << searchDir.c_str ()
	   << searchFile.c_str ());

   pEntry = new File;
   // Set path-information; but cut path in case of special cases
   pEntry->path_ = offStrip
      ? searchDir.substr ((offStrip > 0) ? offStrip : ((offStrip < 0) ? -offStrip : 0))
      : searchDir;
   TRACE8 ("DirectorySearch::find (unsigned long) - Search in dir " << pEntry->path_);
   Check3 (!checkIntegrity ());

#if SYSTEM == UNIX
   pDir = opendir (searchDir.c_str ());
   TRACE9 ("DirectorySearch::find (unsigned long) - opendir: " << (pDir ? "OK" : "Failed"));
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
   TRACE8 ("DirectorySearch::find (unsigned long) - found " << pEntry->name ());

   FileRegularExpr regExp (searchFile.c_str ());
   Check3 (!regExp.checkIntegrity ());

   // Attribut-handling: Files having attrs not specified here are not returned
   unsigned long attr_ = ~convertToSysAttribs (attr | FILE_ATTRIBUTE_ARCHIVE);

   return ((pEntry->attributes () & attr_)
           || !regExp.matches (pEntry->name ()))
          ? next () : pEntry;
#  else
#     error Not implemented!
#  endif
#endif
}

//-----------------------------------------------------------------------------
/// Returns the next matching file according to parameters specified in
/// earlier find-calls. If a file was found 0 is returned, errno otherwise.
/// \returns <tt>const File*</tt>: Pointer to found file-object or NULL
/// \pre The search must have already been started by a find!
//-----------------------------------------------------------------------------
const File* DirectorySearch::next () {
   TRACE9 ("DirectorySearch::next ()");

   Check3 (!checkIntegrity ());
   FileRegularExpr regExp (searchFile.c_str ());
   Check3 (!regExp.checkIntegrity ());

#if SYSTEM == UNIX
   Check3 (pDir);

   unsigned long attr_ = convertToSysAttribs (attr);

   std::string workfile (pEntry->path_);
   std::string temp;
   struct dirent* pDirEnt;
   while ((pDirEnt = readdir (pDir)) != NULL) {            // Files available?
      TRACE8 ("DirectorySearch::next () - found " << pDirEnt->d_name);

      if ((!(attr & FILE_HIDDEN)) && (*pDirEnt->d_name == '.')
          && searchFile[0] != '.')
         continue;

      if (regExp.matches (pDirEnt->d_name)) {
	 temp = workfile + pDirEnt->d_name;

	 if (!stat (temp.c_str (), &pEntry->status)) {
            // Do attributes match?
            TRACE9 ("DirectorySearch::next (): " << pDirEnt->d_name << " (" << std::hex
                    << attr_ << ") -> Mode: " << pEntry->status.st_mode << std::dec);
            if ((attr_ & pEntry->status.st_mode) == pEntry->status.st_mode) {
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
   // Attribut-handling: Files having attrs not specified here are not returned
   unsigned long attr_ = ~convertToSysAttribs (attr | FILE_ATTRIBUTE_ARCHIVE);

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

//-----------------------------------------------------------------------------
/// Checks if this object is integer. If yes 0 is returned, else a number
/// describing the error.
/// \returns \c int: Status; 0: OK
//-----------------------------------------------------------------------------
int DirectorySearch::checkIntegrity () const {
   TRACE9 ("DirectorySearch::checkIntegrity () const");

   return (searchDir.empty ()
	   ? NO_DIR
	   : (searchFile.empty ()
	      ? NO_FILE
	      : (pEntry
		 ? DIRSRCH_OK + (pEntry->path_.empty () && !offStrip)
		 : NO_ENTRY)));
}

//-----------------------------------------------------------------------------
/// Sets the value for the next search. Internally it splits the search-string
/// in its directory- and filepart; it prepares the internal data also for a
/// new search.
/// \param search: Files to find
//-----------------------------------------------------------------------------
void DirectorySearch::setSearchValue (const std::string& search) {
   TRACE8 ("DirectorySearch::setSearchValue (const std::string&) - " << search);
   Check3 (!search.empty ());

   pEntry = NULL;          // New search-value means new search with new result
   offStrip = 0;

   searchFile = search;

   size_t len (search.length () - 1);
   if (searchFile[len] == File::DIRSEPARATOR)
      searchFile.replace (len, 1, 0, '\0');

   len = searchFile.rfind (File::DIRSEPARATOR);
   if (len != std::string::npos) {
      searchDir = searchFile;
      TRACE9 ("DirectorySearch::setSearchValue - 1: " << searchDir);
      searchDir.replace (len + 1, searchDir.length (), 0, '\0');
      TRACE9 ("DirectorySearch::setSearchValue - 2: " << searchDir);
      searchFile.replace (0, len + 1, 0, '\0');
      TRACE9 ("DirectorySearch::setSearchValue - 3: " << searchFile);
   }
   else {
      if (searchFile.empty ()) {
         searchDir = File::DIRSEPARATOR;
         searchFile = '.';
         offStrip = 1;
      }
      else {
         offStrip = 2;
         searchDir = '.';
         searchDir += File::DIRSEPARATOR;
      }
   }
   TRACE9 ("DirectorySearch::setSearchValue - checkIntegrity () = " << checkIntegrity ());
   Check3 (checkIntegrity () <= NO_ENTRY);
}

//-----------------------------------------------------------------------------
/// Releases all internally used resources.
//-----------------------------------------------------------------------------
void DirectorySearch::cleanup () {
   TRACE9 ("DirectorySearch::cleanup ()");
   clearEntry ();

#if SYSTEM == UNIX
#  ifdef CLOSEDIR_VOID
   if (pDir)
      closedir (pDir);
#  else
   if (pDir) {
#if CHECK >= 3
      int rc =
#endif
      closedir (pDir);
      Check3 (!rc); }
#  endif
   pDir = NULL;
#elif SYSTEM == WINDOWS
   FindClose (hSearch);
   hSearch = INVALID_HANDLE_VALUE;
#else
#  error Not implemented yet!
#endif
}

//-----------------------------------------------------------------------------
/// Checks if the passed string specifies an existing directory.
/// \param dir: Directory whose validity should be checked
/// \returns \c bool: True if the directory exists
//-----------------------------------------------------------------------------
bool DirectorySearch::isValid (const std::string& dir) {
   TRACE8 ("DirectorySearch::isValid (const std::string&) - " << dir.c_str ());

   struct stat file;

#ifdef _MSC_VER
   std::string temp (dir);
   if (temp[temp.length () - 1] == File::DIRSEPARATOR)
      temp.replace (temp.length () - 1, 1, 0, '\0');
   return (!::stat (temp.c_str (), &file) && (file.st_mode & _S_IFDIR));
#else
   return (!::stat (dir.c_str (), &file) && (file.st_mode & S_IFDIR));
#endif
}

//-----------------------------------------------------------------------------
/// Checks if the directory-part of this object specifies an existing
/// directory.
/// \returns \c bool: True if the directory exists
//-----------------------------------------------------------------------------
bool DirectorySearch::isValid () const {
   return isValid (searchDir);
}

}
