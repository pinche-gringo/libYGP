//$Id: File.cpp,v 1.11 2002/10/20 23:10:51 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : File
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.11 $
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
//Parameter : o: Object to copy
/*--------------------------------------------------------------------------*/
File::File (const File& o) : path_ (o.path_)
#if SYSTEM == UNIX
   , entry (o.entry), status (o.status), userExec (o.userExec)
#elif SYSTEM == WINDOWS
   , WIN32_FIND_DATA (o)
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
//Parameter : o: Object to copy
//Returns   : File&: Reference to this
/*--------------------------------------------------------------------------*/
File& File::operator= (const File& o) {
   if (this != &o) {
      path_ = o.path_;
#if SYSTEM == UNIX
      entry = o.entry;
      status = o.status;
      userExec = o.userExec;
#elif SYSTEM == WINDOWS
      WIN32_FIND_DATA::operator= (o);
#endif
   } // endif
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Copy-method
//Returns   : File*: Pointer to newly created clone
/*--------------------------------------------------------------------------*/
File* File::clone () const {
   return new File (*this);
}

#if SYSTEM == WINDOWS

/*--------------------------------------------------------------------------*/
//Purpose   : Tests, if the file is executeable. This is decided on the file-
//            extension: EXE-, COM- and BAT-files are considered executeable
//Returns   : Flag, if file is executeable
//Remarks   : Other also executeable files (like 4DOS BTM-files; I think
//            there's even another WINDOZE-format) are not considered as exes.
/*--------------------------------------------------------------------------*/
bool File::isExecuteable () const {
   const char* pEnd = strrchr (cFileName, '.');
   if (pEnd++)
      // Check the next 4 bytes; ignore end-of-string, the buffer is long
      // enough (MAX_PATH) and the trailing \0 is in the compare
      switch (toupper (*pEnd++) + (toupper (*pEnd++) << 8)
              + (toupper (*pEnd++) << 16) + (toupper (*pEnd) << 24)) {
#if defined __BORLANDC__ || defined __GNUG__
      case 'EXE\0' :
      case 'COM\0' :
      case 'BAT\0' :
#else
#  ifdef _MSC_VER
      case '\0EXE' :
      case '\0MOC' :
      case '\0TAB' :
#  else
#     error Unsupported compiler! Please check how long character-constants are treated.
#  endif
#endif
	 return true;
      } // end-switch
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the time of the file in a time_t.
//Returns   : struct tm*: Pointer to time
//Remarks   : The tm_wday, tm_yday and tm_isdst-members are not set!
/*--------------------------------------------------------------------------*/
const time_t File::time () const {
   struct tm fileTime;
   time (fileTime);
   return mktime (&fileTime);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the (local) time of the file in a (C-)struct tm.
//Returns   : struct tm*: Pointer to time
//Remarks   : The tm_wday, tm_yday and tm_isdst-members are not set!
/*--------------------------------------------------------------------------*/
void File::localtime (struct tm& time) const {
   FILETIME fileTemp;
   FileTimeToLocalFileTime (&ftLastWriteTime, &fileTemp);
   setTime (fileTemp, time);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the (local) time of the file in a (C-)struct tm.
//Returns   : struct tm*: Pointer to time
//Remarks   : The tm_wday, tm_yday and tm_isdst-members are not set!
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
   assert ((result.tm_mon >= 0) && (result.tm_mon <= 11));
   result.tm_year = sysTime.wYear - 1900; assert (result.tm_year >= 0);
   result.tm_yday = 0;                                                // TODO?
   result.tm_isdst = 1;
}

#endif

/*--------------------------------------------------------------------------*/
//Purpose   : Opens this for further read/write access
//Parameters: mode: Mode for open the file (analogue to libc's fopen)
//Returns   : void*: Pointer to a handle to the opened file.
/*--------------------------------------------------------------------------*/
void* File::open  (const char* mode) const throw (std::string) {
   std::string file (path ()); file += name ();
   TRACE5 ("File::open  (const char*) const - " << file);
   assert (mode);

   FILE* pFile = fopen (file.c_str (), mode);
   if (pFile == NULL)
      throwErrorText (N_("Error opening file `%1'! Reason: %2"));

   return pFile;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Closes the (previously opened file
//Parameters: file: Handle of opened file
/*--------------------------------------------------------------------------*/
void File::close (void* file) const throw (std::string) {
   TRACE5 ("File::close  () const - " << path () << name ());
   assert (file);

   if (fclose (static_cast <FILE*> (file)))
      throwErrorText (N_("Error closing file `%1'! Reason: %2"));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads (the next) length bytes from the (previously opened) file
//Parameters: file: Handle of openeded file
//            buffer: Buffer for data
//            length: Maximal length of buffer
/*--------------------------------------------------------------------------*/
int File::read (void* file, char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("File::read  (char*, unsigned int) const - " << path () << name ());

   assert (file);
   assert (buffer);
   assert (length);

   int rc (fread (buffer, 1, length, static_cast <FILE*> (file)));
   if (!rc)                        // Exception only if *no* char has been read
      throwErrorText (N_("Error reading from file `%1'! Reason: %2"));

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Writes length bytes to the (previously opened) file
//Parameters: file: Handle of openeded file
//            buffer: Buffer of data
//            length: Length of buffer (= bytes to write)
/*--------------------------------------------------------------------------*/
int File::write (void* file, const char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("File::write  (char*, unsigned int) const - " << path () << name ());
   assert (file);
   assert (buffer);
   assert (length);

   int rc (fwrite (buffer, 1, length, static_cast <FILE*> (file)));
   if (rc < length)
      throwErrorText (N_("Error writing to file `%1!' Reason: %2"));

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if further data is available
//Parameters: file: Handle of openeded file
/*--------------------------------------------------------------------------*/
bool File::isEOF (void* file) const throw (std::string) {
   assert (file);
   return feof (static_cast <FILE*> (file)) != 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends a description of the last error the the passed string
//Parameters: error: ASCIIZ-String describing error-message
//Requires  : error != NULL, an ASCIIZ-string with the placeholders %1, %2
/*--------------------------------------------------------------------------*/
void File::throwErrorText (const char* error) const throw (std::string) {
   assert (error);
   std::string file (path ());
   file += name ();
   std::string err (_(error));
   err.replace (err.find ("%1"), 2, file);
   err.replace (err.find ("%2"), 2, strerror (errno));
   throw (err);
}
