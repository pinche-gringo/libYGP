//$Id: GTKViewer.c,v 1.9 2003/12/22 22:03:15 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : GTKViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.9 $
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


#include <YGP/Internal.h>

#ifdef HAVE_GTKHTML

#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>

#include <dlfcn.h>

#include "GTKViewer.h"

#include <libgtkhtml/gtkhtml.h>

#define TRACELEVEL 1
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

typedef struct _FetchContext {
   HtmlStream* stream;
   FILE* file;
} FetchContext;

typedef struct _JumpContext {
   GtkWidget* view;
   const char* anchor;
} JumpContext;


typedef HtmlDocument* (*PFNNEWDOCUMENT)();
typedef gboolean (*PFNDOCUMENTOPEN)(HtmlDocument*, const gchar*);
typedef void (*PFNSETDOCUMENT)(HtmlView*, HtmlDocument*);
typedef void (*PFNWRITEDOCUMENT) (HtmlDocument*, const gchar*, gint len);
typedef void (*PFNCLOSEDOCUMENT) (HtmlDocument*);
typedef void (*PFNJUMP2ANCHOR) (HtmlView *view, const gchar *anchor);
typedef void (*PFNWRITESTREAM) (HtmlStream *stream, const gchar *buffer, guint size);
typedef void (*PFNCLOSESTREAM) (HtmlStream *stream);
typedef void (*PFNCLEARDOCU) (HtmlDocument *document);
typedef GtkWidget* (*PFNNEWHTMLVIEW)(void);

static PFNNEWHTMLVIEW pfnNewView = NULL;
static PFNNEWDOCUMENT pfnNewDoc = NULL;
static PFNDOCUMENTOPEN pfnOpenDoc = NULL;
static PFNSETDOCUMENT pfnSetDoc = NULL;
static PFNWRITEDOCUMENT pfnWriteDoc = NULL;
static PFNCLOSEDOCUMENT pfnCloseDoc = NULL;
static PFNJUMP2ANCHOR pfnJump2Anchor = NULL;
static PFNWRITESTREAM pfnWriteStream = NULL;
static PFNCLOSESTREAM pfnCloseStream = NULL;
static PFNCLEARDOCU pfnClearDocument = NULL;

typedef struct {
   GtkWidget*  ctrl;
   HtmlDocument* document;
   char* path; } GTKHTMLDATA;

static void gtkhtmlLinkClicked (HtmlDocument *doc, const gchar *url, GTKHTMLDATA* ctrl);
static gboolean gtkhtmlLoadURL (HtmlDocument *doc, const gchar *url,
                                HtmlStream *stream, GTKHTMLDATA* data);
static gboolean dogtkhtmlLoadURL (FetchContext* context);
static gboolean gtkhtmlJump2Anchor  (JumpContext* context);


//----------------------------------------------------------------------------
// Initializes the GTKHTML instance
// \returns void*: Pointer to the created data
//----------------------------------------------------------------------------
void* gtkhtmlInitialize () {
   TRACE ("Initializing gtkhtml viewer\n");
   hDLL = dlopen ("libgtkhtml-2.so", 0x00001);
   if (hDLL) {
      if (!pfnNewView) {
         pfnNewView = (PFNNEWHTMLVIEW)dlsym (hDLL, "html_view_new");
         pfnNewDoc = (PFNNEWDOCUMENT)dlsym (hDLL, "html_document_new");
         pfnOpenDoc = (PFNDOCUMENTOPEN)dlsym (hDLL, "html_document_open_stream");
         pfnSetDoc = (PFNSETDOCUMENT)dlsym (hDLL, "html_view_set_document");
         pfnWriteDoc = (PFNWRITEDOCUMENT)dlsym (hDLL, "html_document_write_stream");
         pfnCloseDoc = (PFNCLOSEDOCUMENT)dlsym (hDLL, "html_document_close_stream");
         pfnJump2Anchor = (PFNJUMP2ANCHOR)dlsym (hDLL, "html_view_jump_to_anchor");
         pfnWriteStream = (PFNWRITESTREAM)dlsym (hDLL, "html_stream_write");
         pfnCloseStream = (PFNCLOSESTREAM)dlsym (hDLL, "html_stream_close");
         pfnClearDocument = (PFNCLEARDOCU)dlsym (hDLL, "html_document_clear");

         if (!(pfnNewView && pfnNewDoc && pfnOpenDoc && pfnSetDoc
               && pfnWriteDoc && pfnCloseDoc && pfnJump2Anchor
               && pfnCloseStream && pfnWriteStream))
            return NULL;

         GTKHTMLDATA* data = (GTKHTMLDATA*)malloc (sizeof (GTKHTMLDATA));
         data->ctrl = pfnNewView ();
         data->path = NULL;
         data->document = pfnNewDoc ();

         g_signal_connect (G_OBJECT (data->document), "link_clicked",
                           G_CALLBACK (gtkhtmlLinkClicked), data);
         g_signal_connect (G_OBJECT (data->document), "request_url",
                           G_CALLBACK (gtkhtmlLoadURL), data);
         return data;
      }
   }
   return NULL;
}

