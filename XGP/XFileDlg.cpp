//$Id: XFileDlg.cpp,v 1.25 2004/10/16 19:17:10 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.25 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.11.1999
//COPYRIGHT   : Copyright (C) 1999 - 2004

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

#include <gtkmm/main.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>

#include "YGP/Trace.h"
#include "YGP/Check.h"
#include "YGP/Internal.h"

#include "XGP/XFileDlg.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; Creates a (modeless) dialog to select a file
/// \param title: Text to display in the title of the dialog
/// \param action: Action which is caused by clicking a file
/// \param dlgOption: Checks to perform after selecting OK
//-----------------------------------------------------------------------------
IFileDialog::IFileDialog (const Glib::ustring& title,
                          Gtk::FileChooserAction action, option dlgOption)
   : Gtk::FileChooserDialog (title, action), opt (dlgOption) {
   TRACE9 ("IFileDialog::IFileDialog (const Glib::ustring&, Gtk::FileChooserAction, option)");

   add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
   add_button ((action == Gtk::FILE_CHOOSER_ACTION_SAVE)
               ? Gtk::Stock::SAVE : Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

   modal = false;
   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
IFileDialog::~IFileDialog () {
   TRACE9 ("IFileDialog::~IFileDialog");
   hide ();
}

//-----------------------------------------------------------------------------
/// Callback on user-action (file selected or dialog closed)
/// \param action: ID of selected action
/// \remarks - Depending on the option, the file must either exist or it is
///     checked if it should be overwritten
//-----------------------------------------------------------------------------
void IFileDialog::on_response (int cmd) {
   TRACE9 ("IFileDialog::on_response (int) - " << cmd);

   switch (cmd) {
   case Gtk::RESPONSE_OK: {
      std::string filename (get_filename ());
      TRACE8 ("IFileDialog::on_response (int) - File selected: " << filename);

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
               Glib::ustring msg (_("File `%1' exists! Overwrite?"));
               msg.replace (msg.find ("%1"), 2, filename);
               Gtk::MessageDialog dlg (msg, false, Gtk::MESSAGE_QUESTION,
                                       Gtk::BUTTONS_YES_NO);
               if (dlg.run () != Gtk::RESPONSE_YES)
                  return;
            }
            break;

         default:
            Check3 (0);
	 } // end-switch option
      } // endif option set 

      fileSelected (filename);
   }

   case Gtk::RESPONSE_CANCEL:
      if (modal) {
         modal = false;
         Gtk::Main::quit ();
      }
      break;
   } // end-switch command-id

   TRACE9 ("IFileDialog::on_response (int) - Default handler");
   Gtk::FileChooserDialog::on_response (cmd);
}

//-----------------------------------------------------------------------------
/// Performs the dialog modaly
/// \returns \c XDialog::OK, XDialog::CANCEL, depending on the user-input
//-----------------------------------------------------------------------------
std::string IFileDialog::execModal () {
   set_modal (modal = true);
   Gtk::Main::run ();
   std:: string file (modal ? get_filename () : "");
   delete this;
   return file;
}


//----------------------------------------------------------------------------
/// Creates a (modeless) dialog and registers a handler to free it after
/// deleting.
/// \param title: Title of the dialog
/// \param action: Action which is caused by clicking a file
/// \param dlgOption: Options for the dialog
//  \returns IFileDialog*: Pointer to created dialog
//----------------------------------------------------------------------------
IFileDialog* IFileDialog::create (const Glib::ustring& title,
                                  Gtk::FileChooserAction action, option dlgOption) {
   IFileDialog* dlg (new IFileDialog (title, action, dlgOption));
   dlg->signal_response ().connect (mem_fun (*dlg, &IFileDialog::free));
   return dlg;
}

//-----------------------------------------------------------------------------
/// Frees the dialog.
/// \remarks Call only if the dialog was created with new
//-----------------------------------------------------------------------------
void IFileDialog::free (int) {
   delete this;
}

}
