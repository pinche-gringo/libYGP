// $Id: Version.cpp,v 1.7 2003/03/06 04:16:02 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Version
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.7 $
//AUTHOR      : Markus Schwab
//CREATED     : 10.9.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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

#include "Internal.h"


// Function to initialize various global data
static const int _init () {
   bindtextdomain (PACKAGE, LOCALEDIR);
   return 0;
}


static const char* Version = "\n!@$%" PACKAGE " V" VERSION
                             " Compiled on " __DATE__ "%$@!\n";
static const int init = _init ();


#if SYSTEM == WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
