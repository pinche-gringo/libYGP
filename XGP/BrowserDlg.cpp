//$Id: BrowserDlg.cpp,v 1.17 2004/09/06 00:27:38 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : X-windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.17 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.01.2003
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


#include <gtkmm/box.h>
#include <gtkmm/radiobutton.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/Internal.h>

#include "XGP/BrowserDlg.h"


namespace XGP {

const char* BrowserDlg::browserNames[BROWSERS] = {
#ifdef HAVE_GTKHTML
    N_("GTKHTML"),
#endif
    N_("galeon"), N_("mozilla"), N_("netscape"), N_("konqueror")
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

   // Create Other radio button
   aBrowsers[BROWSERS] = new Gtk::RadioButton (group, _("Other: "), 0);
   aBrowsers[BROWSERS]->set_active (true);
   pboxOther->pack_start (*aBrowsers[BROWSERS], false, false);
   aBrowsers[BROWSERS]->show ();
   aBrowsers[BROWSERS]->signal_clicked ().connect
      (bind (mem_fun (*this, &BrowserDlg::control), BROWSERS));

   // Create radio button for other browsers and set them if specified by cmd
   for (unsigned int i (0); i < (BROWSERS);
        ++i) {
      aBrowsers[i] = new Gtk::RadioButton (group, _(browserNames[i]), 0);
      aBrowsers[i]->signal_clicked ().connect (bind (mem_fun (*this, &BrowserDlg::control), i));
      get_vbox ()->pack_start (*aBrowsers[i], false, false);
      aBrowsers[i]->show ();
      if (cmd == browserNames[i]) {
         aBrowsers[i]->set_active (true);
         TRACE4 ("BrowserDlg::BrowserDlg (Glib::ustring&) - Using browser " << cmd);
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
   for (unsigned int i (0); i < (sizeof (aBrowsers) / sizeof (aBrowsers[0])); ++i)
      delete aBrowsers[i];
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
   Check1 (cmd < sizeof (aBrowsers) / sizeof (aBrowsers[0]));
   
   if (aBrowsers[cmd]->get_active ()) {
      path.set_sensitive (cmd == ((sizeof (aBrowsers) / sizeof (aBrowsers[0])) - 1));
      if (path.is_sensitive ())
         path.grab_focus ();
      else {
         Check1 (cmd < BROWSERS);
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
