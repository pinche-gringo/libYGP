// $Id: Version.cpp,v 1.3 2002/04/24 03:05:51 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Version
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 10.9.1999
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

#include <Internal.h>

// Function to initialize various global data
static const int _init () {
   bindtextdomain (PACKAGE, LOCALEDIR);
}


static const char* Version = "\n!@$%" PACKAGE " V" VERSION "." MICRO_VERSION
                             " Compiled on " __DATE__ "%$@!\n";
static const int init = _init ();
