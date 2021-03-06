#ifndef YGP_FILE_H
#define YGP_FILE_H

//$Id: File.h,v 1.32 2008/06/10 21:47:32 markus Rel $

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


// TODO/FIXME?: Maybe the classes in this files should be changed to either
// 1. File is protected derived from "local" base
//   or maybe even better
// 2. Use an abstract implementation-class with concrete realizations for
//    every OS (like abstract IFile with concrete IFileUNIX, which
//    is (protected) used by File).


#include <cstdio>
#include <cstring>

#include <time.h>

#ifdef TM_IN_SYS_TIME
#  include <sys/time.h>
#endif

#include <ygp-cfg.h>

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

#     ifndef S_IXGRP
#        define S_IXGRP (S_IXUSR >> 4)
#     endif
#     ifndef S_IXOTH
#        define S_IXOTH (S_IXUSR >> 8)
#     endif
#  endif

#elif SYSTEM == WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

#else
#  error Not yet implemented!
#endif


#include <string>

#include <YGP/Check.h>
#include <YGP/Exception.h>

namespace YGP {

class DirectorySearch;

#if SYSTEM == UNIX

#ifdef MAX_PATH
#undef MAX_PATH
#endif
#define MAX_PATH            MAXNAMLEN

/**Class representing a file in the file system of the operating system.

   This class provides simple management functions.

   Objects of this type are usually returned as  result of a DirectorySearch,
   but can also be used by themself.
*/
struct File {
   friend class DirectorySearch;                   ///< Class mainly using File
   friend class RemoteDirSearch;                   ///< Class mainly using File

   /// Default constructor; creates an empty file object (holding no file)
   File () : path_ (), entry (), status (), userExec (false)
      { *entry.d_name = '\0'; }
   File (const char* name) throw (YGP::FileError);
   File (const File& other);
   virtual ~File ();

   File& operator= (const File& other);
   File& operator= (const char* name) throw (YGP::FileError);

   virtual File* clone () const;

   /// \name Query of data
   //@{
    /// Returning the file name
   const char*   path () const { return path_.c_str (); }
   /// Returning the path to the file
   const char*   name () const { return entry.d_name; }
   /// Returning the size of the file
   unsigned long size () const { return status.st_size; }
   /// Returning the time the file was modified the last time
   time_t        time () const { return status.st_mtime; }
   /// Retrieving the time the file was modified the last time (GMT)
   void          time (struct tm& time) const
     { time = *gmtime (&status.st_mtime); }
   /// Retrieving the time the file was modified the last time (local time)
   void          localtime (struct tm& time) const
     { time = *::localtime (&status.st_mtime); }
   /// Returning the system attributes of the time
   unsigned long attributes () const {
      return status.st_mode; }
   //@}

   /// \name Compare filename
   //@{
   /// Checks if the file name matches the passed text
   int compare (const char* pszName) const { return strcmp (name (), pszName); }
   /// Checks if the file name matches the passed text
   int compare (const std::string& Name) const { return -Name.compare (name ()) ; }
   /// Checks if the file name matches those of the other object
   int compare (const File& other) const { return strcmp (name (), other.name ()); }
   /// Checks if the file name matches those of the other object
   int compare (const File* other) const { Check1 (other);
      return compare (*other); }
   //@}

   /// \name File type
   //@{
   /// Checks if the operating system "hides" this file during "normal" operation
   bool isHidden () const { return *entry.d_name == '.'; }
   /// Checks if file is actually a directory (a file (potentially) holding more files)
   bool isDirectory () const { return S_ISDIR (status.st_mode); }
   /// Checks if file can be executed
   bool isExecuteable () const { return status.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH); }
   /// Checks if file can be executed by the current user
   bool isUserExec () const { return userExec; }
   //@}

