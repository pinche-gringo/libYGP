// $Id: Version.cpp,v 1.17 2008/03/30 13:39:17 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : Version
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.17 $
//AUTHOR      : Markus Schwab
//CREATED     : 10.9.1999
//COPYRIGHT   : Copyright (C) 1999 - 2004, 2006, 2008

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include <YGP/Internal.h>

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
     - Labels displaying "attributes" (e.g. numbers, dates, ...) in a formatted
       way.
     - Entry fields their text formatted  and checking the input for validity.
     - A listbox, designed to display files (with an icon according to the
       type).

   and some classes for dialogs.
*/

/**\namespace XGP

   This namespace holds classes related to the X-windows system (basing on
   GTKMM-2/GKT+-2).
 */

static const char* Version = "\n!@$%" LIBYGP_NAME " V" PACKAGE_VERSION
                             " Compiled on " __DATE__ "%$@!\n";

// Function to initialize various global data
static const int _init () {
   bindtextdomain (LIBYGP_NAME, LOCALEDIR);
   return Version != (const void*)0;
}

static const int init = _init ();
