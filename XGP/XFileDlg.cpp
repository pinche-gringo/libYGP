//$Id: XFileDlg.cpp,v 1.14 2003/03/06 03:11:47 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.14 $
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

#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>

#include "Trace_.h"
#include "Check.h"
#include "Internal.h"

#include "XFileDlg.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; Creates a (modeless) dialog to select a file
//Parameters: title: Text to display in the title of the dialog
//            dlgOption: Checks to perform after selecting OK
/*--------------------------------------------------------------------------*/
IFileDialog::IFileDialog (const std::string& title, option dlgOption)
   : Gtk::FileSelection (title), opt (dlgOption) {
   TRACE9 ("IFileDialog::IFileDialog (title)");
   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; Creates a (modeless) dialog to select a file
//Parameters: title: Text to display in the title of the dialog
//            dlgOption: Checks to perform after selecting OK
/*--------------------------------------------------------------------------*/
IFileDialog::IFileDialog (GtkFileSelection* castitem, option dlgOption)
   : FileSelection (castitem), opt (dlgOption) {
   TRACE9 ("IFileDialog::IFileDialog (castitem)");
   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
IFileDialog::~IFileDialog () {
   TRACE9 ("IFileDialog::~IFileDialog");
   hide ();
}


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
void IFileDialog::init () {
   Check3 (get_accel_group ());
   get_ok_button ()->signal_clicked ().connect (bind (slot (*this, &IFileDialog::command),
                                                      OK));
   get_cancel_button ()->signal_clicked ().connect (bind (slot (*this,
                                                                &IFileDialog::command),
                                                          CANCEL));
   get_cancel_button ()->add_accelerator ("clicked", get_accel_group (), XK_Escape,
                                          Gdk::ModifierType (0), Gtk::ACCEL_VISIBLE);
   modal = false;
   show ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Performs the action of the selected button
//Parameters: action: ID of pressed button
//Remarks   : - Depending on the option, the file must either exist or it is checked if it should be overwritten
/*--------------------------------------------------------------------------*/
void IFileDialog::command (commandID id) {
   TRACE9 ("IFileDialog::command: " << id);

   switch (id) {
   case OK: {
      std::string filename (get_filename ());

      if (opt != NONE) {
	 struct stat fileInfo;
	 int rc (stat (filename.c_str (), &fileInfo));         // Get fileinfo

         switch (opt) {
	 case MUST_EXIST:
            if (rc) {                // File does not exist: Show msg and exit
               std::string err (_("File `%1' does not exist!"));
               err.replace (err.find ("%1"), 2, filename);
               Gtk::MessageDialog msg (err, Gtk::MESSAGE_ERROR);
               msg.run ();
               return;
            }
            break;

         case ASK_OVERWRITE:
            if (!rc) {
               std::string msg (_("File `%1' exists! Overwrite?"));
               msg.replace (msg.find ("%1"), 2, filename);
               Gtk::MessageDialog dlg (msg, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
               if (dlg.run () != Gtk::RESPONSE_YES)
                  return;
            }
            break;
	 } // end-switch option
      } // endif option set 

      fileSelected (filename);
   }

   case CANCEL:
      if (modal) {
         if (id == CANCEL)
            modal = false;
         Gtk::Main::quit ();
      }
      else
         delete this;
      break;
   } // end-switch command-id
}

/*--------------------------------------------------------------------------*/
//Purpose   : Performs the dialog modaly
//Returns   : XDialog::OK, XDialog::CANCEL, depending on the user-input
/*--------------------------------------------------------------------------*/
std::string IFileDialog::execModal () {
   set_modal (modal = true);
   Gtk::Main::run ();
   std:: string file (modal ? get_filename () : "");
   delete this;
   return file;
}
