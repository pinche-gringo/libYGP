// $Id: Version.cpp,v 1.19 2004/12/29 18:22:49 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Version
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.19 $
//AUTHOR      : Markus Schwab
//CREATED     : 10.9.1999
//COPYRIGHT   : Copyright (C) 1999 - 2004

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

#include "YGP/Internal.h"


/**\mainpage Overview of the %YGP library

   \section intro Introduction
   %YGP stands for Yet another General Purpose library and that is what it is
   (and also says alot about my creativity finding "cool" names).

   Those clases are designed to be portable (and maybe even usefull).

   They are distributed under the GNU General Public License; in the hope that
   it will be useful, but WITHOUT ANY WARRANTY; without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   file COPYING for more details.

   \section features Features
   The highlights are

     - A general parser (parsing objects)
     - A regular expression class
     - Classes to relate objects

   and the usual stuff like smart pointers, communication over a network,
   executing other processes or threads and handle/format diverse attributes.
*/

/**\namespace YGP

   This namespace holds plattform independend classes of common use.
 */

static const char* Version = "\n!@$%" PACKAGE " V" VERSION
                             " Compiled on " __DATE__ "%$@!\n";

// Function to initialize various global data
static const int _init () {
   bindtextdomain (PACKAGE, LOCALEDIR);
   return Version == (const void*)0;
}


static const int init = _init ();


#ifndef HAVE_SYSLOG_H
#  include <map>
#  include <YGP/Log.h>

std::map <unsigned int, char*> YGP::Syslog::apAppl;
#endif


#if SYSTEM == WINDOWS

#  ifdef _MSC_VER
#     pragma warning(disable:4786) // Disable warning about truncating debug info
#  endif

#  include <map>
#  include <YGP/Log.h>

#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>


extern "C" {

/*--------------------------------------------------------------------------*/
//Purpose   : DLL-entry point
//Parameters: HANDLE hDLL: Handle to DLL
//            DWORD reason: Reason for calling (attach/detach to/from
//            process/thread
//            LPVOID reserved
/*--------------------------------------------------------------------------*/
int WINAPI dllEntry (HANDLE, DWORD, LPVOID) {
   return true;
}

} // end extern "C"

#endif
