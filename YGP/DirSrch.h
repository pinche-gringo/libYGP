#ifndef DIRSRCH_H
#define DIRSRCH_H

//$Id: DirSrch.h,v 1.32 2003/10/02 22:59:38 markus Rel $

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
#endif

#include <gzo-cfg.h>

#include <IDirSrch.h>

struct File;
class DirectorySearch;


/**Class to search for files in a certain (locally accesable) directory. This
   search can be restricted to files matching certain name-criterias or by
   attributes.

   The name-part of the files to search supports UNIX-like wildcards; that are
   the asterisk (*) for any number of any characters, the question-mark for
   any single character and a set of characters in brackets (([) and
   (])). This set can contain a list of characters (like [abcde]) or a region
   (like [a-e]). To invert this set use a leading caret (^) or a leading
   exclamation mark (!), like ([^a-e]).

   The found (and matching) files are retrieved by objects of type File.

   \note The class does \b not do any word expansion for the search-path
         (like expanding the tilde (~) to the home-directory)!
*/
class DirectorySearch : public IDirectorySearch {
 public:
   //@Section manager-functions
   DirectorySearch ();
   DirectorySearch (const std::string& search);
   virtual ~DirectorySearch ();

   virtual void setSearchValue (const std::string& search);
   /// Returns the directory part of the files to search for.
   virtual std::string getDirectory () const { return searchDir; }
   /// Returns the file specification of the files to search for.
   virtual std::string getFileSpec () const { return searchFile; }

   virtual bool isValid () const;
   static bool isValid (const std::string& dir);

   /// \name Searching
   //@{
   /// Searches for the specified files with the passed attributes.
   /// \returns <tt>const File*</tt>: Pointer to found file or NULL
   const File* find (const std::string& search,
                     unsigned long attribs = IDirectorySearch::FILE_NORMAL) {
      cleanup ();
      setSearchValue (search);
      return find (attribs); }
   /// Searches for previously specified files with the passed attributes.
   /// \returns <tt>const File*</tt>: Pointer to found file or NULL
   virtual const File* find (unsigned long attribs = IDirectorySearch::FILE_NORMAL);
   /// Method to find the next file matching the  previously specified values.
   /// \returns <tt>const File*</tt>: Pointer to found file or NULL
   virtual const File* next ();
   //@}

 protected:
   virtual int checkIntegrity () const;

   void cleanup ();

   std::string searchDir;   ///< Holding the directory part of the search value
   std::string searchFile;       ///< Holding the file part of the search value

   unsigned long attr;             ///< Attributes the found file must have set
   /// Status of object
   enum { DIRSRCH_OK = 0,                       ///< Object is in a valid state
          NO_ENTRY,     ///< Object is in a valid state, but has no entry found
          NO_DIR,                      ///< No directory to search in specified
          NO_FILE,                         ///< No file to search for specified
          LAST };

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
