//$Id: XPrintDlg.cpp,v 1.10 2002/12/25 05:11:32 markus Rel $

//PROJECT     : XGeneral
//SUBSYSTEM   : XPrintDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.11.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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

#include "Trace_.h"
#include "Check.h"

#include <gtk--/box.h>
#include <gtk--/entry.h>
#include <gtk--/label.h>
#include <gtk--/button.h>

#include "Internal.h"
#include "XPrintDlg.h"
#include "XMessageBox.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; creates a small dialog to enter the print command.
//Parameters: pNotify: Object to notify of the print command
//            callback: Method of pNotify to call for printing
/*--------------------------------------------------------------------------*/
XPrintDialog::XPrintDialog (Object* pNotify, const PACTION callback)
   : Dialog (), pCaller (pNotify), callerMethod (callback)
     , ok (new Button (_("OK"))), cancel (new Button (_("Cancel")))
     , lblCommand (new Label (_("Print command: ")))
     , txtCommand (new Entry ()), boxCommand (new HBox ()) {
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
   : Dialog (castitem), pCaller (pNotify), callerMethod (callback)
   , ok (new Button ("OK")), cancel (new Button ("Cancel"))
   , lblCommand (new Label ("Print command: "))
   , txtCommand (new Entry ()), boxCommand (new HBox ()) {
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
   ok->clicked.connect (bind (slot (this, &XPrintDialog::command), OK));
   get_action_area ()->pack_start (*ok, false, false, 0);

   ok->set_flags (GTK_CAN_DEFAULT);

   // - CANCEL
   cancel->set_flags (GTK_CAN_DEFAULT);
   cancel->set_usize (90, 35); cancel->show ();
   cancel->clicked.connect (bind (slot (this, &XPrintDialog::command), CANCEL));
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
//Parameters: action: Id of pressed button
/*--------------------------------------------------------------------------*/
void XPrintDialog::command (commandID id) {
   TRACE9 ("XPrintDialog::command: " << id);

   switch (id) {
   case OK:
      Check3 (pCaller); Check3 (callerMethod); Check3 (txtCommand);

      if (!txtCommand->get_text_length ()) {                      // No input?
	 XMessageBox::Show (string (_("No print-command specified")),
                            XMessageBox::ERROR);
	 return;
      } // endif no input

      {
      FILE* stream (NULL);
      stream = popen (txtCommand->get_text ().c_str (), "w");
      if (!stream) {
         std::string error (_("Could not run command `%1'\nReason: %2"));
         error.replace (error.find ("%1"), 2, txtCommand->get_text ());
         error.replace (error.find ("%2"), 2, strerror (errno));
	 XMessageBox::Show (error, XMessageBox::ERROR);
	 return;
      } // endif error printing

      ofstream pipe (fileno (stream));
      (pCaller->*callerMethod) (pipe);
      pipe.close ();
      pclose (stream);
      }

   case CANCEL:
      delete this;
      break;
   } // end-switch command-id
}
