//$Id: HTMLViewer.cpp,v 1.22 2008/03/30 13:39:17 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : HTMLViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.22 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.10.2003
//COPYRIGHT   : Copyright (C) 2003 - 2005, 2008

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
#if defined HAVE_GTKHTML || defined HAVE_GTKMOZEMBED

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include <gtkmm/scrolledwindow.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#ifdef HAVE_GTKHTML
#  include "XGP/GTKViewer.h"
#endif
#ifdef HAVE_GTKMOZEMBED
#  include "XGP/GtkMozViewer.h"
#endif

#include "XGP/HTMLViewer.h"


namespace XGP {

//----------------------------------------------------------------------------
/// Creates (or updates) a dialog displaying a HTML-document
/// \param file File containing the HTML-document to display
/// \param title Title of help-window
/// \param type Type of control to create
/// \returns HTMLViewer* Pointer to the created dialog
/// \throw CreateError in case of error
//----------------------------------------------------------------------------
HTMLViewer* HTMLViewer::create (const std::string& file, const Glib::ustring& title,
				widgetTypes type) throw (CreateError) {
   TRACE9 ("HTMLViewer::create (const std::string&, const Glib::ustring&, widgetTypes) - " << file);
   Check1 (file.size ());
   Check (type < LAST);
#ifndef HAVE_GTKHTML
   Check (type != GTKHTML);
#endif
#ifndef HAVE_GTKMOZEMBED
   Check (type != GTKMOZEMBED);
#endif

   HTMLViewer* dlg (new HTMLViewer (file, title, type));
   dlg->signal_response ().connect (mem_fun (*dlg, &HTMLViewer::free));
   return dlg;
}

//----------------------------------------------------------------------------
/// Creates a dialog displaying a HTML-document
/// \param file File containing the HTML-document to display
/// \param title Title of help-window
/// \param type Type of control to create
/// \throw CreateError in case of error
//----------------------------------------------------------------------------
HTMLViewer::HTMLViewer (const std::string& file, const Glib::ustring& title,
			widgetTypes type) throw (CreateError)
    : XDialog (title + _(" Help"), XDialog::OK)
      , htmlCtrl (NULL), _type (type) {
   TRACE9 ("HTMLViewer::HTMLViewer (const std::string&, const Glib::ustring&, widgetTypes) - " << file);
   Check1 (file.size ());
   Check1 (type < LAST);
#ifndef HAVE_GTKHTML
   Check (type != GTKHTML);
#endif
#ifndef HAVE_GTKMOZEMBED
   Check (type != GTKMOZEMBED);
#endif

#ifdef HAVE_GTKHTML
   if (type == GTKHTML)
      htmlCtrl = gtkhtmlInitialize ();
#endif
#ifdef HAVE_GTKMOZEMBED
   if (type == GTKMOZEMBED)
      htmlCtrl = gtkMozEmbedInitialize ();
#endif

   if (htmlCtrl) {
      TRACE9 ("HTMLViewer::HTMLViewer (const std::string&, const Glib::ustring&, widgetTypes) - Resizing control");
      resize (640, 400);

      TRACE9 ("HTMLViewer::HTMLViewer (const std::string&, const Glib::ustring&, widgetTypes) - Adding control");
#ifdef HAVE_GTKHTML
      if (type == GTKHTML) {
	 Gtk::ScrolledWindow* scrl (manage (new Gtk::ScrolledWindow));
	 scrl->add (*manage (Glib::wrap (htmlCtrl)));
	 scrl->set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	 get_vbox ()->pack_start (*scrl);
      }
#endif
#ifdef HAVE_GTKMOZEMBED
      if (type == GTKMOZEMBED)
	 get_vbox ()->pack_start (*manage (Glib::wrap (htmlCtrl)));
#endif

      show_all_children ();
      TRACE9 ("HTMLViewer::HTMLViewer (const std::string&, const Glib::ustring&, widgetTypes) - Showing dialog");
      show ();
      Glib::signal_idle ().connect
	 (bind (mem_fun (*this, &HTMLViewer::_display), file));
   }
   else {
      std::string err;
#ifdef HAVE_GTKHTML
      if (type == GTKHTML) {
	 err = _("Can't display the GtkHTML control!\n\nReason: %1");
	 err.replace (err.find ("%1"), 2, gtkhtmlGetError ());
      }
#endif
#ifdef HAVE_GTKMOZEMBED
      if (type == GTKMOZEMBED)
	 err = _("Can't display GtkMozEmbed control!");
#endif
      throw (CreateError (err));
   }
}

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
HTMLViewer::~HTMLViewer () {
   TRACE9 ("HTMLViewer::~HTMLViewer ()");
}


//----------------------------------------------------------------------------
/// Sets the HTML-document to display in the dialog
/// \param file File containing the HTML-document to display
//----------------------------------------------------------------------------
void HTMLViewer::display (const std::string& file) {
   TRACE9 ("HTMLViewer::display (const std::string&) - " << file);
   Check1 (file.size ());

#ifdef HAVE_GTKHTML
   if (_type == GTKHTML)
      gtkhtmlDisplayFile (htmlCtrl, file.c_str ());
#endif
#ifdef HAVE_GTKMOZEMBED
   if (_type == GTKMOZEMBED)
      gtkMozEmbedDisplayURL (htmlCtrl, file.c_str ());
#endif
   TRACE9 ("HTMLViewer::display (const std::string&) - Finished " << file);
}

//----------------------------------------------------------------------------
/// Sets the HTML-document to display in the dialog; do be used from the idle-signal
/// \param file File containing the HTML-document to display
//----------------------------------------------------------------------------
int HTMLViewer::_display (const std::string file) {
   display (file);
   return 0;
}

}

#endif
