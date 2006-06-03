//$Id: Check.cpp,v 1.21 2006/06/03 21:31:53 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Check
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.21 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.9.1999
//COPYRIGHT   : Copyright (C) 1999 - 2006

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

#include <cstdio>

#ifdef HAVE_DLFCN_H
#  include <dlfcn.h>
#endif

#include <iostream>

#include "YGP/Check.h"

extern "C" {

#if SYSTEM == UNIX
#  ifdef HAVE_GTK
#    include <gtk/gtkmessagedialog.h>

   typedef gboolean (*PFNINIT)(int *argc, char ***argv);
   typedef GtkWidget* (*PFNNEWMSGDLG)(GtkWindow *parent, GtkDialogFlags flags,
				      GtkMessageType type, GtkButtonsType buttons,
				      const gchar *message_format, ...);
   typedef void (*PFNSETTITLE)(GtkWindow *window, const gchar *title);
   typedef void (*PFNSETSIZE)(GtkWindow *window, gint width, gint height);
   typedef int (*PFNRUNDLG)(GtkDialog*);
   typedef void (*PFNDESTROY)(GtkWidget*);

   static bool show (const char* expr, const char* title) {
#    ifdef HAVE_DLFCN_H
      static void* hDLL = NULL;
      static bool gtkOK (false);

      if (!hDLL)
	 hDLL = dlopen ("libgtk-x11-2.0.so", RTLD_LAZY);

      if (hDLL && !gtkOK) {
	 PFNINIT pfnInit ((PFNINIT)dlsym (hDLL, "gtk_init_check"));
         if (pfnInit)
	    gtkOK = pfnInit (NULL, NULL);
      }

      if (gtkOK) {
	 PFNNEWMSGDLG pfnNewDlg ((PFNNEWMSGDLG)dlsym (hDLL, "gtk_message_dialog_new"));
	 PFNSETTITLE pfnSetTitle ((PFNSETTITLE)dlsym (hDLL, "gtk_window_set_title"));
	 PFNSETSIZE pfnSetSize ((PFNSETSIZE)dlsym (hDLL, "gtk_window_set_default_size"));
	 PFNRUNDLG pfnRunDlg ((PFNRUNDLG)dlsym (hDLL, "gtk_dialog_run"));
	 PFNDESTROY pfnDestroy ((PFNDESTROY)dlsym (hDLL, "gtk_widget_destroy"));

	 if (pfnNewDlg && pfnSetTitle && pfnSetSize && pfnRunDlg && pfnDestroy) {
	    GtkWidget* mbox (pfnNewDlg (NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR, GTK_BUTTONS_OK_CANCEL,
					expr));
	    pfnSetTitle ((GtkWindow*)mbox, title);
	    pfnSetSize ((GtkWindow*)mbox, 300, -1);
	    gint rc (pfnRunDlg ((GtkDialog*)mbox));
	    pfnDestroy (mbox);
	    return rc != GTK_RESPONSE_OK;
	 }
      }
#    endif
#  else
      static bool show (const char* expr, const char*) {
#  endif
         std::cerr << "Check failed! Continue y/n? ";
         char ch;
         std::cin >> ch;
         return (ch != 'y') && (ch != 'Y'); }
#elif SYSTEM == WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

   inline bool show (const char* expr, const char* title) {
      return MessageBox (NULL, expr, title,
                         MB_OKCANCEL | MB_ICONERROR | MB_TASKMODAL) != IDOK; }

#endif


int check (const char* expr, const char* file, unsigned int line) {
#if defined (__BORLANDC__) || defined (_MSC_VER)
   // Arrays with dynamic lengths don't exist in some parts of the world
   char title[MAX_PATH + 40];
#else
   char title[strlen (file) + 40];
#endif
   snprintf (title, sizeof (title), "Check in %s, line %u", file, line);
   std::cerr << title << ": " << expr << '\n';
   if (show (expr, title)) {
      std::cerr << "\t-> Canceled\n";
      exit (-1);
   }
   std::cerr << "\t-> Continue\n";
   return 0;
}

}
