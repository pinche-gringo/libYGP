//$Id$

//PROJECT     : XGP
//SUBSYSTEM   : Webkit
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision$
//AUTHOR      : Markus Schwab
//CREATED     : 23.6.2009
//COPYRIGHT   : Copyright (C) 2009, 2010

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

#ifdef HAVE_WEBKIT

#include <dlfcn.h>

#include <YGP/Check.h>

#include <webkit/webkitwebview.h>

#include "WebkitViewer.h"


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

typedef GtkWidget* (*PFNNEWWEBKITVIEW)(void);
typedef void (*PFNLOAD)(GtkWidget* widget, const char*);

static PFNNEWWEBKITVIEW pfnNew  = NULL;
static PFNLOAD          pfnLoad = NULL;


//----------------------------------------------------------------------------
/// Initialises the Webkit widget
/// \returns GtkWidget* Created widget
//----------------------------------------------------------------------------
GtkWidget* initialiseWebkit () {
   TRACE ("Initialising Webkit viewer\n");
   if (!g_thread_get_initialized ())
      g_thread_init (NULL);

   if (!hDLL)
      hDLL = dlopen ("libwebkit-1.0" SHAREOBJ_EXT, 0x00001);

   if (hDLL) {
      TRACE ("Checking function pointers\n");
      if (!pfnNew) {
	 TRACE ("Getting function pointers\n");
         pfnNew = (PFNNEWWEBKITVIEW)dlsym (hDLL, "webkit_web_view_new");
         pfnLoad = (PFNLOAD)dlsym (hDLL, "webkit_web_view_open");

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
/// \param ctrl Webkit-widget
/// \param file File to display
/// \remarks Don't call with a NULL-pointer for file
//----------------------------------------------------------------------------
void webkitDisplayURL (GtkWidget* ctrl, const char* url) {
   TRACE2 ("webkitDisplayURL: Showing URL %s\n", url);
   Check2 (ctrl);
   Check2 (url);
   Check1 (pfnLoad);

   pfnLoad (ctrl, url);
}

//----------------------------------------------------------------------------
/// Returns the error of the last action (concerning accessing the DLL)
/// \return const char* A text describing the last error
//----------------------------------------------------------------------------
const char* webkitGetError () {
    return dlerror ();
}

#endif
