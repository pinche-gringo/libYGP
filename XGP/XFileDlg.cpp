//$Id: XFileDlg.cpp,v 1.10 2003/02/04 05:00:18 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
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

#include <sys/stat.h>

#include <string>

#define XK_MISCELLANY
#include <X11/keysymdef.h>

#include "Trace_.h"
#include "Check.h"
#include "Internal.h"

#include "XFileDlg.h"
#include "XMessageBox.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; Creates a (modeless) dialog to select a file
//Parameters: title: Text to display in the title of the dialog
//            pNotify: Object to notify of the selection
//            callback: Method of pNotify to call after file is selected
//            dlgOption: Checks to perform after selecting OK
/*--------------------------------------------------------------------------*/
XFileDialog::XFileDialog (const string& title, Object* pNotify,
                          const PACTION callback, option dlgOption)
   : Gtk::FileSelection (title), pCaller (pNotify), callerMethod (callback)
   , opt (dlgOption) {
   TRACE9 ("XFileDialog::XFileDialog (title)");
   Check3 (pCaller); Check3 (callerMethod);

   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; Creates a (modeless) dialog to select a file
//Parameters: title: Text to display in the title of the dialog
//            pNotify: Object to notify of the selection
//            callback: Method of pNotify to call after file is selected
//            dlgOption: Checks to perform after selecting OK
/*--------------------------------------------------------------------------*/
XFileDialog::XFileDialog (GtkFileSelection* castitem, Object* pNotify,
                          const PACTION callback, option dlgOption)
   : FileSelection (castitem), pCaller (pNotify), callerMethod (callback)
   , opt (dlgOption) {
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
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
void XFileDialog::init () {
   Check3 (get_accel_group ());
   get_cancel_button ()->add_accelerator ("clicked", *get_accel_group (), XK_Escape, 0,
                                          static_cast<GtkAccelFlags> (0));

   get_ok_button ()->clicked.connect (bind (slot (this, &XFileDialog::command), OK));
   get_cancel_button ()->clicked.connect (bind (slot (this, &XFileDialog::command), CANCEL));
   show ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Performs the action of the selected button
//Parameters: action: ID of pressed button
//Remarks   : - Depending on the option, the file must either exist or it is checked if it should be overwritten
/*--------------------------------------------------------------------------*/
void XFileDialog::command (commandID id) {
   TRACE9 ("XFileDialog::command: " << id);

   switch (id) {
   case OK: {
      Check3 (pCaller); Check3 (callerMethod);
      
      string filename (get_filename ());

      if (opt != NONE) {
	 struct stat fileInfo;
	 int rc (stat (filename.c_str (), &fileInfo));         // Get fileinfo

         switch (opt) {
	 case MUST_EXIST:
            if (rc) {                // File does not exist: Show msg and exit
               std::string error (_("File `%1' does not exist!"));
               error.replace (error.find ("%1"), 2, filename);
               XMessageBox::Show (error, XMessageBox::ERROR);
               return;
            }
            break;

         case ASK_OVERWRITE:
            if (!rc) {
               std::string msg (_("File `%1' exists! Overwrite?"));
               msg.replace (msg.find ("%1"), 2, filename);
               if ((rc = XMessageBox::Show (msg, XMessageBox::QUESTION | XMessageBox::YESNO))
                   != XMessageBox::YES)
                  return;
            }
            break;
	 } // end-switch option
      } // endif option set 

      (pCaller->*callerMethod) (filename);
      }
   case CANCEL:
      delete this;
      break;
   } // end-switch command-id
}
