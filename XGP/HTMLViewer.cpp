//$Id: HTMLViewer.cpp,v 1.10 2004/01/15 06:26:35 markus Rel $

//PROJECT     : XGeneral
//SUBSYSTEM   : HTMLViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.10.2003
//COPYRIGHT   : Copyright (C) 2003, 2004

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

#include <gtkmm/scrolledwindow.h>


#define CHECK 9
#define TRACELEVEL 9
#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "XGP/GTKViewer.h"
#include "XGP/HTMLViewer.h"


namespace XGP {

//----------------------------------------------------------------------------
/// Creates (or updates) a dialog displaying a HTML-document
/// \param file: File containing the HTML-document to display
/// \returns HTMLViewer* : Pointer to the created dialog
/// \throw std::string in case of error
//----------------------------------------------------------------------------
HTMLViewer* HTMLViewer::create (const std::string& file) throw (std::string) {
   TRACE9 ("HTMLViewer::create (const std::string&) - " << file);
   Check1 (file.size ());

   HTMLViewer* dlg (new HTMLViewer (file));
   dlg->signal_response ().connect (slot (*dlg, &XDialog::free));
   return dlg;
}


//----------------------------------------------------------------------------
/// Creates a dialog displaying a HTML-document
/// \param file: File containing the HTML-document to display
/// \throw \c std::string in case of error
//----------------------------------------------------------------------------
HTMLViewer::HTMLViewer (const std::string& file) throw (std::string)
    : XDialog (Glib::locale_to_utf8 (_("Help window")), XDialog::OK)
      , htmlCtrl (gtkhtmlInitialize ())
      , scrl (manage (new Gtk::ScrolledWindow)) {
   TRACE9 ("HTMLViewer::HTMLViewer (const std::string&) - " << file);
   Check1 (file.size ());

   if (htmlCtrl) {
      resize (640, 400);
      GtkWidget* ctrl (gtkhtmlGetWidget (htmlCtrl));
      gtk_widget_show (ctrl);
      get_vbox ()->pack_start (*scrl);

      scrl->add (*Glib::wrap (ctrl, false));
      scrl->set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

      display (file.c_str ());
      show_all_children ();
      show ();
   }
   else {
      std::string err (Glib::locale_to_utf8 (_("Can't display the HTML control!\n\nReason: %1")));
      err.replace (err.find ("%1"), 2, gtkhtmlGetError ());
      throw (err);
   }
}

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
HTMLViewer::~HTMLViewer () {
   TRACE9 ("HTMLViewer::~HTMLViewer ()");
   GtkWidget* ctrl (gtkhtmlGetWidget (htmlCtrl)); Check3 (ctrl);
   gtk_widget_destroy (ctrl);
   gtkhtmlFree (htmlCtrl);
}


//----------------------------------------------------------------------------
/// Sets the HTML-document to display in the dialog
/// \param file: File containing the HTML-document to display
/// \throw std::string in case of error
//----------------------------------------------------------------------------
void HTMLViewer::display (const std::string& file) throw (std::string) {
   TRACE9 ("HTMLViewer::display (const std::string&) - " << file);
   Check1 (file.size ());

   std::string err;
   int rc (gtkhtmlDisplayFile (htmlCtrl, file.c_str ()));
   switch (rc) {
   case 0:
      break;

   case -1:
      err = Glib::locale_to_utf8 (_("Can't display the HTML control!\n\nReason: %1"));
      err.replace (err.find ("%1"), 2, gtkhtmlGetError ());
      break;

   case -2:
      err = Glib::locale_to_utf8 (_("Can't display the HTML document!\n\nReason: %1"));
      err.replace (err.find ("%1"), 2, gtkhtmlGetError ());
      break;

   default:
      throw (err);
   } // end-switch

   if (err.size ()) {
      throw err;
   }
}

}

#endif
