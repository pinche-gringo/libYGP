#ifndef DIRSRCH_H
#define DIRSRCH_H

//$Id: DirSrch.h,v 1.21 2001/08/28 20:18:20 markus Exp $

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
   DirectorySearch ();
   DirectorySearch (const std::string& search);
   virtual ~DirectorySearch ();

   virtual void setSearchValue (const std::string& search);
   virtual const std::string& getDirectory () const { return searchDir; }
   virtual const std::string& getFileSpec () const { return searchFile; }

   virtual bool isValid () const;
   virtual bool isValid (const std::string& dir) const;

   //@Section searching
   int find (const std::string& search, dirEntry& result,
             unsigned long attribs = IDirectorySearch::FILE_NORMAL) {
      cleanup ();
      setSearchValue (search);
      return find (result, attribs); }
   virtual int find (dirEntry& result,
                     unsigned long attribs = IDirectorySearch::FILE_NORMAL);
   virtual int find ();

 protected:
   virtual int checkIntegrity () const;

   void cleanup ();

   void result (dirEntry* pResult) { assert (pResult); pEntry = pResult; }
   void result (dirEntry& result) { pEntry = &result; }

   std::string searchDir;
   std::string searchFile;

   unsigned long attr;
   enum { DIRSRCH_OK = 0, NO_ENTRY_PATH, NO_ENTRY, NO_DIR, NO_FILE, LAST};

 private:
   //@Section prohibited manager functions
   DirectorySearch (const DirectorySearch&);
   DirectorySearch& operator= (const DirectorySearch&);

#if SYSTEM == UNIX
   DIR* pDir;
#elif SYSTEM == WINDOWS
   HANDLE hSearch;
#endif
};

#endif // DIRSRCH_H
