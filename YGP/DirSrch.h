#ifndef DIRSRCH_H
#define DIRSRCH_H

//$Id: DirSrch.h,v 1.28 2002/12/07 22:10:08 markus Exp $

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


#include <time.h>
#include <stddef.h>
#include <string.h>

#if SYSTEM == UNIX
#  if HAVE_DIRENT_H
#     include <dirent.h>
#     define NAMLEN(dirent) strlen((dirent)->d_name)
#  else
#     define dirent direct
#     define NAMLEN(dirent) (dirent)->d_namlen
#     if HAVE_SYS_NDIR_H
#        include <sys/ndir.h>
#     endif
#     if HAVE_SYS_DIR_H
#        include <sys/dir.h>
#     endif
#     if HAVE_NDIR_H
#        include <ndir.h>
#     endif
#  endif
#endif

#include <gzo-cfg.h>

#include <IDirSrch.h>

struct File;
class DirectorySearch;


// Class to search for files in a certain directory.  This search can
// be restricted to files matching certain name-criterias or by
// attributes.
//
// The name-part of the files to search supports UNIX-like wildcards;
// that are the asterisk (*) for any number of any characters, the
// question-mark for any single character and a set of characters in
// brackets (([) and (])). This set can contain a list of characters
// (like [abcde]) or a region (like [a-e]). To invert this set use a
// leading caret (^) or a leading exclamation mark (!), like ([^a-e]).
//
// The found (and matching) files are retrieved by objects of type File.
//
// Note: The class does not do any word expansion for the search-path
//       (like expanding the tilde (~) to the home-directory)!
class DirectorySearch : public IDirectorySearch {
 public:
   //@Section manager-functions
   DirectorySearch ();
   DirectorySearch (const std::string& search);
   virtual ~DirectorySearch ();

   virtual void setSearchValue (const std::string& search);
   virtual std::string getDirectory () const { return searchDir; }
   virtual std::string getFileSpec () const { return searchFile; }

   virtual bool isValid () const;
   static bool isValid (const std::string& dir);

   //@Section searching
   const File* find (const std::string& search,
                     unsigned long attribs = IDirectorySearch::FILE_NORMAL) {
      cleanup ();
      setSearchValue (search);
      return find (attribs); }
   virtual const File* find (unsigned long attribs = IDirectorySearch::FILE_NORMAL);
   virtual const File* next ();

 protected:
   virtual int checkIntegrity () const;

   void cleanup ();

   std::string searchDir;
   std::string searchFile;

   unsigned long attr;
   enum { DIRSRCH_OK = 0, NO_ENTRY_PATH, NO_ENTRY, NO_DIR, NO_FILE, LAST};

 private:
   //@Section prohibited manager functions
   DirectorySearch (const DirectorySearch&);
   DirectorySearch& operator= (const DirectorySearch&);

   int offStrip;
#if SYSTEM == UNIX
   DIR* pDir;
#elif SYSTEM == WINDOWS
   HANDLE hSearch;
#endif
};

#endif // DIRSRCH_H
