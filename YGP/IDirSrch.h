#ifndef IDIRSRCH_H
#define IDIRSRCH_H

//$Id: IDirSrch.h,v 1.18 2008/03/29 17:35:17 markus Rel $

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


#include <string>

#include <YGP/File.h>


namespace YGP {

/**Abstract baseclass providing an interface for classes to search in
   directory-structures.

   The following attributes are supported for files:

     - FILE_NORMAL: Retrives all normal files (e.g: no directories).
     - FILE_READONLY: Retrieves all files which the user can not write
          to/change. This applies for files without w-flag in *NIX or with
          a special flag set for DOS/Windoze.
     - FILE_DIRECTORY: Retrieves only directories.
     - FILE_HIDDEN: Retrieves all hidden files. Under *NIX this are files
          starting with a dot (.); in Windows that are files with a special
          system attribute set.
 */
class IDirectorySearch {
 public:
   /// Default constructor
   IDirectorySearch () : pEntry (NULL) { }
   virtual ~IDirectorySearch ();

   /// Intended to set the files to search for; to be implemented by derived
   /// classes.
   virtual void setSearchValue (const std::string& search) = 0;
   /// Returns the files to search for
   std::string getSearchValue () const { return getDirectory () + getFileSpec (); }
   /// Intended to return the directory part of the files to search for; to be
   /// implemented by derived classes.
   virtual std::string getDirectory () const = 0;
   /// Intended to return the name/file part of the files to search for; to be
   /// implemented by derived classes.
   virtual std::string getFileSpec () const = 0;

   /// \name Searching
   //@{
   /// Method to start a find the files previously specified. Additonally the
   /// files returned must match the passed attributes.
   ///
   /// To be implemented by derived classes.
   /// \returns <tt>const File*</tt>: Pointer to found file or NULL
   virtual const File* find (unsigned long attribs = FILE_NORMAL) = 0;
   /// Method to find the next file matching the  previously specified values.
   ///
   /// To be implemented by derived classes.
   /// \returns <tt>const File*</tt>: Pointer to found file or NULL
   virtual const File* next () = 0;
   //@}

   /// Intended to check if the previously set search values are valid
   /// (e.g. the directory is valid, ...)
   ///
   /// To be implemented by derived classes.
   /// \returns \c True if values are OK, false otherwise
   virtual bool isValid () const = 0;
   /// Checks if the passed file is a "special" file (the system directories
   /// `.' and `..').
   static bool isSpecial (const char* pFile) {
      return (bool)((*pFile == '.') ? ((pFile[1] == '.') ? !pFile[2] : !pFile[1]) : false); }

   static unsigned long convertToSysAttribs (unsigned long attributes);
   static unsigned long convertFromSysAttribs (unsigned long attributes);

   static const int FILE_NORMAL;    ///< "Normal" file (e.g. no directory; no device, ...)
   static const int FILE_READONLY;      ///< File can not be altered/written to
   static const int FILE_DIRECTORY;       ///< The file is actually a directory
   static const int FILE_HIDDEN;        ///< The file is "hidden" in normal use

 protected:
   File* pEntry;                       ///< Pointer to (last) found File object

   /// Frees the buffer holding the last found file.
   void clearEntry () { delete pEntry; pEntry = NULL; }

 private:
   //@Section prohibited manager functions
   IDirectorySearch (const IDirectorySearch&);
   IDirectorySearch& operator= (const IDirectorySearch&);
};

}

#endif
