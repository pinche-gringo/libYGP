//$Id: GTKViewer.c,v 1.1 2003/10/17 06:33:20 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : GTKViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.10.2003
//COPYRIGHT   : Anticopyright (A) 2003

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

#ifdef HAVE_GTKHTML

#include <stdio.h>

#include <dlfcn.h>

#include "GTKViewer.h"

#include <libgtkhtml/gtkhtml.h>



static void* hDLL = NULL;


typedef GtkWidget* (*PFNNEWHTMLVIEW)(void);


//----------------------------------------------------------------------------
// Creates a GtkWidget to display a HTML code
// \returns GtkWidget*: Pointer to the created widget
//----------------------------------------------------------------------------
GtkWidget* gtkhtmlGetWidget () {
   hDLL = dlopen ("libgtkhtml-2.so", 0x00001);
   if (hDLL) {
      PFNNEWHTMLVIEW pFnc = (PFNNEWHTMLVIEW)dlsym (hDLL, "html_view_new");
      if (pFnc)
         return pFnc ();
   }
   return NULL;
}

//----------------------------------------------------------------------------
// Displays a file in the GTKHTML control
//----------------------------------------------------------------------------
int gtkhtmlDisplayFile (GtkWidget* ctrl, const char* file) {
   typedef HtmlDocument* (*PFNNEWDOCUMENT)();
   typedef gboolean (*PFNDOCUMENTOPEN)(HtmlDocument*, const gchar*);
   typedef void (*PFNSETDOCUMENT)(HtmlView*, HtmlDocument*);
   typedef void (*PFNWRITEDOCUMENT) (HtmlDocument*, const gchar*, gint len);
   typedef void (*PFNCLOSEDOCUMENT) (HtmlDocument*);

   FILE* pFile = fopen (file, "r");
   if (!pFile)
      return 1;

   PFNNEWDOCUMENT pfnNewDoc = (PFNNEWDOCUMENT)dlsym (hDLL, "html_document_new");
   PFNDOCUMENTOPEN pfnOpenDoc = (PFNDOCUMENTOPEN)dlsym (hDLL, "html_document_open_stream");
   PFNSETDOCUMENT pfnSetDoc = (PFNSETDOCUMENT)dlsym (hDLL, "html_view_set_document");
   PFNWRITEDOCUMENT pfnWriteDoc = (PFNWRITEDOCUMENT)dlsym (hDLL, "html_document_write_stream");
   PFNCLOSEDOCUMENT pfnCloseDoc = (PFNCLOSEDOCUMENT)dlsym (hDLL, "html_document_close_stream");

   if (!(pfnNewDoc && pfnOpenDoc && pfnSetDoc && pfnWriteDoc && pfnCloseDoc))
      return -1;

   puts ("Creating document");
   HtmlDocument* doc = pfnNewDoc ();
   puts ("Opening document");
   if (pfnOpenDoc (doc, "text/html")) {
      char buffer[4096];
      int  i;
      while ((i = fread (buffer, 1, sizeof (buffer), pFile)) > 0)
         pfnWriteDoc (doc, buffer, i);
      pfnCloseDoc (doc);
   }
   else
      return -2;

   puts ("Setting document");
   pfnSetDoc ((HtmlView*)ctrl, doc);
   return 0;
}

//----------------------------------------------------------------------------
// Returns the error of the last action (concerning accessing the DLL)
//----------------------------------------------------------------------------
const char* gtkhtmlGetError () {
    return dlerror ();
}

//----------------------------------------------------------------------------
// Destroys the passed control
//----------------------------------------------------------------------------
void gtkhtmlDeleteWidget (GtkWidget* ctrl) {
    gtk_widget_destroy (ctrl);
    dlclose (hDLL);
    hDLL = NULL;
}


#endif
