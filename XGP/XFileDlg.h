#ifndef XFILEDLG_H
#define XFILEDLG_H

//$Id: XFileDlg.h,v 1.9 2002/12/24 17:48:26 markus Rel $

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


#ifndef __STRING__
class string
#endif

#include <gtk--/fileselection.h>

using namespace Gtk;


// This class can be be used to retrieve file or directory names
// from the user. It will create a new dialog window containing a
// directory list, and a file list corresponding to the current
// working directory. The filesystem can be navigated using the
// directory list or the drop-down history menu. Alternatively, the
// TAB key can be used to navigate using filename completion - common
// in text based editors such as emacs and jed.
//
// The caller is informed via the passed callback about the selection.
//
// There are some flags to perform some checks if the OK button is selected:
//   - ASK_OVERWRITE: Displays a dialog asking if the file should be overwritten
//                    in case if the file exists.
//   - MUST_EXIST: Displays an error message indicating that the file does not
//                 exists for non-existing files.
//
// See also the description of the parent for further options!
class XFileDialog : public FileSelection {
 public:
   typedef void (Object::*PACTION)(const string&);
   typedef enum { NONE, ASK_OVERWRITE, MUST_EXIST } option;

   XFileDialog (const string& title, Object* pNotify,
                const PACTION callback, option dlgOption = NONE);
   XFileDialog (GtkFileSelection* castitem, Object* pNotify,
                const PACTION callback, option dlgOption = NONE);
   ~XFileDialog ();

   static XFileDialog* perform (const string& title, Object* pNotify,
				const PACTION callback, option dlgOption = NONE) {
      return new XFileDialog (title, pNotify, callback, dlgOption); }

 private:
   typedef enum { OK = 1, CANCEL } commandID;

   // Prohibited manager-functions
   XFileDialog (const XFileDialog&);
   const XFileDialog& operator= (const XFileDialog&);

   void command (commandID id);

   void init () {
      get_ok_button ()->clicked.connect (bind (slot (this, &XFileDialog::command), OK));
      get_cancel_button ()->clicked.connect (bind (slot (this, &XFileDialog::command), CANCEL));
      show (); }

   option        opt;
   Object*       pCaller;
   const PACTION callerMethod;
};

#endif
