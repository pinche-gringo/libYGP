#ifndef FILE_H
#define FILE_H

//$Id: File.h,v 1.14 2002/12/25 05:09:40 markus Rel $

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
// 1. File is protected derived from "local" base
//   or maybe even better
// 2. Use an abstract implementation-class with concrete realizations for
//    every OS (like abstract IFile with concrete IFileUNIX, which
//    is (protected) used by File).


#include <stdio.h>

#include <time.h>

#ifdef TM_IN_SYS_TIME
#  include <sys/time.h>
#endif

#include <gzo-cfg.h>

#if SYSTEM == UNIX
#  if HAVE_DIRENT_H
#     include <dirent.h>
#     undef NAMLEN
#     define NAMLEN(dirent) strlen((dirent)->d_name)
#  else
#     undef dirent
#     undef NAMLEN
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

#include <Check.h>

class DirectorySearch;

// Class to handle result of DirectorySearch
#if SYSTEM == UNIX

#define MAX_PATH            MAXNAMLEN

struct File {
   friend class DirectorySearch;
   friend class RemoteDirSearch;

   File () : userExec (false)
      { *entry.d_name = '\0'; }
   File (const File& other);
   virtual ~File ();

   File& operator= (const File& other);

   virtual File* clone () const;

   //@Section query of data
   const char*   path () const { return path_.c_str (); }
   const char*   name () const { return entry.d_name; }
   unsigned long size () const { return status.st_size; }
   time_t        time () const { return status.st_mtime; }
   void          time (struct tm& time) const
     { time = *gmtime (&status.st_mtime); }
   void          localtime (struct tm& time) const
     { time = *::localtime (&status.st_mtime); }
   unsigned long attributes () const { return status.st_mode; }

   //@Section compare filename
   int compare (const char* pszName) const { return strcmp (name (), pszName); }
   int compare (const std::string& Name) const { return name () == Name; }
   int compare (const File& other) const { return strcmp (name (), other.name ()); }
   int compare (const File* other) const { Check1 (other);
      return compare (*other); }

   //@Section file-type
   bool isHidden () const { return *entry.d_name == '.'; }
   bool isDirectory () const { return S_ISDIR (status.st_mode); }
   bool isExecuteable () const { return status.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH); }
   bool isUserExec () const { return userExec; }

   //@Section file-access
   virtual bool isEOF (void* file) const throw (std::string);
   virtual void* open  (const char* mode) const throw (std::string);
   virtual void close (void* file) const throw (std::string);
   virtual int  read  (void* file, char* buffer, unsigned int length) const throw (std::string);
   virtual int  write (void* file, const char* buffer, unsigned int length) const throw (std::string);

   static const char DIRSEPARATOR;

 protected:
   std::string   path_;
   struct dirent entry;
   struct stat   status;

   bool  userExec;

   void throwErrorText (const char* error) const throw (std::string);

   void path (const char* path) { path_ = path; }
   void path (const std::string& path) { path_ = path; }
   void name (const char* name) { strcpy (entry.d_name, name); }
   void name (const std::string& name) {
      memcpy (entry.d_name, name.data (), name.length ());
      entry.d_name[name.length ()] = '\0'; }
   void size (unsigned long size) { status.st_size = size; }
   void time (time_t time) { status.st_mtime = time; }
   void attributes (unsigned long attr) { status.st_mode = attr; }
};

#elif SYSTEM == WINDOWS

struct File : protected WIN32_FIND_DATA {
   friend class DirectorySearch;
   friend class RemoteDirSearch;

   File () { }
   File (const File& other);
   virtual ~File ();

   File& operator= (const File& other);

   virtual File* clone () const;

   //@Section query of data
   const char*   path () const { return path_.c_str (); }
   const char*   name () const { return cFileName; }
   unsigned long size () const { return nFileSizeLow; }
   time_t        time () const;
   void          time (struct tm& time) const {
      setTime (ftLastWriteTime, time); }
   void                localtime (struct tm& time) const;
   unsigned long attributes () const { return dwFileAttributes; }

   //@Section compare filename
   int compare (const char* pszName) const { return stricmp (name (), pszName); }
   int compare (const std::string& Name) const { return compare (Name.c_str ()); }
   int compare (const File& other) const { return compare (other.name ()); }
   int compare (const File* other) const { Check1 (other);
      return compare (*other); }

   //@Section file-type
   bool isHidden () const { return (dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)!= 0; }
   bool isDirectory () const { return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
   bool isExecuteable () const;
   bool isUserExec () const { return isExecuteable (); }

   //@Section file-access
   virtual bool isEOF (void* file) const throw (std::string);
   virtual void* open  (const char* mode) const throw (std::string);
   virtual void close (void* file) const throw (std::string);
   virtual int  read  (void* file, char* buffer, unsigned int length) const throw (std::string);
   virtual int  write (void* file, const char* buffer, unsigned int length) const throw (std::string);

   static const char DIRSEPARATOR;

 protected:
   std::string path_;

   void throwErrorText (const char* error) const throw (std::string);

   void path (const char* path) { path_ = path; }
   void path (const std::string& path) { path_ = path; }
   void name (const char* name) { if (name) strcpy (cFileName, name); }
   void name (const std::string& name) {
      memcpy (cFileName, name.data (), name.length ());
      cFileName[name.length ()] = '\0'; }
   void size (unsigned long size) { nFileSizeLow = size; }
   void time (time_t) { Check (0); }
   void attributes (unsigned long attr) { dwFileAttributes = attr; }

 private:
   static void setTime (const FILETIME& time, struct tm& result);
};

#else
#  error Not implemented yet!
#endif

typedef struct File File;

#endif
