#ifndef DIRSRCH_H
#define DIRSRCH_H

//$Id: DirSrch.h,v 1.6 1999/09/07 22:41:01 Markus Rel $

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
#include <assert.h>

#ifdef UNIX

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#include <sys/stat.h>
#else
#ifdef WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#else
#error Not yet implemented!
#endif
#endif

#include <string>


class DirectorySearch;


// Class to handle result of DirectorySearch
#ifdef UNIX
#define FILE_NORMAL         (S_IFREG | S_ISUID | S_ISGID | S_ISVTX \
                             | S_IRWXU | S_IRWXG | S_IRWXO)
#define FILE_READONLY       (S_IFREG | S_ISUID | S_ISGID | S_ISVTX \
                             | S_IRUSR | S_IRGRP | S_IROTH \
                             | S_IXUSR | S_IXGRP | S_IXOTH)
#define FILE_DIRECTORY      (S_IFDIR)
#define FILE_HIDDEN         0x80000000

#define MAX_PATH            MAXNAMLEN

typedef struct dirEntry {
   friend class DirectorySearch;

   char*         pPath;
   struct dirent entry;
   struct stat   status;

   dirEntry () : pPath (new char [MAXNAMLEN]), pEndPath (pPath)
       , userExec (false)
     { assert (pPath); *entry.d_name = '\0'; }
   dirEntry (const dirEntry& o);
   virtual ~dirEntry () { delete [] pPath; }

   const dirEntry& operator= (const dirEntry& o);

   //@Section query of data
   const char*         path () const { assert (pPath); return pPath; }
   const char*         name () const { return entry.d_name; }
   const unsigned long size () const { return status.st_size; }
   const time_t        time () const { return status.st_mtime; }
   void                time (struct tm& time) const
     { time = *localtime (&status.st_mtime); }
   const unsigned long attributs () const { return status.st_mode; }

   //@Section file-type
   bool isDirectory () const { return S_ISDIR (status.st_mode); }
   bool isExecuteable () const { return status.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH); }
   bool isUserExec () const { return userExec; }

 private:
   char* pEndPath;
   bool  userExec;
} dirEntry;
#else
#ifdef WINDOWS
#define FILE_NORMAL         FILE_ATTRIBUTE_ARCHIVE
#define FILE_READONLY       FILE_ATTRIBUTE_READONLY
#define FILE_DIRECTORY      FILE_ATTRIBUTE_DIRECTORY
#define FILE_HIDDEN         (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)

typedef struct dirEntry : public WIN32_FIND_DATA {
   char* pPath;

   dirEntry () : pPath (new char [MAX_PATH]) { assert (pPath); }
   dirEntry (const dirEntry& o);
   virtual ~dirEntry () { delete [] pPath; }

   const dirEntry& operator= (const dirEntry& o);

   //@Section query of data
   const char*         path () const { assert (pPath); return pPath; }
   const char*         name () const { return cFileName; }
   const unsigned long size () const { return nFileSizeLow; }
   const time_t        time () const;
   void                time (struct tm& time) const;
   const unsigned long attributs () const { return dwFileAttributes; }

   //@Section file-type
   bool isDirectory () const { return dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY; }
   bool isExecuteable () const;
   bool isUserExec () const { return isExecuteable (); }
} dirEntry;

#else
#error Not implemented yet!
#endif
#endif


#ifndef MAX_PATH
#error MAX_PATH must be defined as max. filelength in every operating-system!
#endif

// Class to search for files in a certain directory
class DirectorySearch {
 public:
   static const char DIR_SPLIT;

   //@Section manager-functions
   DirectorySearch () : pEntry (NULL)
#ifdef UNIX
     , pDir (NULL)
#else
     , hSearch (INVALID_HANDLE_VALUE)
#endif
     { }
   DirectorySearch (const std::string& search) : pEntry (NULL)
#ifdef UNIX
      , pDir (NULL)
#else
      , hSearch (INVALID_HANDLE_VALUE)
#endif
      { setFile (search); }
   virtual ~DirectorySearch ();

   void setFile (const std::string& search);

   //@Section searching
   inline int find (const std::string& search, dirEntry* pResult,
		    unsigned long attribs = 0);
   int find (dirEntry* pResult, unsigned long attribs = 0);
   int find ();

 private:
   int checkIntegrity () const;

   inline void cleanup ();

   std::string searchDir;
   std::string searchFile;

#if defined (UNIX) || defined (WINDOWS)
   unsigned long attr;
#endif
#ifdef UNIX
   DIR* pDir;
#else
#ifdef WINDOWS
   HANDLE hSearch;
#endif
#endif
   dirEntry* pEntry;
};


// Implementation of inline-functions
inline int DirectorySearch::find (const std::string& search, dirEntry* pResult,
                                  unsigned long attribs) {
   assert (!search.empty ());
   setFile (search);
   return find (pResult, attribs);
}


inline void DirectorySearch::cleanup () {
#ifdef UNIX
# ifdef CLOSEDIR_VOID
   int rc (closedir (pDir)); assert (!rc);   
#else
   closedir (pDir);
#endif
   pDir = NULL;
#else
#ifdef WINDOWS
   FindClose (hSearch);
   hSearch = INVALID_HANDLE_VALUE;
#else
#error Not implemented yet!
#endif
#endif
}

#endif
