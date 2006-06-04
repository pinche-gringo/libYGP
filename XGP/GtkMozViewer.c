//$Id: GtkMozViewer.c,v 1.2 2006/06/04 04:10:56 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : GtkMozEmbedViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
//AUTHOR      : Markus Schwab
//CREATED     : 24.7.2005
//COPYRIGHT   : Copyright (C) 2005, 2006

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

#ifdef HAVE_GTKHTML

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
/// \returns GtkWidget*: Created widget
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
/// \param ctrl: MozEmbed-widget
/// \param file: File to display
/// \remarks: Don't call with a NULL-pointer for file
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
/// \return const char*: A text describing the last error
//----------------------------------------------------------------------------
const char* gtkMozEmbedGetError () {
    return dlerror ();
}

#endif