//----------------------------------------------------------------------------
// Frees the memory used by the  GTKHTML data
// \param gtkData: Data of the call to gkthtmlIntialize
//----------------------------------------------------------------------------
void gtkhtmlFree (void* data) {
    if (data) {
       if (((GTKHTMLDATA*)data)->path)
          free (((GTKHTMLDATA*)data)->path);

       pfnSetDoc ((HtmlView*)(((GTKHTMLDATA*)data)->ctrl), NULL);
       if (((GTKHTMLDATA*)data)->document)
          pfnClearDocument (((GTKHTMLDATA*)data)->document);
       free (data);
   }
}

//----------------------------------------------------------------------------
// Creates a GtkWidget to display a HTML code
// \param gtkData: Data of the call to gkthtmlIntialize
// \returns GtkWidget*: Pointer to the created widget
//----------------------------------------------------------------------------
GtkWidget* gtkhtmlGetWidget (void* gtkData) {
   return ((GTKHTMLDATA*)gtkData)->ctrl;
}


//----------------------------------------------------------------------------
// Displays a file in the GTKHTML control
//----------------------------------------------------------------------------
int gtkhtmlDisplayFile (void* data, const char* file) {
   TRACE ("Creating document\n");
   GtkWidget* ctrl = ((GTKHTMLDATA*)data)->ctrl;
   HtmlDocument* document = ((GTKHTMLDATA*)data)->document;
   const char* anchor = NULL;

   TRACE ("Removing old document\n");
   pfnSetDoc ((HtmlView*)ctrl, NULL);
   pfnClearDocument (((GTKHTMLDATA*)data)->document);
   TRACE ("Setting document\n");
   pfnSetDoc ((HtmlView*)ctrl, ((GTKHTMLDATA*)data)->document);

   TRACE ("Opening the document\n");
   if (pfnOpenDoc (document, "text/html")) {
      // Store the path of the file
      if (!strncmp (file, "file://", 7))
         file += 7;

      gsize bytes = 0;
      GError* error = NULL;
      unsigned int nlen = strlen (file);
      file = g_filename_from_utf8 (file, nlen, 0, &bytes, &error);

      char tmp[4] = "";
      char* oldpath = ((GTKHTMLDATA*)data)->path ? ((GTKHTMLDATA*)data)->path : tmp;
      TRACE2 ("Oldpath = '%s'\n", oldpath);
      if (*file == '/')
         *oldpath = '\0';
      unsigned int olen = strlen (oldpath);
      char* newpath = (char*)malloc (nlen + olen  + 2);
      if (olen) {
         memcpy (newpath, oldpath, olen);
         if (newpath[olen - 1] != '/')
            newpath[olen++] = '/';
      }
      memcpy (newpath + olen, file, nlen + 1);
      oldpath = strrchr (newpath + olen, '#');
      
      if (oldpath) {
         *oldpath = '\0';
         anchor = oldpath + 1; 
      }

      TRACE2 ("Reading: `%s'\n", newpath);
      FILE* pFile = fopen (newpath, "r");
      if (!pFile) {
         int err = errno;
         const char* const msg = _("Error loading file '%s': %s");
         char* const strError = g_locale_to_utf8 (msg, strlen (msg), 0,
                                                  &bytes, &error);
         GtkWidget* dlg = gtk_message_dialog_new
             (GTK_WINDOW (gtk_widget_get_ancestor (ctrl, GTK_TYPE_WINDOW)),
              GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
              GTK_BUTTONS_OK, strError, newpath, g_strerror (err));
         gtk_dialog_run (GTK_DIALOG (dlg));
         gtk_widget_destroy (dlg);
         free (newpath);
         return 1;
      }

      if (((GTKHTMLDATA*)data)->path)
         free (((GTKHTMLDATA*)data)->path);
      ((GTKHTMLDATA*)data)->path = newpath;
      newpath = strrchr (newpath, '/');
      if (newpath)
         *++newpath = '\0';
      else
         *((GTKHTMLDATA*)data)->path = '\0';

      // Stream in the file
      char buffer[4096];
      int  i;
      while ((i = fread (buffer, 1, sizeof (buffer), pFile)) > 0)
         pfnWriteDoc (((GTKHTMLDATA*)data)->document, buffer, i);
      pfnCloseDoc (((GTKHTMLDATA*)data)->document);
   }
   else
      return -2;

   if (anchor && *anchor) {
      TRACE2 ("Preparing to jump to anchor - %s\n", anchor);
      JumpContext* context = g_new (JumpContext, 1);
      context->view = ctrl;
      context->anchor = anchor;
      gtk_idle_add ((GtkFunction)gtkhtmlJump2Anchor, context);
   }
   return 0;
}

