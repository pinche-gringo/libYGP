// $Id: Version.cpp,v 1.7 2003/07/20 02:21:20 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Version
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.7 $
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

/**\mainpage Overview of the XGP library

   \section intro Introduction
   XGP stands for yet another X-windows General Purpose library (matching its
   counterpart YGP) and provides some classes using the X windows system.

   This classes base on the GTKMM toolkit (which itself bases on GTK).

   They are distributed under the GNU General Public License; in the hope that
   it will be useful, but WITHOUT ANY WARRANTY; without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   file COPYING for more details.

   \section features Features
   The highlights are

     - An application framework (mainly providing easier menu creation, help
       menu handling and an about box).
     - Labels displaying (e.g. numbers, dates, ...)
     - Entry fields their text formatted  and checking the input for validity.
     - A listbox, designed to display files (with an icon according to the
       type).

   and some classes for dialogs.
*/

// Function to initialize various global data
static const int _init () {
   bindtextdomain (PACKAGE, LOCALEDIR);
   return 0;
}


static const char* Version = "\n!@$%" PACKAGE " V" VERSION
                             " Compiled on " __DATE__ "%$@!\n";
static const int init = _init ();
