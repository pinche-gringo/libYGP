//$Id: GTKViewer.c,v 1.2 2003/10/19 00:00:26 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : GTKViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
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
#include <errno.h>
#include <malloc.h>
#include <string.h>

#include <dlfcn.h>

#include "GTKViewer.h"

#include <libgtkhtml/gtkhtml.h>

#undef TRACE
#undef TRACE2
#if TRACELEVEL > 0
#  define TRACE(x)     printf (x)
#  define TRACE2(x, y) printf (x, y)
#else
#  define TRACE(x)
#  define TRACE2(x, y)
#endif

static void* hDLL = NULL;

typedef GtkWidget* (*PFNNEWHTMLVIEW)(void);
typedef struct {
   GtkWidget*  ctrl;
   char* path; } GTKHTMLDATA;

static void gtkhtmlLinkClicked (HtmlDocument *doc, const gchar *url, GTKHTMLDATA* ctrl);


//----------------------------------------------------------------------------
// Initializes the GTKHTML instance
// \returns void*: Pointer to the created data
//----------------------------------------------------------------------------
void* gtkhtmlInitialize () {
   TRACE ("Initializing gtkhtml viewer\n");
   hDLL = dlopen ("libgtkhtml-2.so", 0x00001);
   if (hDLL) {
      PFNNEWHTMLVIEW pFnc = (PFNNEWHTMLVIEW)dlsym (hDLL, "html_view_new");
      if (pFnc) {
         GTKHTMLDATA* data = (GTKHTMLDATA*)malloc (sizeof (GTKHTMLDATA));
         data->ctrl = pFnc ();
         data->path = NULL;
         return data;
      }
   }
   return NULL;
}

//----------------------------------------------------------------------------
// Frees the memory used by the  GTKHTML data
// @param gtkData: Data of the call to gkthtmlIntialize
//----------------------------------------------------------------------------
void gtkhtmlFree (void* data) {
    if (data) {
       if (((GTKHTMLDATA*)data)->path)
          free (((GTKHTMLDATA*)data)->path);
       free (data);
   }
}

//----------------------------------------------------------------------------
// Creates a GtkWidget to display a HTML code
// @param gtkData: Data of the call to gkthtmlIntialize
// \returns GtkWidget*: Pointer to the created widget
//----------------------------------------------------------------------------
GtkWidget* gtkhtmlGetWidget (void* gtkData) {
   return ((GTKHTMLDATA*)gtkData)->ctrl;
}


//----------------------------------------------------------------------------
// Displays a file in the GTKHTML control
//----------------------------------------------------------------------------
int gtkhtmlDisplayFile (void* data, const char* file) {
   typedef HtmlDocument* (*PFNNEWDOCUMENT)();
   typedef gboolean (*PFNDOCUMENTOPEN)(HtmlDocument*, const gchar*);
   typedef void (*PFNSETDOCUMENT)(HtmlView*, HtmlDocument*);
   typedef void (*PFNWRITEDOCUMENT) (HtmlDocument*, const gchar*, gint len);
   typedef void (*PFNCLOSEDOCUMENT) (HtmlDocument*);

   PFNNEWDOCUMENT pfnNewDoc = (PFNNEWDOCUMENT)dlsym (hDLL, "html_document_new");
   PFNDOCUMENTOPEN pfnOpenDoc = (PFNDOCUMENTOPEN)dlsym (hDLL, "html_document_open_stream");
   PFNSETDOCUMENT pfnSetDoc = (PFNSETDOCUMENT)dlsym (hDLL, "html_view_set_document");
   PFNWRITEDOCUMENT pfnWriteDoc = (PFNWRITEDOCUMENT)dlsym (hDLL, "html_document_write_stream");
   PFNCLOSEDOCUMENT pfnCloseDoc = (PFNCLOSEDOCUMENT)dlsym (hDLL, "html_document_close_stream");

   if (!(pfnNewDoc && pfnOpenDoc && pfnSetDoc && pfnWriteDoc && pfnCloseDoc))
      return -1;

   TRACE ("Creating document\n");
   HtmlDocument* doc = pfnNewDoc ();
   GtkWidget* ctrl = ((GTKHTMLDATA*)data)->ctrl;

   g_signal_connect (G_OBJECT (doc), "link_clicked",
                     G_CALLBACK (gtkhtmlLinkClicked), data);

   TRACE ("Opening the document\n");
   if (pfnOpenDoc (doc, "text/html")) {
      // Store the path of the file
      if (!strncmp (file, "file://", 7))
         file += 7;

      char* oldpath = ((GTKHTMLDATA*)data)->path ? ((GTKHTMLDATA*)data)->path : "";
      char* newpath = (char*)malloc (strlen (oldpath)
                                     + ((*file != '/') ? strlen (file) + 2 : 2));
      strcpy (newpath, oldpath);
      int len = strlen (newpath);
      if (len && (newpath[len - 1] != '/'))
         newpath[len++] = '/';
      strcpy (newpath + len, file);

      TRACE2 ("Reading: `%s'\n", newpath);
      FILE* pFile = fopen (newpath, "r");
      if (!pFile) {
         GtkWidget* dlg = gtk_message_dialog_new 
             (GTK_WINDOW (gtk_widget_get_ancestor (ctrl, GTK_TYPE_WINDOW)),
              GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
              GTK_BUTTONS_OK, (const gchar*)_("Error loading file `%s': %s"),
              (const gchar*)newpath, (const gchar*)g_strerror (errno));
         gtk_dialog_run (GTK_DIALOG (dlg));
         gtk_widget_destroy (GTK_WIDGET (dlg));
         free (newpath);
         return 1;
      }
      if (((GTKHTMLDATA*)data)->path)
         free (((GTKHTMLDATA*)data)->path);
      ((GTKHTMLDATA*)data)->path = newpath;
      newpath = strchr (newpath, '/');
      if (newpath)
         *++newpath = '\0';
      else
         *((GTKHTMLDATA*)data)->path = '\0';

      // Stream in the file
      char buffer[4096];
      int  i;
      while ((i = fread (buffer, 1, sizeof (buffer), pFile)) > 0)
         pfnWriteDoc (doc, buffer, i);
      pfnCloseDoc (doc);
   }
   else
      return -2;

   TRACE ("Setting document\n");
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
void gtkhtmlLinkClicked (HtmlDocument *doc, const gchar *url, GTKHTMLDATA* data) {
    TRACE2 ("Link: %s\n", url);
    gtkhtmlDisplayFile (data, url);
}


#endif
