//$Id: BrowserDlg.cpp,v 1.27 2008/03/30 13:39:17 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : BrowserDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.27 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.01.2003
//COPYRIGHT   : Copyright (C) 2003 - 2008

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


#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/radiobutton.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "XGP/BrowserDlg.h"


namespace XGP {

const char* BrowserDlg::browserNames[] = {
#ifdef HAVE_GTKHTML
    N_("GTKHTML"),
#endif
#ifdef HAVE_GTKMOZEMBED
    N_("GTKMOZEMBED"),
#endif
#ifdef HAVE_WEBKIT
    N_("WEBKIT"),
#endif
#if HAVE_EPIPHANY > 0
    N_("epiphany"),
#endif
#if HAVE_FIREFOX > 0
    N_("firefox"),
#endif
#if HAVE_GALEON > 0
    N_("galeon"),
#endif
#if HAVE_SEAMONKEY > 0
    N_("seamonkey"),
#endif
#if HAVE_MOZILLA > 0
    N_("mozilla"),
#endif
#if HAVE_NETSCAPE > 0
    N_("netscape"),
#endif
#if HAVE_KONQUEROR > 0
    N_("konqueror"),
#endif
#if HAVE_OPERA > 0
    N_("opera"),
#endif
#if HAVE_DILLO > 0
    N_("dillo"),
#endif
#if HAVE_IEXPLORER > 0
    N_("iexplore"),
#endif
    N_("Other:"),
};


//-----------------------------------------------------------------------------
/// Constructor
//-----------------------------------------------------------------------------
BrowserDlg::BrowserDlg (Glib::ustring& cmd)
   : XDialog (_("Select a browser"), OKCANCEL),
     pboxOther (new Gtk::HBox), aBrowsers (), path (cmd) {
   TRACE3 ("BrowserDlg::BrowserDlg (Glib::ustring&) - " << cmd);

   unsigned int selection (-1U);

   if (cmd.empty ())
      cmd = browserNames[0];

   Gtk::RadioButtonGroup group;
   Gtk::RadioButton* rb;

   std::string filename;
   for (unsigned int i (0);
	i < (sizeof (browserNames) / sizeof (*browserNames)); ++i) {
      rb = NULL;
      try {
	 filename = PKGDIR "Browser_";
	 filename += browserNames[i];
	 filename += ".png";
	 TRACE1 ("BrowserDlg::BrowserDlg (Glib::ustring&) - Loading: " << filename);
	 Glib::RefPtr<Gdk::Pixbuf> img (Gdk::Pixbuf::create_from_file (filename));

	 Gtk::HBox* boxRB (manage (new Gtk::HBox));
	 Gtk::Label* lblRB (manage (new Gtk::Label (_(browserNames[i]), true)));
	 Gtk::Image* imgRB (manage (new Gtk::Image (img)));

	 rb = manage (new Gtk::RadioButton (group));
	 rb->add (*boxRB);
	 boxRB->pack_start (*imgRB, Gtk::PACK_SHRINK, 5);
	 boxRB->pack_start (*lblRB, Gtk::PACK_EXPAND_WIDGET, 5);
      }
      catch (Glib::Error& e) {
	 TRACE9 ("BrowserDlg::BrowserDlg (Glib::ustring&) - Failed loading icon " << browserNames[i] << ":\n\t" << e.what ());
	 rb = manage (new Gtk::RadioButton (group, _(browserNames[i]), false));
      }

      Check3 (rb);
      rb->signal_clicked ().connect (bind (mem_fun (*this, &BrowserDlg::control), i));
      aBrowsers.push_back (rb);

      (i == ((sizeof (browserNames) / sizeof (*browserNames)) - 1))
	 ? pboxOther->pack_start (*rb, false, false, 5)
	 : get_vbox ()->pack_start (*rb, false, false);
      if (cmd == browserNames[i]) {
         rb->set_active (true);
	 selection = i;
         TRACE4 ("BrowserDlg::BrowserDlg (Glib::ustring&) - Using browser " << cmd);
      }
   }

   pboxOther->pack_start (path, true, true);
   get_vbox ()->pack_start (*pboxOther, false, false);

   show_all_children ();
   show ();

   control (selection != -1U ? selection : 0);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
BrowserDlg::~BrowserDlg () {
}

//-----------------------------------------------------------------------------
/// Handling of the OK button; closes dialog with commiting data
//-----------------------------------------------------------------------------
void BrowserDlg::okEvent () {
   path.commit ();
   XDialog::okEvent ();
}

//-----------------------------------------------------------------------------
/// Callback for grey-logic of the dialog
//-----------------------------------------------------------------------------
void BrowserDlg::control (unsigned int cmd) {
   TRACE9 ("BrowserDlg::control (unsigned int) - " << cmd);
   Check1 (cmd < aBrowsers.size ());

   if (aBrowsers[cmd]->get_active ()) {
      path.set_sensitive (cmd == (aBrowsers.size () - 1));
      if (path.is_sensitive ())
         path.grab_focus ();
      else {
         Check1 (cmd < aBrowsers.size ());
         path.setText (browserNames[cmd]);
      }
   }
}

//----------------------------------------------------------------------------
/// Creates a (modeless) dialog and registers a handler to free it after
/// deleting.
/// \param cmd Default command to execute to start the browser
//  \returns BrowserDlg* Pointer to created dialog
//----------------------------------------------------------------------------
BrowserDlg* BrowserDlg::create (Glib::ustring& cmd) {
   BrowserDlg* dlg (new BrowserDlg (cmd));
   dlg->signal_response ().connect (mem_fun (*dlg, &BrowserDlg::free));
   return dlg;
}

}
