#ifndef DIRENTRY_H
#define DIRENTRY_H

//$Id: File.h,v 1.1 2001/04/02 20:57:29 markus Exp $

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
#include <assert.h>

#include <gzo-cfg.h>

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

#  if STAT_MACROS_BROKEN
#     error Autoconf reported invalid file-type macros (S_ISDIR, ...)
#  else
#     include <sys/stat.h>
#  endif

#elif SYSTEM == WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

#else
#  error Not yet implemented!
#endif


#include <string>

class DirectorySearch;


// Class to handle result of DirectorySearch
#if SYSTEM == UNIX

#define MAX_PATH            MAXNAMLEN


typedef struct dirEntry {
   friend class DirectorySearch;

   dirEntry () : userExec (false)
      { *entry.d_name = '\0'; }
   dirEntry (const dirEntry& o);
   ~dirEntry () { }

   const dirEntry& operator= (const dirEntry& o);

   //@Section query of data
   const char*         path () const { return path_.c_str (); }
   const char*         name () const { return entry.d_name; }
   const unsigned long size () const { return status.st_size; }
   const time_t        time () const { return status.st_mtime; }
   void                time (struct tm& time) const
     { time = *localtime (&status.st_mtime); }
   const unsigned long attributes () const { return status.st_mode; }

   //@Section compare filename
   int compare (const char* pszName) const { return strcmp (name (), pszName); }
   int compare (const std::string& Name) const { return strcmp (name (), Name.c_str ()); }
   int compare (const dirEntry& other) const { return strcmp (name (), other.name ()); }
   int compare (const dirEntry* other) const { assert (other);
      return strcmp (name (), other->name ()); }

   //@Section file-type
   bool isDirectory () const { return S_ISDIR (status.st_mode); }
   bool isExecuteable () const { return status.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH); }
   bool isUserExec () const { return userExec; }

   static const char DIRSEPERATOR = '/';

 private:
   std::string   path_;
   struct dirent entry;
   struct stat   status;

   bool  userExec;
} dirEntry;
#elif SYSTEM == WINDOWS

typedef struct dirEntry : public WIN32_FIND_DATA {
   friend class DirectorySearch;

   dirEntry () { }
   dirEntry (const dirEntry& o);
   ~dirEntry () { }

   const dirEntry& operator= (const dirEntry& o);

   //@Section query of data
   const char*         path () const { return path_.c_str (); }
   const char*         name () const { return cFileName; }
   const unsigned long size () const { return nFileSizeLow; }
   const time_t        time () const;
   void                time (struct tm& time) const;
   const unsigned long attributes () const { return dwFileAttributes; }

   //@Section compare filename
   int compare (const char* pszName) const { return stricmp (name (), pszName); }
   int compare (const std::string& Name) const { return strcmp (name (), Name.c_str ()); }
   int compare (const dirEntry& other) const { return stricmp (name (), other.name ()); }
   int compare (const dirEntry* other) const { assert (other);
      return stricmp (name (), other->name ()); }

   //@Section file-type
   bool isDirectory () const { return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
   bool isExecuteable () const;
   bool isUserExec () const { return isExecuteable (); }

   static const char DIRSEPERATOR = '\\';

 private:
   std::string path_;
} dirEntry;

#else
#  error Not implemented yet!
#endif


#endif