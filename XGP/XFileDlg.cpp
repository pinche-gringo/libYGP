//$Id: XFileDlg.cpp,v 1.4 2000/02/24 22:16:35 Markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
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

#include <sys/stat.h>

#include <string>

#define DEBUG 0
#include "Trace_.h"
#include "Check.h"

#include "XFileDlg.h"
#include "XMessageBox.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; connects the controls with methods
//Parameters: title: Title of dialog
//            pNotify: Object to notify of selected file
//            callback: Method of pNotify to call after file is selected
//            dlgOption: Checks to perform after OK-event
/*--------------------------------------------------------------------------*/
XFileDialog::XFileDialog (const string& title, Object* pNotify,
                          const PACTION callback, option dlgOption)
   : FileSelection (title), pCaller (pNotify), callerMethod (callback)
   , opt (dlgOption) {
   TRACE9 ("XFileDialog::XFileDialog (title)");
   Check3 (pCaller); Check3 (callerMethod);

   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; connects the controls with methods
//Parameters: title: Title of dialog
//            pNotify: Object to notify of selected file
//            callback: Method of pNotify to call after file is selected
//            dlgOption: Checks to perform after OK-event
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
//Purpose   : Performs the action of the selected button
//Parameters: action: ID of pressed button
//Remarks   : Depending on the option, the file must either exist or it is
//            checked if it should be overwritten
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
               XMessageBox::show (string ("File '") + filename
                                  + string ("' does not exist!"),
                                  XMessageBox::ERROR);
               return;
            }
            break;

         case ASK_OVERWRITE:
            if (!rc)
               if ((rc = XMessageBox::show (string ("File '") + filename
                                      + string ("' exists! Overwrite?"),
                                     XMessageBox::QUESTION | XMessageBox::YESNO))
                   != XMessageBox::YES)
                  return;
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
