//$Id: IDirSrch.cpp,v 1.4 2002/05/24 06:52:49 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : DirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.8.2001
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


#include <gzo-cfg.h>

#include "File.h"
#include "Trace_.h"
#include "IDirSrch.h"


#if SYSTEM == UNIX
#  if HAVE_DIRENT_H
#     include <dirent.h>
#     define NAMLEN(dirent) strlen((dirent)->d_name)
#  else
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

static const int FILE_NORMAL_    = (S_IFREG   | S_IFLNK | S_ISUID | S_ISGID
                                    | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO);
static const int FILE_READONLY_  = (S_IFREG   | S_IFLNK | S_ISUID | S_ISGID
                                    | S_ISVTX | S_IRUSR | S_IRGRP | S_IROTH
                                    | S_IXUSR | S_IXGRP | S_IXOTH);
static const int FILE_DIRECTORY_ = (S_IFDIR | S_ISUID | S_ISGID | S_ISVTX
                                    | S_IRWXU | S_IRWXG | S_IRWXO);
static const int FILE_HIDDEN_    = (1 << (sizeof (long) * 8 - 1));

#elif SYSTEM == WINDOWS

#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

static const int FILE_NORMAL_    = ~FILE_ATTRIBUTE_DIRECTORY;
static const int FILE_READONLY_  = FILE_ATTRIBUTE_READONLY;
static const int FILE_DIRECTORY_ = FILE_ATTRIBUTE_DIRECTORY;
static const int FILE_HIDDEN_    = FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN;
#else
#  error Not implemented yet!
#endif

#define ADDSYSATTRIB(result, sample, flag) if (sample & IDirectorySearch::flag)\
                                              result |= flag##_;



const int IDirectorySearch::FILE_NORMAL    = 1;
const int IDirectorySearch::FILE_READONLY  = IDirectorySearch::FILE_NORMAL << 1;
const int IDirectorySearch::FILE_DIRECTORY = IDirectorySearch::FILE_READONLY << 1;
const int IDirectorySearch::FILE_HIDDEN    = IDirectorySearch::FILE_DIRECTORY << 1;


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
IDirectorySearch::~IDirectorySearch () {
   delete pEntry;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Converts attributes (constants according to IDirectorySearch)
//            into the values used by the system
//Parameters: attribs: Attributes to convert
//Returns   : unsigned long: Systemattributes
/*--------------------------------------------------------------------------*/
unsigned long IDirectorySearch::convertToSysAttribs (unsigned long attribs) {
   TRACE3 ("IDirectorySearch::convertToSysAttribs (unsigned long) - "
           << hex << attribs);
   unsigned long attr = 0;

   ADDSYSATTRIB (attr, attribs, FILE_NORMAL);
   ADDSYSATTRIB (attr, attribs, FILE_READONLY);
   ADDSYSATTRIB (attr, attribs, FILE_DIRECTORY);
   ADDSYSATTRIB (attr, attribs, FILE_HIDDEN);

   TRACE3 ("IDirectorySearch::convertToSysAttribs (unsigned long) - System: "
           << hex << attr);
   return attr;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts attributes (constants according to IDirectorySearch)
//            into the values used by the system
//Parameters: attribs: Attributes to convert
//Returns   : unsigned long: Systemattributes
/*--------------------------------------------------------------------------*/
unsigned long IDirectorySearch::convertFromSysAttribs (unsigned long attribs) {
   TRACE3 ("IDirectorySearch::convertFromSysAttribs (unsigned long) - "
           << hex << attribs);
   unsigned long attr = 0;

#if SYSTEM == UNIX
   attr |= (attribs & S_IFDIR) ? FILE_DIRECTORY : FILE_NORMAL;
   if ((attribs & (S_IWUSR | S_IWGRP | S_IWOTH)) == 0)
      attr |= FILE_READONLY;
#elif SYSTEM == WINDOWS
   attr |= (attribs & FILE_ATTRIBUTE_DIRECTORY) ? FILE_DIRECTORY : FILE_NORMAL;
   if (attribs & FILE_ATTRIBUTE_HIDDEN)
      attr |= FILE_HIDDEN;
   if (attribs & FILE_ATTRIBUTE_READONLY)
      attr |= FILE_READONLY;
#else
#  error Not implemented yet!
#endif

   TRACE3 ("IDirectorySearch::convertFromSysAttribs (unsigned long) - "
           << hex << attr);
   return attr;
}
