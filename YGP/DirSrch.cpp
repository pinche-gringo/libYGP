//$Id: DirSrch.cpp,v 1.6 1999/08/11 17:21:48 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Tokenize
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.7.1999
//COPYRIGHT   : Anticopyright (A) 1999

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifdef WINDOWS
#include <ctype.h>
#include <string.h>
#endif

#include <errno.h>

#include "DirSrch.h"
#include "FileRExp.h"

#ifdef UNIX
#include <unistd.h>

const char DirectorySearch::DIR_SPLIT = '/';
#else
const char DirectorySearch::DIR_SPLIT = '\\';
#endif


/*--------------------------------------------------------------------------*/
//Purpose   : Copyconstructor
//Parameter : o: Object to copy
/*--------------------------------------------------------------------------*/
dirEntry::dirEntry (const dirEntry& o) : pPath (new char [MAX_PATH])
#ifdef UNIX
   , entry (o.entry), status (o.status), userExec (o.userExec)
   , pEndPath (pPath + (o.pEndPath - o.pEndPath))
#endif
{
   assert (pPath);
   strcpy (pPath, o.pPath);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
DirectorySearch::~DirectorySearch () {
   cleanup ();
}


/*--------------------------------------------------------------------------*/
//Purpose   : Assignmentoperator
//Parameter : o: Object to copy
/*--------------------------------------------------------------------------*/
const dirEntry& dirEntry::operator= (const dirEntry& o) {
   if (this != &o) {
      pPath = new char [MAX_PATH]; assert (pPath);
      strcpy (pPath, o.pPath);
#ifdef UNIX
      entry = o.entry;
      status = o.status;
      pEndPath = pPath + (o.pEndPath - o.pEndPath);
      userExec = o.userExec;
#endif
   } // endif
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the first file which matches the search-criteria
//Parameters: pResult: Buffer where to place the result
//Returns   : Status; 0: OK
//Requires  : pResult != NULL; searchDir already set
/*--------------------------------------------------------------------------*/
int DirectorySearch::find (dirEntry* pResult, unsigned long attribs) {
   assert (pResult);

   cleanup ();
   pEntry = pResult;
   assert (!checkIntegrity ());

#ifdef UNIX
   attr = attribs;
   if ((attr & FILE_NORMAL) == FILE_NORMAL)
      attr &= ~FILE_NORMAL;

   pDir = opendir (searchDir.c_str ());
   if (!pDir)
      return errno;
#else
   #ifdef WINDOWS
   // Attribut-handling: Files having attrs not specified here are not ret.
   attr = ~attribs;
   searchDir += '*';
   hSearch = FindFirstFile (searchDir.c_str (), pEntry);
   if (hSearch == INVALID_HANDLE_VALUE)
      return GetLastError ();
   #else
      #error Not implemented!
   #endif

   // After (successful) start of search set searchDir to path of search
   searchDir.replace (searchDir.length () - 1, 0, '\0');
#endif

   strcpy (pEntry->pPath, searchDir.c_str ());

#ifdef UNIX
   pEntry->pEndPath = pEntry->pPath + searchDir.length ();
   return find ();
#else
   #ifdef WINDOWS
   FileRegularExpr regExp (searchFile.c_str ());
   assert (!regExp.checkIntegrity ());

   return ((pEntry->dwFileAttributes & attr) || !regExp.matches (pEntry->name ()))
          ? find () : 0;
   #else
      #error Not implemented!
   #endif
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the next file which matches the search-criteria
//Returns   : Status; 0: OK
//Requires  : pResult != NULL; searchDir already set
/*--------------------------------------------------------------------------*/
int DirectorySearch::find () {
   assert (!checkIntegrity ());
   FileRegularExpr regExp (searchFile.c_str ());
   assert (!regExp.checkIntegrity ());

#ifdef UNIX
   assert (pDir);

   struct dirent* pDirEnt;
   while ((pDirEnt = readdir (pDir)) != NULL) {            // Files available?
     if ((!(attr & FILE_HIDDEN)) && (*pDirEnt->d_name == '.'))
        continue;

     if (regExp.matches (pDirEnt->d_name)) {
         strcpy (pEntry->pEndPath, pDirEnt->d_name);
         stat (pEntry->pPath, &pEntry->status);
         pEntry->userExec = !access (pEntry->pPath, X_OK);
         *pEntry->pEndPath = '\0';

         // Do attributes match?
         short access (pEntry->status.st_mode & FILE_NORMAL);
         short type (pEntry->status.st_mode & ~FILE_NORMAL);
         if (!(access & attr)
	     || (type & attr)) {
            pEntry->entry = *pDirEnt;
            return 0;
         } // endif attributs OK
   } // endif filename OK
   } // end-while files available

   return ENOENT;
#else
   #ifdef WINDOWS
   while (FindNextFile (hSearch, pEntry))
      if (!(pEntry->dwFileAttributes & attr) && regExp.matches (pEntry->name ()))
         return 0;
   return GetLastError ();
   #else
      #error Not implemented yet!
   #endif
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the status of the object
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int DirectorySearch::checkIntegrity () const {
   assert (!searchDir.empty ());
   if (pEntry) {
      assert (pEntry->pPath);
#ifdef UNIX
      assert (pEntry->pEndPath);
#endif
   }

#ifdef UNIX
   assert (!searchFile.empty ());
#endif

   return 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Splits the search-string in its directory- and its filepart
//Parameters: search: Files to find
/*--------------------------------------------------------------------------*/
void DirectorySearch::setFile (const std::string& search) {
   assert (!search.empty ());

   searchFile = search;

   unsigned int len (search.length ());
   if (searchFile[len - 1] == DIR_SPLIT)
      searchFile.replace (--len, 1, 0, '\0');

   while (len--) {
      if (search[len] == DIR_SPLIT) {
         searchDir = search;
         searchDir.replace (len + 1, searchDir.length (), 0, '\0');
         searchFile.replace (0, len + 1, 0, '\0');
	 assert (!checkIntegrity ());
         return;
      } // endif
   } // end-while

   searchDir = "./";
   assert (!checkIntegrity ());
}

#ifdef WINDOWS
/*--------------------------------------------------------------------------*/
//Purpose   : Tests, if the file is executeable. This is decided on the file-
//            extension: EXE-, COM- and BAT-files are considered executeable
//Returns   : Flag, if file is executeable
//Remarks   : Other also executeable files (like 4DOS BTM-files; I think
//            there's even another WINDOZE-format) are not considered as exes.
/*--------------------------------------------------------------------------*/
bool dirEntry::isExecuteable () const {
   const char* pEnd (strrchr (cFileName, '.'));
   if (pEnd++)
      // Check the next 4 bytes; ignore end-of-string, the buffer is long
      // enough (MAX_PATH) and the trailing \0 is in the compare
      switch (toupper (*pEnd++) + (toupper (*pEnd++) << 8)
              + (toupper (*pEnd++) << 16) + (toupper (*pEnd) << 24)) {
      case 'EXE\0' :
      case 'COM\0' :
      case 'BAT\0' :
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