//----------------------------------------------------------------------------
// Returns the error of the last action (concerning accessing the DLL)
// \param context: Context of the fetch
// \return gboolean: FALSE (stop idle function)
//----------------------------------------------------------------------------
static gboolean gtkhtmlJump2Anchor  (JumpContext* context) {
   TRACE2 ("Jumping to anchor - %s\n", context->anchor);
   pfnJump2Anchor ((HtmlView*)context->view, context->anchor);
   g_free (context);
   return FALSE;
}

//----------------------------------------------------------------------------
// Returns the error of the last action (concerning accessing the DLL)
// \return const char*: A text describing the last error
//----------------------------------------------------------------------------
const char* gtkhtmlGetError () {
    return dlerror ();
}

//----------------------------------------------------------------------------
// Callback when clicking on a link
// \param doc: Displayed document
// \param url: URL to display
// \param data: Internal GTKHTML data
//----------------------------------------------------------------------------
static void gtkhtmlLinkClicked (HtmlDocument *doc, const gchar *url, GTKHTMLDATA* data) {
    TRACE2 ("Link: %s\n", url);
    gtkhtmlDisplayFile (data, url);
}

//----------------------------------------------------------------------------
// Callback when the HTML parser founds an "inline" document (such as images,
// or an external CSS style sheet)
// \param doc: Displayed document
// \param url: URL of "included" file
// \param stream: Stream, where to write the file to
// \param data: Internal GTKHTML data
//----------------------------------------------------------------------------
static gboolean gtkhtmlLoadURL (HtmlDocument *doc, const gchar *url,
                                HtmlStream *stream, GTKHTMLDATA* data) {
   TRACE2 ("Inline document: %s\n", url);

   if (!strncmp (url, "file://", 7))
      url += 7;

   char tmp[4] = "";
   char* oldpath = ((GTKHTMLDATA*)data)->path ? ((GTKHTMLDATA*)data)->path : tmp;
   unsigned int olen = strlen (oldpath);
   char* newpath = (char*)url;
   if (olen) {
      unsigned int nlen = strlen (url);
      newpath = (char*)malloc (nlen + olen  + 2);
      memcpy (newpath, oldpath, olen);
      if (newpath[olen - 1] != '/')
         newpath[olen++] = '/';
      memcpy (newpath + olen, url, nlen + 1);
   }

   TRACE2 ("Opening document: %s\n", newpath);
   FILE* pFile = fopen (newpath, "r");
   if (newpath != url)
      free (newpath);

   if (pFile) {
      FetchContext* context = g_new (FetchContext, 1);

      context->stream = stream;
      context->file = pFile;

      gtk_idle_add ((GtkFunction)dogtkhtmlLoadURL, context);
      return TRUE;
   }
   return FALSE;
}

//----------------------------------------------------------------------------
// Callback when the HTML parser founds an "inline" document (such as images,
// or an external CSS style sheet)
// \param context: Context of the fetch
// \return gboolean: FALSE (stop idle function)
//----------------------------------------------------------------------------
static gboolean dogtkhtmlLoadURL (FetchContext* context) {
   TRACE ("Reading inline document\n");

   // Stream in the file
   char buffer[4096];
   int  i;
   while ((i = fread (buffer, 1, sizeof (buffer), context->file)) > 0)
      pfnWriteStream (context->stream, buffer, i);
   pfnCloseStream (context->stream);
   fclose (context->file);
   g_free (context);
   return FALSE;
}

#endif
