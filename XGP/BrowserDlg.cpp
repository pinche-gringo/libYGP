//$Id: BrowserDlg.cpp,v 1.21 2005/10/17 03:50:40 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : BrowserDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.21 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.01.2003
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


#include <gtkmm/box.h>
#include <gtkmm/radiobutton.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/Internal.h>

#include "XGP/BrowserDlg.h"


namespace XGP {

const char* BrowserDlg::browserNames[] = {
#ifdef HAVE_GTKHTML
    N_("GTKHTML"),
#endif
#ifdef HAVE_GTKMOZEMBED
    N_("GTKMOZEMBED"),
#endif
#if HAVE_FIREFOX > 0
    N_("firefox"),
#endif
#if HAVE_GALEON > 0
    N_("galeon"),
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
#if HAVE_IEXPLORER > 0
    N_("iexplore"),
#endif
    N_("Other:"),
};


//-----------------------------------------------------------------------------
/// Constructor
//-----------------------------------------------------------------------------
BrowserDlg::BrowserDlg (Glib::ustring& cmd)
   : XDialog (Glib::locale_to_utf8 (_("Select a browser")), OKCANCEL)
     , pboxOther (new Gtk::HBox ()), path (cmd) {
   TRACE3 ("BrowserDlg::BrowserDlg (Glib::ustring&) - " << cmd);

   pboxOther->show ();

   if (cmd.empty ())
      cmd = browserNames[0];

   Gtk::RadioButtonGroup group;
   Gtk::RadioButton* rb;

   for (unsigned int i (0);
	i < (sizeof (browserNames) / sizeof (*browserNames)); ++i) {
      rb = manage (new Gtk::RadioButton (group, _(browserNames[i]), 0));
      rb->signal_clicked ().connect (bind (mem_fun (*this, &BrowserDlg::control), i));
      aBrowsers.push_back (rb);

      (i == ((sizeof (browserNames) / sizeof (*browserNames)) - 1))
	 ? pboxOther->pack_start (*rb, false, false, 5)
	 : get_vbox ()->pack_start (*rb, false, false);
      rb->show ();
      if (cmd == browserNames[i]) {
         rb->set_active (true);
         TRACE4 ("BrowserDlg::BrowserDlg (Glib::ustring&) - Using browser " << cmd);
	 control (i);
      }
   }

   path.show ();
   pboxOther->pack_start (path, true, true);
   get_vbox ()->pack_start (*pboxOther, false, false);

   show ();
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
/// \param cmd: Default command to execute to start the browser
//  \returns BrowserDlg*: Pointer to created dialog
//----------------------------------------------------------------------------
BrowserDlg* BrowserDlg::create (Glib::ustring& cmd) {
   BrowserDlg* dlg (new BrowserDlg (cmd));
   dlg->signal_response ().connect (mem_fun (*dlg, &BrowserDlg::free));
   return dlg;
}

}
