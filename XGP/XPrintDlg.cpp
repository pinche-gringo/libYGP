//$Id: XPrintDlg.cpp,v 1.15 2003/07/20 04:15:38 markus Rel $

//PROJECT     : XGeneral
//SUBSYSTEM   : PrintDialog
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.15 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.11.1999
//COPYRIGHT   : Anticopyright (A) 1999 - 2003

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

#include <cstdio>
#include <cerrno>

#include <string>
#include <fstream>

#include "Trace_.h"
#include "Check.h"

#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/messagedialog.h>

#include "Internal.h"
#include "XPrintDlg.h"


//-----------------------------------------------------------------------------
/// Constructor; creates a small dialog to enter the print command.
//-----------------------------------------------------------------------------
IPrintDialog::IPrintDialog ()
   : XDialog (Glib::locale_to_utf8 (_("Print")), OKCANCEL)
     , lblCommand (new Gtk::Label (_("Print command: ")))
     , txtCommand (new Gtk::Entry ()), boxCommand (new Gtk::HBox ()) {
   TRACE9 ("IPrintDialog::IPrintDialog (title) '" << title << '\'');
   init ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
IPrintDialog::~IPrintDialog () {
   TRACE9 ("IPrintDialog::~IPrintDialog");
}


//-----------------------------------------------------------------------------
/// Initialization of the class; creates the controls
//-----------------------------------------------------------------------------
void IPrintDialog::init () {
   Check3 (lblCommand); Check3 (txtCommand); Check3 (boxCommand);

   // Command-box
   txtCommand->set_text ("lpr");
   txtCommand->show ();

   lblCommand->show ();
   boxCommand->pack_start (*lblCommand, false, false, 5);
   boxCommand->pack_start (*txtCommand, true, false, 5);
   boxCommand->show ();

   get_vbox ()->pack_start (*boxCommand, true, false, 5);

   show ();

   txtCommand->grab_focus ();
}

//-----------------------------------------------------------------------------
/// Callback after pressing OK
//-----------------------------------------------------------------------------
void IPrintDialog::okEvent () {
   TRACE9 ("IPrintDialog::okEvent ()");

   Check3 (txtCommand);

   if (!txtCommand->get_text_length ()) {                      // No input?
      Gtk::MessageDialog msg (_("No print-command specified"), Gtk::MESSAGE_ERROR);
      msg.run ();
      return;
   } // endif no input

   FILE* stream (NULL);
   stream = popen (txtCommand->get_text ().c_str (), "w");
   if (!stream) {
      std::string err (_("Could not run command `%1'\nReason: %2"));
      err.replace (err.find ("%1"), 2, txtCommand->get_text ());
      err.replace (err.find ("%2"), 2, strerror (errno));
      Gtk::MessageDialog msg (err, Gtk::MESSAGE_ERROR);
      msg.run ();
      return;
   } // endif error printing

   // TODO!! std::ofstream pipe (fileno (stream));
   std::ofstream pipe;
   printToStream (pipe);
   pipe.close ();
   pclose (stream);

   delete this;
}
