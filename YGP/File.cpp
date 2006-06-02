//$Id: File.cpp,v 1.30 2006/06/02 02:33:39 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : File
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.30 $
//AUTHOR      : Markus Schwab
//CREATED     : 28.3.2001
//COPYRIGHT   : Copyright (C) 2000 - 2006

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

#include <ygp-cfg.h>

#include <cstdio>
#include <cerrno>
#include <cstring>

#if SYSTEM == UNIX
#  include <unistd.h>
#endif

#include "YGP/Internal.h"

#include "YGP/Trace.h"

#include "YGP/File.h"


namespace YGP {

#if SYSTEM == UNIX
const char File::DIRSEPARATOR = '/';
#elif SYSTEM == WINDOWS
const char File::DIRSEPARATOR = '\\';
#else
#  error Unsupported plattform!
#endif


//-----------------------------------------------------------------------------
/// Copyconstructor Parameter : other: Object to copy
//-----------------------------------------------------------------------------
File::File (const File& other) : path_ (other.path_)
#if SYSTEM == UNIX
   , entry (other.entry), status (other.status), userExec (other.userExec)
#elif SYSTEM == WINDOWS
   , WIN32_FIND_DATA (other)
#endif
{
}

//-----------------------------------------------------------------------------
/// Constructor; create from a name Parameter : name: Pointer to
/// character-array holding name of file to create
/// \throw String describing the error
//-----------------------------------------------------------------------------
File::File (const char* name) throw (std::string)
#if SYSTEM == UNIX
   : userExec (false)
#endif
{
   operator= (name);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
File::~File () {
}



//-----------------------------------------------------------------------------
/// Assignment operator
/// \param other: Object to copy
/// \returns \c File&: Reference to this
//-----------------------------------------------------------------------------
File& File::operator= (const File& other) {
   path_ = other.path_;
#if SYSTEM == UNIX
   entry = other.entry;
   status = other.status;
   userExec = other.userExec;
#elif SYSTEM == WINDOWS
   (*(WIN32_FIND_DATA*)this) = other;
#endif
   return *this;
}

//-----------------------------------------------------------------------------
/// Assignment operator; create from a name Parameter : name: Name of file
/// \returns Reference to self
/// \throw String describing the error
//-----------------------------------------------------------------------------
File& File::operator= (const char* name) throw (std::string) {
#if SYSTEM == UNIX
   // If file exists, fill data-fields
   if (!stat (name, &status)) {
      const char* posName (strrchr (name, DIRSEPARATOR));
      if (posName)
         path_.assign (name, ++posName - name);
      else {
         path_ = "./";
         posName = name;
      }
      strncpy (entry.d_name, posName, sizeof (entry.d_name));

      userExec = !access (name, X_OK);
   }
   else
      throw (strerror (errno));

#elif SYSTEM == WINDOWS
   HANDLE hFile;
   BY_HANDLE_FILE_INFORMATION fileInfo;
   if (((hFile = CreateFile (name, 0,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, 0, NULL))
        != INVALID_HANDLE_VALUE)
       && GetFileInformationByHandle (hFile, &fileInfo)) {
      const char* posName = strrchr (name, DIRSEPARATOR);
      if (posName)
         path_.assign (name, ++posName - name);
      else {
         path_ = ".\\";
         posName = name;
      }
      strncpy (cFileName, posName, sizeof (cFileName));
      dwFileAttributes = fileInfo.dwFileAttributes;
      ftCreationTime = fileInfo.ftCreationTime;
      ftLastAccessTime = fileInfo.ftLastAccessTime;
      ftLastWriteTime = fileInfo.ftLastWriteTime;
      nFileSizeHigh = fileInfo.nFileSizeHigh;
      nFileSizeLow = fileInfo.nFileSizeLow;

      CloseHandle (hFile);
   }
   else {
      char buffer[80];
      FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError (),
                     0, buffer, sizeof (buffer), NULL);
      throw (buffer);
   }
#endif

   return *this;
}

//-----------------------------------------------------------------------------
/// Duplicates (clones) the object and returns a pointer to the newly created
/// object.
/// \returns \c File*: Pointer to newly created clone
//-----------------------------------------------------------------------------
File* File::clone () const {
   return new File (*this);
}

#if SYSTEM == WINDOWS

//-----------------------------------------------------------------------------
/// Returns true, if the file is executeable. Under UNIX it checks the
/// file-permissions (any execute-right is sufficient), under DOS/WINDOZE the
/// extension of the file (EXE/COM/BAT).
/// \returns \c Flag, if file is executeable
/// \remarks
///     - VBS, WSH, and whatever Windows executes directly are not considered
///       as executable.
///     - The call is only valid with a defined object
//-----------------------------------------------------------------------------
bool File::isExecuteable () const {
   const char* pEnd = strrchr (cFileName, '.');
   if (pEnd++) {
      static char* aexeExtensions[] = { "EXE", "COM", "BAT" };
      char   compExt[4];
      unsigned int i;

      // Copy uppercase extension for compare
      for (i = 0; i < sizeof (compExt); ++i) {
         compExt[i] = toupper (*pEnd);
         if (!*pEnd)
            break;
         ++pEnd;
      }

      // Check if an extension matches
      for (i = 0; i < (sizeof (aexeExtensions) / sizeof (aexeExtensions[0])); ++i)
         if (*(unsigned int *)aexeExtensions[i] == *(unsigned int *)compExt)
            return true;
   }
   return false;
}

//-----------------------------------------------------------------------------
/// Retrieves the modification time of the file in the format of the
/// system-time.
/// \returns \c time_t: Filetime in system format
/// \remarks The call is only valid after a successfull find.
//-----------------------------------------------------------------------------
time_t File::time () const {
   struct tm fileTime;
   time (fileTime);
   return mktime (&fileTime);
}

//-----------------------------------------------------------------------------
/// Returns the (local) time of the file in a (C-)struct tm.
/// \param time: Broken down time structure to set
/// \remarks The call is only valid with a valid object
//-----------------------------------------------------------------------------
void File::localtime (struct tm& time) const {
   FILETIME fileTemp;
   FileTimeToLocalFileTime (&ftLastWriteTime, &fileTemp);
   setTime (fileTemp, time);
}

//-----------------------------------------------------------------------------
/// Converts the filetime into a (C-)struct tm.
/// \param time: Time of the file (in Windows format)
/// \param result: Time of file converted into a struct tm
/// \remarks
///     - The tm_wday, tm_yday and tm_isdst-members are not set!
///     - The call is only valid with a valid object
//-----------------------------------------------------------------------------
void File::setTime (const FILETIME& time, struct tm& result) {
   SYSTEMTIME sysTime;
   FileTimeToSystemTime (&time, &sysTime);

   result.tm_sec = sysTime.wSecond;
   result.tm_min = sysTime.wMinute;
   result.tm_hour = sysTime.wHour;
   result.tm_mday = sysTime.wDay;
   result.tm_wday = sysTime.wDayOfWeek;
   result.tm_mon = sysTime.wMonth - 1;
   Check3 ((result.tm_mon >= 0) && (result.tm_mon <= 11));
   result.tm_year = sysTime.wYear - 1900; Check3 (result.tm_year >= 0);
   result.tm_yday = 0;
   result.tm_isdst = 1;
}

#endif

//-----------------------------------------------------------------------------
/// Opends the file in the specified mode. The mode parameter can have the
/// same values than the ANSI-C fopen-function.
/// \param mode: Mode for open the file (analogue to libc's fopen)
/// \returns \c void*: Pointer to a handle for the opened file.
/// \throw string: In case of an error a textual description
//-----------------------------------------------------------------------------
void* File::open  (const char* mode) const throw (std::string) {
   std::string file (path ()); file += name ();
   TRACE5 ("File::open  (const char*) const - " << file);
   Check1 (mode);

   FILE* pFile = fopen (file.c_str (), mode);
   if (pFile == NULL)
      throwErrorText (N_("Error opening file `%1'! Reason: %2"));

   return pFile;
}

//-----------------------------------------------------------------------------
/// Closes a (previously opened) file
/// \param file: Handle of opened file
/// \throw string: In case of an error a textual description
//-----------------------------------------------------------------------------
void File::close (void* file) const throw (std::string) {
   TRACE5 ("File::close  () const - " << path () << name ());
   Check1 (file);

   if (fclose (static_cast <FILE*> (file)))
      throwErrorText (N_("Error closing file `%1'! Reason: %2"));
}

//-----------------------------------------------------------------------------
/// Reads the (next) specified number of characters from the (previously
/// opened) file (or less, if the end of the file has been reached) into
/// buffer. and returns the number of actually read bytes. If an error
/// occurres, an exception is thrown
/// \param file: Handle of openeded file
/// \param buffer: Buffer for data
/// \param length: Maximal length of buffer
/// \returns \c int: Number of read bytes
/// \throw string: In case of an error a textual description
//-----------------------------------------------------------------------------
int File::read (void* file, char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("File::read  (char*, unsigned int) const - " << path () << name ());
   Check1 (file);
   Check1 (buffer);
   Check1 (length);

   int rc (fread (buffer, 1, length, static_cast <FILE*> (file)));
   if (!rc)                              // Exception only if an error occured
      if (ferror (static_cast <FILE*> (file)))
         throwErrorText (N_("Error reading from file `%1'! Reason: %2"));
   return rc;
}

//-----------------------------------------------------------------------------
/// Writes the specified number of characters to the (previously opened) file
/// and returns the number of actually written bytes. If an error occurres, an
/// exception is thrown.
/// \param file: Handle of openeded file
/// \param buffer: Buffer of data
/// \param length: Length of buffer (= bytes to write)
/// \returns \c int: Number of written bytes
/// \throw string: In case of an error a textual description
//-----------------------------------------------------------------------------
int File::write (void* file, const char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("File::write  (char*, unsigned int) const - " << path () << name ());
   Check1 (file);
   Check1 (buffer);
   Check1 (length);

   int rc (fwrite (buffer, 1, length, static_cast <FILE*> (file)));
   if ((unsigned int)rc < length)
      throwErrorText (N_("Error writing to file `%1!' Reason: %2"));

   return rc;
}

//-----------------------------------------------------------------------------
/// Checks if further data is available for reading.
/// \param file: Handle of openeded file
/// \returns \c bool: True, if further data is available
//-----------------------------------------------------------------------------
bool File::isEOF (void* file) const throw (std::string) {
   Check1 (file);
   return feof (static_cast <FILE*> (file)) != 0;
}

//-----------------------------------------------------------------------------
/// Throws an error consisting of the passed string, where the characters '%1'
/// are subsituted with the name of the file and '%2' with an error message
/// (according to strerror).
/// \param error: ASCIIZ-String describing error-message
/// \pre error != NULL, an ASCIIZ-string with the placeholders %1, %2
//-----------------------------------------------------------------------------
void File::throwErrorText (const char* error) const throw (std::string) {
   Check1 (error);
   std::string file (path ());
   file += name ();
   std::string err (_(error));
   err.replace (err.find ("%1"), 2, file);
   err.replace (err.find ("%2"), 2, strerror (errno));
   throw (err);
}

}

