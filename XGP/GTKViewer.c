//$Id: GTKViewer.c,v 1.13 2005/03/08 04:48:57 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : GTKViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.13 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.10.2003
//COPYRIGHT   : Copyright (C) 2003 - 2005

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
#include <YGP/Internal.h>

#if SYSTEM == UNIX
const char DIR_SEPARATOR = '/';
#elif SYSTEM == WINDOWS
const char DIR_SEPARATOR = '\\';
#else
#  error Not implemented yet!
#endif

#ifdef HAVE_GTKHTML

#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>

#include <dlfcn.h>

#include <YGP/Check.h>

#include <gtk/gtkmessagedialog.h>

#include <gtkhtml/gtkhtml.h>

#include "GTKViewer.h"


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

typedef GtkWidget* (*PFNNEWHTML)(void);
typedef GtkHTMLStream* (*PFNNEWSTREAM)(GtkHTML* widget);
typedef void (*PFNFRAMES)(GtkHTML* widget, gboolean);
typedef void (*PFNWRITE)(GtkHTML* widget, GtkHTMLStream *stream, const gchar *buffer, guint size);
typedef void (*PFNEND)(GtkHTML* widget, GtkHTMLStream *stream, GtkHTMLStreamStatus status);
typedef void (*PFNSETBASE)(GtkHTML* widget, const char* base);

static PFNNEWHTML   pfnNew       = NULL;
static PFNFRAMES    pfnFrames    = NULL;
static PFNNEWSTREAM pfnNewStream = NULL;
static PFNWRITE     pfnWrite     = NULL;
static PFNEND       pfnEnd       = NULL;
static PFNSETBASE   pfnSetBase   = NULL;

static void gtkhtmlLoadURL (GtkHTML *widget, const gchar *url, GtkHTMLStream *stream, gpointer);
static void gtkhtmlLinkClicked (GtkHTML *widget, const gchar *url, gpointer);


//----------------------------------------------------------------------------
/// Initializes the GTKHTML instance
/// \returns GtkWidget*: Created HTML-widget
//----------------------------------------------------------------------------
GtkWidget* gtkhtmlInitialize () {
   TRACE ("Initializing gtkhtml viewer\n");

   if (!hDLL)
      hDLL = dlopen ("libgtkhtml-3.1.so", 0x00001);

   if (hDLL) {
      if (!pfnNew) {
         pfnNew = (PFNNEWHTML)dlsym (hDLL, "gtk_html_new");
         pfnNewStream = (PFNNEWSTREAM)dlsym (hDLL, "gtk_html_begin");
	 pfnFrames = (PFNFRAMES)dlsym (hDLL, "gtk_html_set_allow_frameset");
         pfnWrite = (PFNWRITE)dlsym (hDLL, "gtk_html_write");
         pfnEnd = (PFNEND)dlsym (hDLL, "gtk_html_end");
	 pfnSetBase = (PFNSETBASE)dlsym (hDLL, "gtk_html_set_base");

         if (!(pfnNew && pfnNewStream && pfnFrames && pfnWrite && pfnEnd && pfnSetBase))
            return NULL;
      }

      GtkWidget* widget = pfnNew ();

      g_signal_connect (widget, "link_clicked", G_CALLBACK (gtkhtmlLinkClicked), NULL);
      g_signal_connect (widget, "url_requested", G_CALLBACK (gtkhtmlLoadURL), widget);
      return widget;
   }
   return NULL;
}

//----------------------------------------------------------------------------
/// Displays a file in the GTKHTML control
/// \param ctrl: HTML-widget
/// \param file: File to display
/// \returns int: <0 in case of an hard error; 0: OK; >0 soft error
//----------------------------------------------------------------------------
int gtkhtmlDisplayFile (GtkWidget* ctrl, const char* file) {
   Check2 (ctrl);
   Check2 (file);

   gtkhtmlLoadURL ((GtkHTML*)ctrl, file, pfnNewStream ((GtkHTML*)ctrl), NULL);
}

//----------------------------------------------------------------------------
/// Returns the error of the last action (concerning accessing the DLL)
/// \return const char*: A text describing the last error
//----------------------------------------------------------------------------
const char* gtkhtmlGetError () {
    return dlerror ();
}

//----------------------------------------------------------------------------
/// Callback when clicking on a link
/// \param doc: Displayed document
/// \param url: URL to display
/// \param x: Unused
//----------------------------------------------------------------------------
static void gtkhtmlLinkClicked (GtkHTML *widget, const gchar *url, gpointer x) {
    TRACE2 ("Link: %s\n", url);
    gtkhtmlLoadURL (widget, url, pfnNewStream (widget), NULL);
}

//----------------------------------------------------------------------------
/// Callback when the HTML parser founds an "inline" document (such as images,
/// or an external CSS style sheet)
/// \param doc: Displayed document
/// \param url: URL of "included" file
/// \param stream: Stream, where to write the file to
/// \param x: Unused
//----------------------------------------------------------------------------
static void gtkhtmlLoadURL (GtkHTML *widget, const gchar *url, GtkHTMLStream *stream, gpointer x) {

   if (*url != '#') {
      // Store the path of the url
      if (!strncmp (url, "file://", 7))
	 url += 7;

      gsize nlen = 0;
      const char* nfile = g_filename_from_utf8 (url, -1, 0, &nlen, NULL);
      if (nfile)
	 url = nfile;

      if (*url == DIR_SEPARATOR) {
	 const char* end = strrchr (url, DIR_SEPARATOR); Check3 (end);
	 char* newpath = (char*)malloc (end - url + 1);
	 memcpy (newpath, url, end - url);
	 newpath[end - url] = '\\';
	 pfnSetBase (widget, newpath);
	 free (newpath);
	 url = end + 1;
      }

      TRACE2 ("Reading: `%s'\n", url);
      FILE* pFile = fopen (url, "r");
      if (!pFile) {
	 int err = errno;
	 const char* const msg = _("Error loading file '%s': %s");
	 char* const strError = g_locale_to_utf8 (msg, -1, 0,
						  &nlen, NULL);
	 GtkWidget* dlg = gtk_message_dialog_new
	    (GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (widget), GTK_TYPE_WINDOW)),
	     GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
	     GTK_BUTTONS_OK, strError, url, g_strerror (err));
	 gtk_dialog_run (GTK_DIALOG (dlg));
	 gtk_widget_destroy (dlg);
	 return;
      }

      // Stream in the file
      char buffer[4096];
      int  i;
      while ((i = fread (buffer, 1, sizeof (buffer), pFile)) > 0)
	 pfnWrite (widget, stream, buffer, i);
      pfnEnd (widget, stream, (i == -1) ? GTK_HTML_STREAM_ERROR : GTK_HTML_STREAM_OK);
      fclose (pFile);
   }
}

#endif
