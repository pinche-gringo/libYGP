#ifndef DIRSRCH_H
#define DIRSRCH_H

//$Id: DirSrch.h,v 1.18 2001/04/02 21:00:43 markus Exp $

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


// TODO/FIXME?: Maybe the classes in this files should be changed to either
// 1. dirEntry is protected derived from "local" base
//   or maybe even better
// 2. Use an abstract implementation-class with concrete realizations for
//    every OS (like abstract IdirEntry with concrete IdirEntryUNIX, which
//    is (protected) used by dirEntry).


#include <time.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include <gzo-cfg.h>

#include <DirEntry.h>
#include <IDirSrch.h>

class DirectorySearch;


#ifndef MAX_PATH
#  error MAX_PATH must be defined as max. filelength in every operating-system!
#endif

// Class to search for files in a certain directory
class DirectorySearch : public IDirectorySearch {
 public:
   //@Section manager-functions
   DirectorySearch () : IDirectorySearch ()
#if SYSTEM == UNIX
     , pDir (NULL)
#else
     , hSearch (INVALID_HANDLE_VALUE)
#endif
     { }
   DirectorySearch (const std::string& search) : IDirectorySearch ()
#if SYSTEM == UNIX
      , pDir (NULL)
#else
      , hSearch (INVALID_HANDLE_VALUE)
#endif
      { setFile (search); }
   virtual ~DirectorySearch ();

   void setFile (const std::string& search);
   std::string getFile () const { return searchDir + searchFile; }

   //@Section searching
   inline int find (const std::string& search, dirEntry& result,
		    unsigned long attribs = FILE_NORMAL);
   inline int find (const std::string& search) {
      setFile (search); assert (!checkIntegrity ());
      return find (*pEntry, attr); }
   virtual int find (dirEntry& result, unsigned long attribs = FILE_NORMAL);
   virtual int find ();

#if SYSTEM == UNIX
   enum { FILE_NORMAL    = (S_IFREG | S_IFLNK | S_ISUID | S_ISGID | S_ISVTX
                            | S_IRWXU | S_IRWXG | S_IRWXO),
          FILE_READONLY  = (S_IFREG | S_IFLNK | S_ISUID | S_ISGID | S_ISVTX
                            | S_IRUSR | S_IRGRP | S_IROTH \
                            | S_IXUSR | S_IXGRP | S_IXOTH),
          FILE_DIRECTORY = (S_IFDIR | S_ISUID | S_ISGID | S_ISVTX
                            | S_IRWXU | S_IRWXG | S_IRWXO),
	  FILE_HIDDEN    = (1 << (sizeof (long) * 8 - 1)) };
#elif SYSTEM == WINDOWS
   enum { FILE_NORMAL    = (~FILE_ATTRIBUTE_DIRECTORY),
          FILE_READONLY  = FILE_ATTRIBUTE_READONLY,
          FILE_DIRECTORY = FILE_ATTRIBUTE_DIRECTORY,
          FILE_HIDDEN    = (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN) };
#else
#  error Not implemented yet!
#endif


 protected:
   virtual int checkIntegrity () const;

   void cleanup ();

   void result (dirEntry* pResult) { assert (pResult); pEntry = pResult; }
   void result (dirEntry& result) { pEntry = &result; }

   enum { DIRSRCH_OK = 0, NO_ENTRY_PATH, NO_ENTRY, NO_DIR, NO_FILE, LAST};

 private:
   //@Section prohibited manager functions
   DirectorySearch (const DirectorySearch&);
   DirectorySearch& operator= (const DirectorySearch&);

   std::string searchDir;
   std::string searchFile;

   unsigned long attr;
#if SYSTEM == UNIX
   DIR* pDir;
#elif SYSTEM == WINDOWS
   HANDLE hSearch;
#endif
};


// Implementation of inline-functions
inline int DirectorySearch::find (const std::string& search, dirEntry& result,
                                  unsigned long attribs) {
   assert (!search.empty ());
   setFile (search);
   return find (result, attribs);
}

#endif // DIRSRCH_H
