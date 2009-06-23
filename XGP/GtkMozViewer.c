//$Id: GtkMozViewer.c,v 1.5 2008/06/08 12:42:40 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : GtkMozEmbedViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
//AUTHOR      : Markus Schwab
//CREATED     : 24.7.2005
//COPYRIGHT   : Copyright (C) 2005 - 2008

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


#include <ygp-cfg.h>

#ifdef HAVE_GTKMOZEMBED

#include <dlfcn.h>

#include <YGP/Check.h>

#include <gtkmozembed.h>

#include "GtkMozViewer.h"


#undef TRACE
#undef TRACE2
#if TRACELEVEL > 0
#  define TRACE(x)     printf (x); fflush (stdin);
#  define TRACE2(x, y) printf (x, y); fflush (stdin);
#else
#  define TRACE(x)
#  define TRACE2(x, y)
#endif

static void* hDLL = NULL;

typedef GtkWidget* (*PFNNEWMOZEMBED)(void);
typedef void (*PFNLOAD)(GtkMozEmbed* widget, const char*);

static PFNNEWMOZEMBED pfnNew  = NULL;
static PFNLOAD        pfnLoad = NULL;


//----------------------------------------------------------------------------
/// Initializes the GktMozEmbed instance
/// \returns GtkWidget* Created widget
//----------------------------------------------------------------------------
GtkWidget* gtkMozEmbedInitialize () {
   TRACE ("Initializing gtkmozembed viewer\n");

   if (!hDLL)
      hDLL = dlopen ("libgtkembedmoz.so", 0x00001);

   if (hDLL) {
      TRACE ("Checking function pointers\n");
      if (!pfnNew) {
	 TRACE ("Getting function pointers\n");
         pfnNew = (PFNNEWMOZEMBED)dlsym (hDLL, "gtk_moz_embed_new");
         pfnLoad = (PFNLOAD)dlsym (hDLL, "gtk_moz_embed_load_url");

         if (!(pfnNew && pfnLoad))
            return NULL;
      }
      TRACE ("Creating control\n");
      return pfnNew ();
   }
   return NULL;
}

//----------------------------------------------------------------------------
/// Displays a URL in the GtkMozEmbed control
/// \param ctrl MozEmbed-widget
/// \param file File to display
/// \remarks Don't call with a NULL-pointer for file
//----------------------------------------------------------------------------
void gtkMozEmbedDisplayURL (GtkWidget* ctrl, const char* url) {
   TRACE2 ("gtkMozEmbedDisplayURL: Showing URL %s\n", url);
   Check2 (ctrl);
   Check2 (url);
   Check1 (pfnLoad);

   pfnLoad ((GtkMozEmbed*)ctrl, url);
}

//----------------------------------------------------------------------------
/// Returns the error of the last action (concerning accessing the DLL)
/// \return const char* A text describing the last error
//----------------------------------------------------------------------------
const char* gtkMozEmbedGetError () {
    return dlerror ();
}

#endif
