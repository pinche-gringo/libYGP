//$Id: IDirSrch.cpp,v 1.15 2004/10/14 04:00:06 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : IDirectorySearch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.15 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2004

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

#include "YGP/File.h"
#include "YGP/Trace.h"
#include "YGP/IDirSrch.h"


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

#     ifndef S_IFLNK
#        define S_IFLNK     0
#     endif
#     ifndef S_ISUID
#        define S_ISUID     0
#     endif
#     ifndef S_ISGID
#        define S_ISGID     0
#     endif
#     ifndef S_ISVTX
#        define S_ISVTX     0
#     endif
#     ifndef S_IRWXG
#        define S_IRWXG     (((S_IREAD) | (S_IWRITE) | (S_IEXEC)) >> 3)
#     endif
#     ifndef S_IRWXO
#        define S_IRWXO     (((S_IREAD) | (S_IWRITE) | (S_IEXEC)) >> 6)
#     endif
#     ifndef S_IRGRP
#        define S_IRGRP     ((S_IREAD) >> 3)
#     endif
#     ifndef S_IROTH
#        define S_IROTH     ((S_IREAD) >> 6)
#     endif
#     ifndef S_IWGRP
#        define S_IWGRP     ((S_IWRITE) >> 3)
#     endif
#     ifndef S_IWOTH
#        define S_IWOTH     ((S_IWRITE) >> 6)
#     endif
#  endif

static const int FILE_NORMAL_    = ((S_IFREG)  |(S_IFLNK)|(S_ISUID)|(S_ISGID)
                                    | (S_ISVTX)|(S_IRWXU)|(S_IRWXG)|(S_IRWXO));
static const int FILE_READONLY_  = ((S_IFREG)  |(S_IFLNK)|(S_ISUID)|(S_ISGID)
                                    | (S_ISVTX)|(S_IRUSR)|(S_IRGRP)|(S_IROTH)
                                    | (S_IXUSR)|(S_IXGRP)|(S_IXOTH));
static const int FILE_DIRECTORY_ = ((S_IFDIR)  |(S_ISUID)|(S_ISGID)|(S_ISVTX)
                                    | (S_IRWXU)|(S_IRWXG)|(S_IRWXO));
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


namespace YGP {

#define ADDSYSATTRIB(result, sample, flag) if (sample & IDirectorySearch::flag)\
                                              result |= flag##_;



const int IDirectorySearch::FILE_NORMAL    = 1;
const int IDirectorySearch::FILE_READONLY  = 2;
const int IDirectorySearch::FILE_DIRECTORY = 4;
const int IDirectorySearch::FILE_HIDDEN    = 8;


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
IDirectorySearch::~IDirectorySearch () {
   delete pEntry;
}


//-----------------------------------------------------------------------------
/// Converts attributes according to IDirectorySearch (as defined at the top)
/// into values used by the system.
/// \param attribs: Attributes to convert
/// \returns <tt>unsigned long</tt>: Systemattributes
//-----------------------------------------------------------------------------
unsigned long IDirectorySearch::convertToSysAttribs (unsigned long attribs) {
   TRACE3 ("IDirectorySearch::convertToSysAttribs (unsigned long) - "
           << std::hex << attribs);
   unsigned long attr = 0;

   ADDSYSATTRIB (attr, attribs, FILE_NORMAL);
   ADDSYSATTRIB (attr, attribs, FILE_READONLY);
   ADDSYSATTRIB (attr, attribs, FILE_DIRECTORY);
   ADDSYSATTRIB (attr, attribs, FILE_HIDDEN);

   TRACE3 ("IDirectorySearch::convertToSysAttribs (unsigned long) - System: "
           << std::hex << attr);
   return attr;
}

//-----------------------------------------------------------------------------
/// Converts the attributes as used by the system into attributes used by
/// IDirectorySearch (as defined at the top).
/// \param attribs: Attributes to convert
/// \returns <tt>unsigned long</tt>: Attributes according to IDirectorySearch
//-----------------------------------------------------------------------------
unsigned long IDirectorySearch::convertFromSysAttribs (unsigned long attribs) {
   TRACE3 ("IDirectorySearch::convertFromSysAttribs (unsigned long) - "
           << std::hex << attribs);
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
           << std::hex << attr);
   return attr;
}

}
