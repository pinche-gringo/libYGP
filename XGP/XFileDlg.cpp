//$Id: XFileDlg.cpp,v 1.32 2008/03/30 13:39:17 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : XFileDlg
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.32 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.11.1999
//COPYRIGHT   : Copyright (C) 1999 - 2004, 2006, 2008

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include <sys/stat.h>

#include <string>

#include <gtkmm/main.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>

#include "YGP/Check.h"
#include "YGP/Trace.h"

#define CONVERT_TO_UTF8
#include "YGP/Internal.h"

#include "XGP/XFileDlg.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; Creates a (modeless) dialog to select a file
/// \param title Text to display in the title of the dialog
/// \param action Action which is caused by clicking a file
/// \param dlgOption Checks to perform after selecting OK
//-----------------------------------------------------------------------------
FileDialog::FileDialog (const Glib::ustring& title,
			Gtk::FileChooserAction action, unsigned int dlgOption)
   : Gtk::FileChooserDialog (title, action), sigSelected (), opt (dlgOption), modal (false) {
   TRACE9 ("FileDialog::FileDialog (const Glib::ustring&, Gtk::FileChooserAction, unsigned int)");

   add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
   add_button ((action == Gtk::FILE_CHOOSER_ACTION_SAVE)
               ? Gtk::Stock::SAVE : Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

   set_select_multiple (dlgOption & MULTIPLE);
   opt = opt & ~MULTIPLE;
   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
FileDialog::~FileDialog () {
   TRACE9 ("FileDialog::~FileDialog");
   hide ();
}

//-----------------------------------------------------------------------------
/// Callback on user-action (file selected or dialog closed)
/// \param cmd ID of selected action
/// \remarks - Depending on the option, the file must either exist or it is
///     checked if it should be overwritten
//-----------------------------------------------------------------------------
void FileDialog::on_response (int cmd) {
   TRACE9 ("FileDialog::on_response (int) - " << cmd);

   auto handleFile = [this] (std::string filename) {
      TRACE8 ("FileDialog::on_response (int) - File selected: " << filename);

      if (opt != NONE) {
	struct stat fileInfo;
	int rc(stat(filename.c_str(), &fileInfo));  // Get fileinfo

	if (opt & MUST_EXIST)
	   if (rc) {  // File does not exist: Show msg and exit
	      Glib::ustring err(_("File `%1' does not exist!"));
	      err.replace(err.find("%1"), 2, Glib::filename_to_utf8(filename));
	      Gtk::MessageDialog dlg(err, Gtk::MESSAGE_ERROR);
	      dlg.set_parent(*this);
	      dlg.run();
	      return;
	   }

	if (opt & ASK_OVERWRITE)
	   if (!rc) {
	      Glib::ustring msg(_("File `%1' exists! Overwrite?"));
	      msg.replace(msg.find("%1"), 2, Glib::filename_to_utf8(filename));
	      Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_QUESTION,
				     Gtk::BUTTONS_YES_NO);
	      dlg.set_parent(*this);
	      if (dlg.run() != Gtk::RESPONSE_YES)
		 return;
	   }
      } // endif option set

      sigSelected.emit (filename);
   };

   switch (cmd) {
   case Gtk::RESPONSE_OK: {

      if (get_select_multiple()) {
	 Glib::SListHandle<Glib::ustring> files = get_filenames();
	 for (auto i(files.begin()); i != files.end (); ++i)
	    handleFile(*i);
      }
      else
	 handleFile(get_filename());
   }  // Missing break is intentional

   case Gtk::RESPONSE_CANCEL:
      if (modal) {
         modal = false;
         Gtk::Main::quit ();
      }
      break;
   } // end-switch command-id

   TRACE9 ("FileDialog::on_response (int) - Default handler");
   Gtk::FileChooserDialog::on_response (cmd);
}

//-----------------------------------------------------------------------------
/// Performs the dialog modaly
/// \returns std::string The selected file
/// \remarks When executed modally, only the first file is returned
//-----------------------------------------------------------------------------
std::string FileDialog::execModal () {
   Check2 (!(opt % MULTIPLE));
   set_modal (modal = true);
   Gtk::Main::run ();
   std:: string file (modal ? get_filename () : "");
   delete this;
   return file;
}


//----------------------------------------------------------------------------
/// Creates a (modeless) dialog and registers a handler to free it after
/// deleting.
/// \param title Title of the dialog
/// \param action Action which is caused by clicking a file
/// \param dlgOption Options for the dialog
//  \returns FileDialog* Pointer to created dialog
//----------------------------------------------------------------------------
FileDialog* FileDialog::create (const Glib::ustring& title,
				Gtk::FileChooserAction action, unsigned int dlgOption) {
   FileDialog* dlg (new FileDialog (title, action, dlgOption));
   dlg->signal_response ().connect (mem_fun (*dlg, &FileDialog::free));
   return dlg;
}

//-----------------------------------------------------------------------------
/// Frees the dialog.
/// \remarks Call only if the dialog was created with new
//-----------------------------------------------------------------------------
void FileDialog::free (int) {
   delete this;
}

}