   /// \name File access
   //@{
   virtual bool isEOF (void* file) const throw (YGP::FileError);
   virtual void* open  (const char* mode) const throw (YGP::FileError);
   virtual void close (void* file) const throw (YGP::FileError);
   virtual int  read  (void* file, char* buffer, unsigned int length) const throw (YGP::FileError);
   virtual int  write (void* file, const char* buffer, unsigned int length) const throw (YGP::FileError);
   //@}

   ///< Character to separte directories of the operating system
   static const char DIRSEPARATOR;

 protected:
   std::string   path_;           ///< Path of the file (don't access directly)
   struct dirent entry;           ///< File entry (as seen by the OS - don't access directly)
   struct stat   status;          ///< Attributes of the file (don't access directly)

   bool  userExec;   ///< Flag, if the file can be executed by the current user (don't access directly)

   void throwErrorText (const char* error) const throw (YGP::FileError);

   /// Sets the path of the file (only internally)
   void path (const char* path) { path_ = path; }
   /// Sets the path of the file (only internally)
   void path (const std::string& path) { path_ = path; }
   /// Sets the name of the file to the passed value
   void name (const char* name) { strncpy (entry.d_name, name, sizeof (entry.d_name)); }
   /// Sets the name of the file to the passed value (only internally)
   void name (const std::string& name) {
      memcpy (entry.d_name, name.data (), name.length ());
      entry.d_name[name.length ()] = '\0'; }
   /// Sets the size of the file (only internally)
   void size (unsigned long size) { status.st_size = size; }
   /// Sets the last modification time of the file (only internally)
   void time (time_t time) { status.st_mtime = time; }
   /// Sets the attributes the file (only internally)
   void attributes (unsigned long attr) { status.st_mode = attr; }
};

#elif SYSTEM == WINDOWS

struct File : protected WIN32_FIND_DATA {
   friend class DirectorySearch;
   friend class RemoteDirSearch;

   File () : WIN32_FIND_DATA (), path_ () { }
   File (const char* name) throw (YGP::FileError);
   File (const File& other);
   virtual ~File ();

   File& operator= (const File& other);
   File& operator= (const char* name) throw (YGP::FileError);

   virtual File* clone () const;

   /// \name Query of data
   //@{
   const char*   path () const { return path_.c_str (); }
   const char*   name () const { return cFileName; }
   unsigned long size () const { return nFileSizeLow; }
   time_t        time () const;
   void          time (struct tm& time) const {
      setTime (ftLastWriteTime, time); }
   void                localtime (struct tm& time) const;
   unsigned long attributes () const { return dwFileAttributes; }
   //@}

   /// \name Compare filename
   //@{
   int compare (const char* pszName) const { return stricmp (name (), pszName); }
   int compare (const std::string& Name) const { return compare (Name.c_str ()); }
   int compare (const File& other) const { return compare (other.name ()); }
   int compare (const File* other) const { Check1 (other);
      return compare (*other); }
   //@}

   /// \name File type
   //@{
   bool isHidden () const { return (dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)!= 0; }
   bool isDirectory () const { return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
   bool isExecuteable () const;
   bool isUserExec () const { return isExecuteable (); }
   //@}

   /// \name File access
   //@{
   virtual bool isEOF (void* file) const throw (YGP::FileError);
   virtual void* open  (const char* mode) const throw (YGP::FileError);
   virtual void close (void* file) const throw (YGP::FileError);
   virtual int  read  (void* file, char* buffer, unsigned int length) const throw (YGP::FileError);
   virtual int  write (void* file, const char* buffer, unsigned int length) const throw (YGP::FileError);
   //@}

   static const char DIRSEPARATOR;

 protected:
   std::string path_;

   void throwErrorText (const char* error) const throw (YGP::FileError);

   void path (const char* path) { path_ = path; }
   void path (const std::string& path) { path_ = path; }
   void name (const char* name) { if (name) strncpy (cFileName, name, sizeof (cFileName)); }
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

/// Declare also a File, to not have to use <tt>struct File</tt>
typedef struct File File;

}

#endif
