//$Id: XPrintDlg.cpp,v 1.13 2003/03/03 05:53:43 markus Rel $

//PROJECT     : XGeneral
//SUBSYSTEM   : XPrintDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.13 $
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

#include <stdio.h>
#include <errno.h>

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


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; creates a small dialog to enter the print command.
//Parameters: pNotify: Object to notify of the print command
//            callback: Method of pNotify to call for printing
/*--------------------------------------------------------------------------*/
XPrintDialog::XPrintDialog (Object* pNotify, const PACTION callback)
   : XDialog (OKCANCEL), pCaller (pNotify), callerMethod (callback)
     , lblCommand (new Gtk::Label (_("Print command: ")))
     , txtCommand (new Gtk::Entry ()), boxCommand (new Gtk::HBox ()) {
   TRACE9 ("XPrintDialog::XPrintDialog (title) '" << title << '\'');
   Check3 (pCaller); Check3 (callerMethod);

   set_title (_("Print"));
   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; creates a small dialog to enter the print command.
//Parameters: castitem: GTK+ dialog to use
//            pNotify: Object to notify of the print command
//            callback: Method of pNotify to call for printing
/*--------------------------------------------------------------------------*/
XPrintDialog::XPrintDialog (GtkDialog* castitem, Object* pNotify,
                            const PACTION callback)
   : XDialog (castitem), pCaller (pNotify), callerMethod (callback)
   , lblCommand (new Gtk::Label ("Print command: "))
   , txtCommand (new Gtk::Entry ()), boxCommand (new Gtk::HBox ()) {
   TRACE9 ("XPrintDialog::XPrintDialog (castitem)");
   Check3 (pCaller); Check3 (callerMethod);

   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XPrintDialog::~XPrintDialog () {
   TRACE9 ("XPrintDialog::~XPrintDialog");
   hide ();
}


/*--------------------------------------------------------------------------*/
//Purpose   : Initialization of the class; creates the controls
/*--------------------------------------------------------------------------*/
void XPrintDialog::init () {
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

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after pressing OK
/*--------------------------------------------------------------------------*/
void XPrintDialog::okEvent () {
   TRACE9 ("XPrintDialog::okEvent ()");

   Check3 (pCaller); Check3 (callerMethod); Check3 (txtCommand);

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
   (pCaller->*callerMethod) (pipe);
   pipe.close ();
   pclose (stream);

   delete this;
}
