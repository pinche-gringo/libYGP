//$Id: XFileDlg.cpp,v 1.1 1999/11/14 15:50:37 Markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileDlg
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


#include <string>

#define DEBUG 0
#include "Trace.h"
#include "Check.h"

#include "XFileDlg.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; connects the controls with methods
//Parameters: title: Title of dialog
//            pNotify: Gtk_Object to notify of selected file
//            callback: Method of pNotify to call after file is selected
/*--------------------------------------------------------------------------*/
XFileDialog::XFileDialog (const string& title, Gtk_Object* pNotify,
                          PFILEDIALOGACTION callback)
   : Gtk_FileSelection (title), pCaller (pNotify), callerMethod (callback) {
   TRACE9 ("XFileDialog::XFileDialog (title)");
   Check3 (pCaller); Check3 (callerMethod);

   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; connects the controls with methods
//Parameters: title: Title of dialog
//            pNotify: Gtk_Object to notify of selected file
//            callback: Method of pNotify to call after file is selected
/*--------------------------------------------------------------------------*/
XFileDialog::XFileDialog (GtkFileSelection* castitem, Gtk_Object* pNotify,
                          PFILEDIALOGACTION callback)
   : Gtk_FileSelection (castitem), pCaller (pNotify), callerMethod (callback) {
   TRACE9 ("XFileDialog::XFileDialog (castitem)");
   Check3 (pCaller); Check3 (callerMethod);

   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XFileDialog::~XFileDialog () {
   TRACE9 ("XFileDialog::~XFileDialog");
   hide ();
}


/*--------------------------------------------------------------------------*/
//Purpose   : Performs the action of the selected button
//Parameters: action: ID of pressed button
/*--------------------------------------------------------------------------*/
void XFileDialog::command (commandID id) {
   TRACE9 ("XFileDialog::command: " << id);

   switch (id) {
   case OK:
      Check3 (pCaller); Check3 (callerMethod);
      (pCaller->*callerMethod) (get_filename ());

   case CANCEL:
      delete_self ();
      break;
   } // end-switch command-id
}
