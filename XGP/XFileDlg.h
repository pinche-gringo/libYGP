#ifndef XFILEDLG_H
#define XFILEDLG_H

//$Id: XFileDlg.h,v 1.24 2008/03/23 22:12:55 markus Exp $

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

#include <gtkmm/filechooserdialog.h>

namespace XGP {

/**This class can be be used to retrieve file or directory names from the
   user. It will create a new dialog window containing a directory list, and a
   file list corresponding to the current working directory. The filesystem
   can be navigated using the directory list or the drop-down history
   menu. Alternatively, the TAB key can be used to navigate using filename
   completion - common in text based editors such as emacs and jed.

   The caller is informed via the passed callback about the selection.

   There are some flags to perform some checks if the \c OK button is selected:
     - ASK_OVERWRITE: Displays a dialog asking if the file should be overwritten
                      in case if the file exists.
     - MUST_EXIST: Displays an error message indicating that the file does not
                   exists for non-existing files.

     See also the description of the parent for further options!
*/
class FileDialog : public Gtk::FileChooserDialog {
 public:
   /// Options for the dialog
   enum { NONE = 0,           ///< Don't perform any checks on the entered file
	  ASK_OVERWRITE = 1,              ///< Security question if file exists
	  MUST_EXIST = 2,           ///< Only allow to select of existing files
	  MULTIPLE = 0x80000000             ///< User can select multiple files
   };

   ~FileDialog ();

   std::string execModal ();

   void on_response (int cmd);

   /// Creates the dialog
   static FileDialog* create (const Glib::ustring& title,
			      Gtk::FileChooserAction action = Gtk::FILE_CHOOSER_ACTION_OPEN,
			      unsigned int dlgOption = NONE);

   /// Signal emitted, when OK is selected
   sigc::signal<void, const std::string&> sigSelected;

 protected:
   /// IDs for the possible commands (OK, CANCEL)
   typedef enum { OK = 1, CANCEL } commandID;

   /// Callback after selecting a file
   /// @param: Name of selected file
   virtual void fileSelected (std::string&) { }

   void free (int);

   FileDialog (const Glib::ustring& title,
	       Gtk::FileChooserAction action = Gtk::FILE_CHOOSER_ACTION_OPEN,
	       unsigned int dlgOption = NONE);

 private:
   // Prohibited manager-functions
   FileDialog (const FileDialog&);
   const FileDialog& operator= (const FileDialog&);

   unsigned int opt;
   bool         modal;
};

}

#endif
