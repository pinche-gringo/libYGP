//$Id: GTKViewer.c,v 1.17 2008/06/08 12:42:40 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : GTKViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.17 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.10.2003
//COPYRIGHT   : Copyright (C) 2003 - 2005

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
typedef const char* (*PFNGETBASE)(GtkHTML* widget);
typedef gboolean (*PFNANCHOR)(GtkHTML* widget, const char* anchor);

static PFNNEWHTML   pfnNew       = NULL;
static PFNFRAMES    pfnFrames    = NULL;
static PFNNEWSTREAM pfnNewStream = NULL;
static PFNWRITE     pfnWrite     = NULL;
static PFNEND       pfnEnd       = NULL;
static PFNSETBASE   pfnSetBase   = NULL;
static PFNGETBASE   pfnGetBase   = NULL;
static PFNANCHOR    pfnAnchor    = NULL;

static void gtkhtmlLoadURL (GtkHTML *widget, const gchar *url, GtkHTMLStream *stream, gpointer);
static void gtkhtmlLinkClicked (GtkHTML *widget, const gchar *url, gpointer);


//----------------------------------------------------------------------------
/// Initializes the GTKHTML instance
/// \returns GtkWidget*: Created HTML-widget
//----------------------------------------------------------------------------
GtkWidget* gtkhtmlInitialize () {
   TRACE ("Initializing gtkhtml viewer\n");

   if (!hDLL)
      hDLL = dlopen ("libgtkhtml-" HAVE_GTKHTML ".so", 0x00001);

   if (hDLL) {
      if (!pfnNew) {
         pfnNew = (PFNNEWHTML)dlsym (hDLL, "gtk_html_new");
         pfnNewStream = (PFNNEWSTREAM)dlsym (hDLL, "gtk_html_begin");
	 pfnFrames = (PFNFRAMES)dlsym (hDLL, "gtk_html_set_allow_frameset");
         pfnWrite = (PFNWRITE)dlsym (hDLL, "gtk_html_write");
         pfnEnd = (PFNEND)dlsym (hDLL, "gtk_html_end");
	 pfnSetBase = (PFNSETBASE)dlsym (hDLL, "gtk_html_set_base");
	 pfnGetBase = (PFNGETBASE)dlsym (hDLL, "gtk_html_get_base");
         pfnAnchor = (PFNANCHOR)dlsym (hDLL, "gtk_html_jump_to_anchor");

         if (!(pfnNew && pfnNewStream && pfnFrames && pfnWrite && pfnEnd
	       && pfnSetBase && pfnGetBase && pfnAnchor))
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
/// \remarks: Don't call with a NULL-pointer for file
//----------------------------------------------------------------------------
void gtkhtmlDisplayFile (GtkWidget* ctrl, const char* file) {
   Check2 (ctrl);
   Check2 (file);

   if (!strncmp (file, "file://", 7))
      file += 7;

   gsize nlen = 0;
   const char* nfile = g_filename_from_utf8 (file, -1, 0, &nlen, NULL);
   if (!nfile)
      nfile = file;

   // Check if file contains a path
   const char* pathEnd = strrchr (nfile, DIR_SEPARATOR);
   if (pathEnd) {
      // Store the path of the url
      const char* oldPath = "";
      gsize oldLen = 0;
      if (*nfile != DIR_SEPARATOR) {
	 oldPath = pfnGetBase ((GtkHTML*)ctrl);
	 if (!oldPath)
	    oldPath = "";
	 oldLen = strlen (oldPath);
      }

      if (!oldLen || strncmp (oldPath, file, oldLen)) {
	 char* newPath = (char*)malloc (oldLen + ++pathEnd - nfile + 1);
	 memcpy (newPath, oldPath, oldLen);
	 memcpy (newPath + oldLen, nfile, pathEnd - nfile);
	 newPath[oldLen + pathEnd - nfile] = '\0';
	 pfnSetBase ((GtkHTML*)ctrl, newPath);
	 TRACE2 ("Set base %s\n", newPath);
	 free (newPath);
      }
   }

   gtkhtmlLoadURL ((GtkHTML*)ctrl, nfile, pfnNewStream ((GtkHTML*)ctrl), NULL);
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
    gtkhtmlDisplayFile (GTK_WIDGET (widget), url);
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
   TRACE2 ("Loading file `%s'\n", url);
   Check1 (url); Check1 (widget); Check1 (stream);
   Check1 (*url);

   const char* anchor = NULL;
   if (*url != '#') {
      char* anch = strrchr (url + 1, '#');
      if (anch) {
	 *anch = '\0';
	 anchor = anch + 1;
      }

      TRACE2 ("Reading file `%s'\n", url);
      FILE* pFile = fopen (url, "r");

      if (!pFile) {
	 gsize nlen = 0;
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
   else
      anchor = url + 1;

   if (anchor && *anchor) {
      TRACE2 ("Jumping to anchor - %s\n", anchor);
      pfnAnchor (widget, anchor);
   }
}

#endif
