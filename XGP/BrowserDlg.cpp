//$Id: BrowserDlg.cpp,v 1.6 2003/03/06 04:27:55 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : X-windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.01.2003
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


#include <gtkmm/box.h>
#include <gtkmm/radiobutton.h>

#include <Check.h>
#include <Trace_.h>

#include "BrowserDlg.h"


const char* BrowserDlg::browserNames[4] = { N_("galeon"), N_("mozilla"),
                                            N_("netscape"), N_("konqueror") };


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
BrowserDlg::BrowserDlg (std::string& cmd)
   : XDialog (OKCANCEL), pboxOther (new Gtk::HBox ()), path (cmd) {
   TRACE3 ("BrowserDlg::BrowserDlg (std::string&) - " << cmd);
   Check3 (sizeof (browserNames) < sizeof (aBrowsers));
   set_title (_("Select a browser"));

   pboxOther->show ();

   if (cmd.empty ())
      cmd = browserNames[0];

   Gtk::RadioButton_Helpers::Group group;

   // Create Other radio button
   aBrowsers[sizeof (browserNames) / sizeof (browserNames[0])] =
      new Gtk::RadioButton (group, _("Other: "), 0);
   aBrowsers[sizeof (browserNames) / sizeof (browserNames[0])]->set_active (true);
   pboxOther->pack_start (*aBrowsers[sizeof (browserNames) / sizeof (browserNames[0])],
                          false, false);
   aBrowsers[sizeof (browserNames) / sizeof (browserNames[0])]->show ();
   aBrowsers[sizeof (browserNames) / sizeof (browserNames[0])]->signal_clicked ().connect
      (bind (slot (*this, &BrowserDlg::control),
             sizeof (browserNames) / sizeof (browserNames[0])));

   // Create radio button for other browsers and set them if specified by cmd
   for (unsigned int i (0); i < (sizeof (browserNames) / sizeof (browserNames[0]));
        ++i) {
      aBrowsers[i] = new Gtk::RadioButton (group, _(browserNames[i]), 0);
      aBrowsers[i]->signal_clicked ().connect (bind (slot (*this, &BrowserDlg::control), i));
      get_vbox ()->pack_start (*aBrowsers[i], false, false);
      aBrowsers[i]->show ();
      if (cmd == browserNames[i]) {
         aBrowsers[i]->set_active (true);
         TRACE4 ("BrowserDlg::BrowserDlg (std::string&) - Using browser " << cmd);
      }
   }

   path.show ();
   pboxOther->pack_start (path, true, true);
   get_vbox ()->pack_start (*pboxOther, false, false);

   show ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
BrowserDlg::~BrowserDlg () {
   for (unsigned int i (0); i < (sizeof (aBrowsers) / sizeof (aBrowsers[0])); ++i)
      delete aBrowsers[i];
}

/*--------------------------------------------------------------------------*/
//Purpose   : Handling of the OK button; closes dialog with commiting data
/*--------------------------------------------------------------------------*/
void BrowserDlg::okEvent () {
   path.commit ();
   XDialog::okEvent ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback for grey-logic of the dialog
/*--------------------------------------------------------------------------*/
void BrowserDlg::control (unsigned int cmd) {
   TRACE9 ("BrowserDlg::control (unsigned int) - " << cmd);
   Check1 (cmd < sizeof (aBrowsers) / sizeof (aBrowsers[0]));
   
   if (aBrowsers[cmd]->get_active ()) {
      path.set_sensitive (cmd == ((sizeof (aBrowsers) / sizeof (aBrowsers[0])) - 1));
      if (path.is_sensitive ())
         path.grab_focus ();
      else {
         Check1 (cmd < sizeof (browserNames) / sizeof (browserNames[0]));
         path.getAttribute () = browserNames[cmd];
         path.update ();
      }
   }
}
