//$Id: XPrintDlg.cpp,v 1.1 1999/11/15 00:17:16 Markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XPrintDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.11.1999
//COPYRIGHT   : Anticopyright (A) 1999

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
#include <fstream.h>

#define DEBUG 0
#include "Trace.h"
#include "Check.h"

#include <gtk--/box.h>
#include <gtk--/entry.h>
#include <gtk--/label.h>
#include <gtk--/button.h>

#include "XPrintDlg.h"
#include "XMessageBox.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; connects the controls with methods
//Parameters: pNotify: Gtk_Object to notify of selected file
//            callback: Method of pNotify to call after file is selected
/*--------------------------------------------------------------------------*/
XPrintDialog::XPrintDialog (Gtk_Object* pNotify, const PACTION callback)
   : Gtk_Dialog (), pCaller (pNotify), callerMethod (callback)
   , ok (new Gtk_Button ("OK")), cancel (new Gtk_Button ("Cancel"))
   , lblCommand (new Gtk_Label ("Print command: "))
   , txtCommand (new Gtk_Entry ()), boxCommand (new Gtk_HBox ()) {
   TRACE9 ("XPrintDialog::XPrintDialog (title) '" << title << '\'');
   Check3 (pCaller); Check3 (callerMethod);

   set_title ("Print");
   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; connects the controls with methods
//Parameters: title: Title of dialog
//            pNotify: Gtk_Object to notify of selected file
//            callback: Method of pNotify to call after file is selected
/*--------------------------------------------------------------------------*/
XPrintDialog::XPrintDialog (GtkDialog* castitem, Gtk_Object* pNotify,
                            const PACTION callback)
   : Gtk_Dialog (castitem), pCaller (pNotify), callerMethod (callback)
   , ok (new Gtk_Button ("OK")), cancel (new Gtk_Button ("Cancel"))
   , lblCommand (new Gtk_Label ("Print command: "))
   , txtCommand (new Gtk_Entry ()), boxCommand (new Gtk_HBox ()) {
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
   Check3 (ok); Check3 (cancel);
   Check3 (lblCommand); Check3 (txtCommand); Check3 (boxCommand);

   // Create buttons: OK
   ok->set_usize (90, 35); ok->show ();
   connect_to_method (ok->clicked, this, &command, OK);
   get_action_area ()->pack_start (*ok, false, false, 0);

   ok->set_flags (GTK_CAN_DEFAULT);

   // - CANCEL
   cancel->set_flags (GTK_CAN_DEFAULT);
   cancel->set_usize (90, 35); cancel->show ();
   connect_to_method (cancel->clicked, this, &command, CANCEL);
   get_action_area ()->pack_start (*cancel, false, false, 0);

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
   ok->grab_default ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Performs the action of the selected button
//Parameters: action: ID of pressed button
/*--------------------------------------------------------------------------*/
void XPrintDialog::command (commandID id) {
   TRACE9 ("XPrintDialog::command: " << id);

   switch (id) {
   case OK:
      Check3 (pCaller); Check3 (callerMethod); Check3 (txtCommand);

      if (!txtCommand->get_text_length ()) {                      // No input?
	 XMessageBox::show (string ("No print-command specified"),
                            XMessageBox::ERROR);
	 return;
      } // endif no input

      {
      FILE* stream (NULL);
      stream = popen (txtCommand->get_text ().c_str (), "w");
      if (!stream) {
	 XMessageBox::show (string ("Could not run command '")
                            + txtCommand->get_text () + string ("'\nReason: ")
                            + string (strerror (errno)), XMessageBox::ERROR);
	 return;
      } // endif error printing

      ofstream pipe (fileno (stream));
      (pCaller->*callerMethod) (pipe);
      pipe.close ();
      pclose (stream);
      }

   case CANCEL:
      delete_self ();
      break;
   } // end-switch command-id
}
