#ifndef XFILEDLG_H
#define XFILEDLG_H

//$Id: XFileDlg.h,v 1.22 2004/12/05 03:33:22 markus -Rel $

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
class IFileDialog : public Gtk::FileChooserDialog {
 public:
   /// Options for the dialog
   enum { NONE = 0,           ///< Don't perform any checks on the entered file
	  ASK_OVERWRITE = 1,              ///< Security question if file exists
	  MUST_EXIST = 2,           ///< Only allow to select of existing files
	  MULTIPLE = 0x80000000             ///< User can select multiple files
   };

   ~IFileDialog ();

   std::string execModal ();

   void on_response (int cmd);

   /// Creates the dialog
   static IFileDialog* create (const Glib::ustring& title,
                               Gtk::FileChooserAction action = Gtk::FILE_CHOOSER_ACTION_OPEN,
                               unsigned int dlgOption = NONE);

 protected:
   /// IDs for the possible commands (OK, CANCEL)
   typedef enum { OK = 1, CANCEL } commandID;

   /// Callback after selecting a file
   /// @param file: Name of selected file
   virtual void fileSelected (std::string& file) { }

   void free (int);

   IFileDialog (const Glib::ustring& title,
                Gtk::FileChooserAction action = Gtk::FILE_CHOOSER_ACTION_OPEN,
                unsigned int dlgOption = NONE);

 private:
   // Prohibited manager-functions
   IFileDialog (const IFileDialog&);
   const IFileDialog& operator= (const IFileDialog&);

   unsigned int opt;
   bool         modal;
};


/**Template version of the IFileDialog class to provide a (typesafe) callback
   for the user input.
*/
template <class T>
class TFileDialog : public IFileDialog {
 public:
   /// Declaration of the type of the callback for the user action
   typedef void (T::*PCALLBACK)(const std::string&);

   /// Destructor
   ~TFileDialog () { }

   /// Creates the dialog (and set it as child of the parent)
   /// \param title: Title of the dialog
   /// \param parent: Object which should be informed about the selection
   /// \param callback: Method of parent to handle information about selection 
   /// \param action: Action which is caused by clicking a file
   /// \param dlgOption: Checks to perform after selecting OK
   /// \remarks Cares also about freeing the dialog
   static TFileDialog* create (const Glib::ustring& title, T& parent,
                               const PCALLBACK callback,
                               Gtk::FileChooserAction action = Gtk::FILE_CHOOSER_ACTION_OPEN,
                               unsigned dlgOption = NONE) {
      TFileDialog<T>* dlg (new TFileDialog<T> (title, parent, callback, action, dlgOption));
      dlg->get_window ()->set_transient_for (parent.get_window ());
      dlg->signal_response ().connect (mem_fun (*dlg, &TFileDialog<T>::free));
      return dlg;
   }

 protected:
   /// Constructor; creates a (modeless) dialog to select a file
   /// \param title: Title of the dialog
   /// \param parent: Object which should be informed about the selection
   /// \param callback: Method of parent to handle information about selection 
   /// \param action: Action which is caused by clicking a file
   /// \param dlgOption: Checks to perform after selecting OK
   TFileDialog (const Glib::ustring& title, T& parent, const PCALLBACK callback,
                Gtk::FileChooserAction action = Gtk::FILE_CHOOSER_ACTION_OPEN,
                unsigned dlgOption = NONE)
      : IFileDialog (title, action, dlgOption), caller (parent)
      , callerMethod (callback) { }

 private:
   typedef enum { OK = 1, CANCEL } commandID;

   // Prohibited manager-functions
   TFileDialog (const TFileDialog&);
   const TFileDialog& operator= (const TFileDialog&);

   virtual void fileSelected (std::string& file) { (caller.*callerMethod) (file); }

   T&              caller;
   const PCALLBACK callerMethod;
};

}

#endif
