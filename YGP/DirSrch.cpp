//$Id: DirSrch.cpp,v 1.14 2000/02/06 22:11:35 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : DirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.14 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.7.1999
//COPYRIGHT   : Anticopyright (A) 1999

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


#ifdef WINDOWS
#include <ctype.h>
#include <string.h>
#endif

#include <errno.h>

#define DEBUG 0
#include "Trace_.h"
#include "DirSrch.h"
#include "FileRExp.h"

#ifdef UNIX
#include <unistd.h>
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
//Parameters: result: Buffer where to place the result
//Returns   : Status; 0: OK
//Requires  : pResult != NULL; searchDir already set
/*--------------------------------------------------------------------------*/
int DirectorySearch::find (dirEntry& result, unsigned long attribs) {
   cleanup ();
   pEntry = &result;
   assert (!checkIntegrity ());

   TRACE5 ("DirectorySearch::find " << searchDir << searchFile);

#ifndef WINDOWS
   attr = attribs;
#endif

#ifdef UNIX
   pDir = opendir (searchDir.c_str ());
   if (!pDir)
      return errno;
#else
#   ifdef WINDOWS
   // Attribut-handling: Files having attrs not specified here are not ret.
   attr = ~attribs;

   std::string temp (searchDir + '*');
   hSearch = FindFirstFile (temp.c_str (), pEntry);
   if (hSearch == INVALID_HANDLE_VALUE)
      return GetLastError ();
#  else
#     error Not implemented!
#  endif
#endif

   strcpy (pEntry->pPath, searchDir.c_str ());

#ifdef UNIX
   pEntry->pEndPath = pEntry->pPath + searchDir.length ();
   return find ();
#else
#  ifdef WINDOWS
   TRACE9 ("DirectorySearch::find - found " << pEntry->name ());

   FileRegularExpr regExp (searchFile.c_str ());
   assert (!regExp.checkIntegrity ());

   return ((pEntry->dwFileAttributes & attr) || !regExp.matches (pEntry->name ()))
          ? find () : 0;
#  else
#     error Not implemented!
#  endif
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the next file which matches the search-criteria
//Returns   : Status; 0: OK
//Requires  : searchDir, pEntry  already set
/*--------------------------------------------------------------------------*/
int DirectorySearch::find () {
   assert (!checkIntegrity ());
   FileRegularExpr regExp (searchFile.c_str ());
   assert (!regExp.checkIntegrity ());

#ifdef UNIX
   assert (pDir);

   struct dirent* pDirEnt;
   while ((pDirEnt = readdir (pDir)) != NULL) {            // Files available?
      TRACE9 ("DirectorySearch::find - found " << pDirEnt->d_name);

      if ((!(attr & FILE_HIDDEN)) && (*pDirEnt->d_name == '.'))
         continue;

      if (regExp.matches (pDirEnt->d_name)) {
         strcpy (pEntry->pEndPath, pDirEnt->d_name);
         stat (pEntry->pPath, &pEntry->status);
         pEntry->userExec = !access (pEntry->pPath, X_OK);
         *pEntry->pEndPath = '\0';

         // Do attributes match?
         unsigned short access (pEntry->status.st_mode & FILE_NORMAL);
         unsigned short type (pEntry->status.st_mode & ~FILE_NORMAL);
         if (((access & attr) == access) || (type & attr)) {
            pEntry->entry = *pDirEnt;
            TRACE5 ("DirectorySearch::find - match " << pEntry->name ());
            return 0;
         } // endif attributs OK
      } // endif filename OK
   } // end-while files available

   return ENOENT;
#else
#  ifdef WINDOWS
   while (FindNextFile (hSearch, pEntry))
      if (!(pEntry->dwFileAttributes & attr) && regExp.matches (pEntry->name ())) {
         TRACE5 ("DirectorySearch::find - match " << pEntry->name ());
         return 0;
      }
   return GetLastError ();
#  else
#      error Not implemented yet!
#   endif
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the status of the object
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int DirectorySearch::checkIntegrity () const {
      return searchDir.empty () ? NO_DIR : searchFile.empty () ? NO_FILE :
	                                  pEntry ?
#ifdef UNIX
                                          !pEntry->pEndPath ? NO_ENTRY_ENDPATH :
#endif
                                          !pEntry->pPath : NO_ENTRY_PATH;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Splits the search-string in its directory- and its filepart; it
//            prepares the internal data also for a new search
//Parameters: search: Files to find
/*--------------------------------------------------------------------------*/
void DirectorySearch::setFile (const std::string& search) {
   assert (!search.empty ());

   searchFile = search;

   unsigned int len (search.length ());
   if (searchFile[len - 1] == getSplitChar ())
      searchFile.replace (--len, 1, 0, '\0');

   while (len--) {
      if (search[len] == getSplitChar ()) {
         searchDir = search;
         searchDir.replace (len + 1, searchDir.length (), 0, '\0');
         searchFile.replace (0, len + 1, 0, '\0');
         assert (checkIntegrity () <= NO_ENTRY);
         return;
      } // endif
   } // end-while

#ifdef UNIX
   searchDir = "./";
#else
   searchDir = ".\\";
#endif
   assert (checkIntegrity () <= NO_ENTRY);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Cleanup of data
/*--------------------------------------------------------------------------*/
void DirectorySearch::cleanup () {
#ifdef UNIX
#  ifdef CLOSEDIR_VOID
   int rc (closedir (pDir)); assert (!rc);
#  else
   closedir (pDir);
#  endif
   pDir = NULL;
#else
#  ifdef WINDOWS
   FindClose (hSearch);
   hSearch = INVALID_HANDLE_VALUE;
#  else
#     error Not implemented yet!
#  endif
#endif
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
   const char* pEnd = strrchr (cFileName, '.');
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
