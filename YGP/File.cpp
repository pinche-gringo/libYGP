//$Id: File.cpp,v 1.1 2001/04/02 20:57:29 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : DirEntry
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 28.3.2001
//COPYRIGHT   : Anticopyright (A) 2001

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


#include <gzo-cfg.h>

#include "DirEntry.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Copyconstructor
//Parameter : o: Object to copy
/*--------------------------------------------------------------------------*/
dirEntry::dirEntry (const dirEntry& o) : path_ (o.path_)
#if SYSTEM == UNIX
   , entry (o.entry), status (o.status), userExec (o.userExec)
#else
#  if SYSTEM == WINDOWS
   , WIN32_FIND_DATA (o)
#  else
#     error Not implemented!
#  endif
#endif
{
}


/*--------------------------------------------------------------------------*/
//Purpose   : Assignmentoperator
//Parameter : o: Object to copy
/*--------------------------------------------------------------------------*/
const dirEntry& dirEntry::operator= (const dirEntry& o) {
   if (this != &o) {
      path_ = o.path_;
#if SYSTEM == UNIX
      entry = o.entry;
      status = o.status;
      userExec = o.userExec;
#endif
   } // endif
   return *this;
}

#if SYSTEM == WINDOWS

/*--------------------------------------------------------------------------*/
//Purpose   : Tests, if the file is executeable. This is decided on the file-
//            extension: EXE-, COM- and BAT-files are considered executeable
//Returns   : Flag, if file is executeable
//Remarks   : Other also executeable files (like 4DOS BTM-files; I think
//            there's even another WINDOZE-format) are not considered as exes.
/*--------------------------------------------------------------------------*/
bool dirEntry::isExecuteable () const {
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
const time_t dirEntry::time () const {
   struct tm fileTime;
   time (fileTime);
   return mktime (&fileTime);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the time of the file in a (C-)struct tm.
//Returns   : struct tm*: Pointer to time
//Remarks   : The tm_wday, tm_yday and tm_isdst-members are not set!
/*--------------------------------------------------------------------------*/
void dirEntry::time (struct tm& time) const {
   FILETIME fileTemp;
   SYSTEMTIME sysTime;
   FileTimeToLocalFileTime (&ftLastWriteTime, &fileTemp);
   FileTimeToSystemTime (&fileTemp, &sysTime);
   time.tm_sec = sysTime.wSecond;
   time.tm_min = sysTime.wMinute;
   time.tm_hour = sysTime.wHour;
   time.tm_mday = sysTime.wDay;
   time.tm_wday = sysTime.wDayOfWeek;
   time.tm_mon = sysTime.wMonth - 1; assert ((time.tm_mon >= 0) && (time.tm_mon <= 11));
   time.tm_year = sysTime.wYear - 1900; assert (time.tm_year >= 0);
   time.tm_yday = 0;                                                  // TODO?
   time.tm_isdst = 1;
}

#endif
