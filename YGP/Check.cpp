//$Id: Check.cpp,v 1.6 2002/11/18 04:37:10 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Check
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.9.1999
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

#include <gzo-cfg.h>

#include <stdio.h>

#include <iostream>

#include "Check.h"


#if SYSTEM == UNIX
#  ifdef HAVE_GTKMM
#     include <dlfcn.h>
#     include <gtk--/main.h>
#     include <../X-windows/XMessageBox.h>

      typedef int (*PFNCMSGBOX)(const char*, const char*, int, unsigned int);

      inline bool show (const char* expr, const char* title) {
         Main* appl (NULL);
         if (!Main::instance ()) {
            g_thread_init (NULL);
            appl = new Main (0, NULL);
         }

         void* hDLL (dlopen ("libXAppl.so", 0x00001));
         if (hDLL) {
            PFNCMSGBOX pFnc ((PFNCMSGBOX)dlsym (hDLL, "showMessageBox"));
            if (pFnc) {
               gdk_threads_enter ();
               bool rc (pFnc (expr, title,
                              XMessageBox::CRITICAL | XMessageBox::OKCANCEL, 0)
                        != XMessageBox::OK);
               gdk_threads_leave ();
               delete appl;
               dlclose (hDLL);
               return rc;
            }
         }
         cerr << PACKAGE "-error: Couldn't display message box! " << dlerror () << '\n';
#  else
      inline bool show (const char* expr, const char*) {
#  endif
         cerr << "Assertion failed! Continue y/n? ";
         char ch;
         cin >> ch;
         return (ch != 'y') && (ch != 'Y'); }
#elif SYSTEM == WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

   inline bool show (const char* expr, const char* title) {
      return MessageBox (NULL, expr, title,
                         MB_OKCANCEL | MB_ICONERROR | MB_TASKMODAL) != IDOK; }

#endif


int check (const char* expr, const char* file, unsigned int line) {
#ifdef _MSC_VER
   // Arrays with dynamic lengths don't exist in some parts of the world
   char title[MAX_PATH + 40];
#else
   char title[strlen (file) + 40];
#endif
   sprintf (title, "Check in %s, line %u", file, line);
   std::cerr << title << ": " << expr << '\n';
   if (show (expr, title)) {
      std::cerr << "\t-> Canceled\n";
      exit (-1);
   }
   std::cerr << "\t-> Continue\n";
   return 0;
}
