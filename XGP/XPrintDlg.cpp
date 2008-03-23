//$Id: XPrintDlg.cpp,v 1.26 2008/03/23 22:12:55 markus Exp $

//PROJECT     : libXGP
//SUBSYSTEM   : PrintDialog
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.26 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.11.1999
//COPYRIGHT   : Copyright (C) 1999 - 2004, 2006, 2008

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

#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/messagedialog.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include "XGP/XPrintDlg.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; creates a small dialog to enter the print command.
//-----------------------------------------------------------------------------
PrintDialog::PrintDialog ()
   : XDialog (_("Print"), OKCANCEL), sigPrint (),
     lblCommand (new Gtk::Label (_("Print command: "))),
     txtCommand (new Gtk::Entry), boxCommand (new Gtk::HBox) {
   TRACE9 ("PrintDialog::PrintDialog (title) '" << title << '\'');
   init ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
PrintDialog::~PrintDialog () {
   TRACE9 ("PrintDialog::~PrintDialog");
}


//-----------------------------------------------------------------------------
/// Initialization of the class; creates the controls
//-----------------------------------------------------------------------------
void PrintDialog::init () {
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
void PrintDialog::okEvent () {
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

   sigPrint.emit (stream);
   pclose (stream);
}

//----------------------------------------------------------------------------
/// Creates a (modeless) print dialog and registers a handler to free it
/// after deleting.
/// \returns IPrintDialog*: Pointer to created dialog
//----------------------------------------------------------------------------
PrintDialog* PrintDialog::create () {
    PrintDialog* dlg (new PrintDialog ());
    dlg->signal_response ().connect (mem_fun (*dlg, &PrintDialog::free));
    return dlg;
}

}
