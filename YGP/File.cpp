//$Id: File.cpp,v 1.12 2002/12/01 21:18:34 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : File
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.12 $
//AUTHOR      : Markus Schwab
//CREATED     : 28.3.2001
//COPYRIGHT   : Anticopyright (A) 2001, 2002

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

#include <stdio.h>
#include <errno.h>

#include "Internal.h"

#include "Trace_.h"

#include "File.h"


#if SYSTEM == UNIX
const char File::DIRSEPARATOR = '/';
#else
const char File::DIRSEPARATOR = '\\';
#endif


/*--------------------------------------------------------------------------*/
//Purpose   : Copyconstructor
//Parameter : other: Object to copy
/*--------------------------------------------------------------------------*/
File::File (const File& other) : path_ (other.path_)
#if SYSTEM == UNIX
   , entry (other.entry), status (other.status), userExec (other.userExec)
#elif SYSTEM == WINDOWS
   , WIN32_FIND_DATA (other)
#else
#  error Not implemented!
#endif
{
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
File::~File () {
}



/*--------------------------------------------------------------------------*/
//Purpose   : Assignmentoperator
//Parameters: other: Object to copy
//Returns   : File&: Reference to this
/*--------------------------------------------------------------------------*/
File& File::operator= (const File& other) {
   if (this != &other) {
      path_ = other.path_;
#if SYSTEM == UNIX
      entry = other.entry;
      status = other.status;
      userExec = other.userExec;
#elif SYSTEM == WINDOWS
      WIN32_FIND_DATA::operator= (other);
#endif
   } // endif
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Duplicates (clones) the object and returns a pointer to the
//            newly created object.
//Returns   : File*: Pointer to newly created clone
/*--------------------------------------------------------------------------*/
File* File::clone () const {
   return new File (*this);
}

#if SYSTEM == WINDOWS

/*--------------------------------------------------------------------------*/
//Purpose   : Returns true, if the file is executeable. Under UNIX it checks
//            the file-permissions (any execute-right is sufficient), under
//            DOS/WINDOZE the extension of the file (EXE/COM/BAT).
//Returns   : Flag, if file is executeable
//Remarks   : - VBS, WSH, and whatever Windows executes directly are not considered executable
//            - The call is only valid after a successfull find.
/*--------------------------------------------------------------------------*/
bool File::isExecuteable () const {
   const char* pEnd = strrchr (cFileName, '.');
   if (pEnd++) {
      static char* aexeExtensions[] = { "EXE", "COM", "BAT" };
      char   compExt[4];
      unsigned int i;

      // Copy uppercase extension for compare
      for (i = 0; i < sizeof (compExt); ++) {
         compExt[i] = toupper (*pEnd);
         if (!*pEnd)
            break;
         ++pEnd;
      }

      // Check if an extension matches
      for (i = 0; i < (sizeof (aexeExtensions) / sizeof (aexeExtensions[0])); ++i)
         if (*(unsigned int *)aexeExtensions[i] == *(unsigned int *)compExt)
            return true;
      return false;
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the modification time of the file in the format of the
//            system-time.
//Returns   : struct tm*: Pointer to time
//Remarks   : - The call is only valid after a successfull find.
/*--------------------------------------------------------------------------*/
const time_t File::time () const {
   struct tm fileTime;
   time (fileTime);
   return mktime (&fileTime);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the (local) time of the file in a (C-)struct tm.
//Returns   : time: Broken down time structure to set
//Remarks   : - The call is only valid after a successfull find.
/*--------------------------------------------------------------------------*/
void File::localtime (struct tm& time) const {
   FILETIME fileTemp;
   FileTimeToLocalFileTime (&ftLastWriteTime, &fileTemp);
   setTime (fileTemp, time);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the filetime into a (C-)struct tm.
//Parameters: time: Time of the file (in Windows format)
//            result: Time of file converted into a struct tm
//Remarks   : - The tm_wday, tm_yday and tm_isdst-members are not set!
//            - The call is only valid after a successfull find.
/*--------------------------------------------------------------------------*/
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
   result.tm_yday = 0;                                                // TODO?
   result.tm_isdst = 1;
}

#endif

/*--------------------------------------------------------------------------*/
//Purpose   : Opends the file in the specified mode. The mode parameter can
//            have the same values than the ANSI-C fopen-function.
//Parameters: mode: Mode for open the file (analogue to libc's fopen)
//Returns   : void*: Pointer to a handle for the opened file.
//Throws    : string: In case of an error a textual description
/*--------------------------------------------------------------------------*/
void* File::open  (const char* mode) const throw (std::string) {
   std::string file (path ()); file += name ();
   TRACE5 ("File::open  (const char*) const - " << file);
   Check3 (mode);

   FILE* pFile = fopen (file.c_str (), mode);
   if (pFile == NULL)
      throwErrorText (N_("Error opening file `%1'! Reason: %2"));

   return pFile;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Closes a (previously opened) file
//Parameters: file: Handle of opened file
//Throws    : string: In case of an error a textual description
/*--------------------------------------------------------------------------*/
void File::close (void* file) const throw (std::string) {
   TRACE5 ("File::close  () const - " << path () << name ());
   Check3 (file);

   if (fclose (static_cast <FILE*> (file)))
      throwErrorText (N_("Error closing file `%1'! Reason: %2"));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads the (next) specified number of characters from the
//            (previously opened) file (or less, if the end of the file has
//            been reached) into buffer. and returns the number of actually
//            read bytes.
//
//            If an error occurres, an exception is thrown
//Parameters: file: Handle of openeded file
//            buffer: Buffer for data
//            length: Maximal length of buffer
//Returns   : int: Number of read bytes
//Throws    : string: In case of an error a textual description
/*--------------------------------------------------------------------------*/
int File::read (void* file, char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("File::read  (char*, unsigned int) const - " << path () << name ());

   Check3 (file);
   Check3 (buffer);
   Check3 (length);

   int rc (fread (buffer, 1, length, static_cast <FILE*> (file)));
   if (!rc)                        // Exception only if *no* char has been read
      throwErrorText (N_("Error reading from file `%1'! Reason: %2"));

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Writes the specified number of characters to the  (previously
//            opened) file and returns the number of actually written bytes.
//
//            If an error occurres, an exception is thrown.
//Parameters: file: Handle of openeded file
//            buffer: Buffer of data
//            length: Length of buffer (= bytes to write)
//Returns   : int: Number of written bytes
//Throws    : string: In case of an error a textual description
/*--------------------------------------------------------------------------*/
int File::write (void* file, const char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("File::write  (char*, unsigned int) const - " << path () << name ());
   Check3 (file);
   Check3 (buffer);
   Check3 (length);

   int rc (fwrite (buffer, 1, length, static_cast <FILE*> (file)));
   if (rc < length)
      throwErrorText (N_("Error writing to file `%1!' Reason: %2"));

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if further data is available for reading.
//Parameters: file: Handle of openeded file
//Returns   : bool: True, if further data is available
/*--------------------------------------------------------------------------*/
bool File::isEOF (void* file) const throw (std::string) {
   Check3 (file);
   return feof (static_cast <FILE*> (file)) != 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Throws an error consisting of the passed string, where the
//            characters '%1' are subsituted with the name of the file and
//           '%2' with an error message (according to strerror).
//Parameters: error: ASCIIZ-String describing error-message
//Requires  : error != NULL, an ASCIIZ-string with the placeholders %1, %2
/*--------------------------------------------------------------------------*/
void File::throwErrorText (const char* error) const throw (std::string) {
   Check3 (error);
   std::string file (path ());
   file += name ();
   std::string err (_(error));
   err.replace (err.find ("%1"), 2, file);
   err.replace (err.find ("%2"), 2, strerror (errno));
   throw (err);
}
