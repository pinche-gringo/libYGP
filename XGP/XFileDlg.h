#ifndef XFILEDLG_H
#define XFILEDLG_H

//$Id: XFileDlg.h,v 1.13 2003/03/03 05:53:43 markus Exp $

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

#include <gtkmm/fileselection.h>


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
class IFileDialog : public Gtk::FileSelection {
 public:
   typedef enum { NONE, ASK_OVERWRITE, MUST_EXIST } option;

   IFileDialog (const std::string& title, option dlgOption = NONE);
   IFileDialog (GtkFileSelection* castitem, option dlgOption = NONE);
   ~IFileDialog ();

   std::string execModal ();

   static IFileDialog* perform (const std::string& title, option dlgOption = NONE) {
      return new IFileDialog (title, dlgOption); }

 protected:
   typedef enum { OK = 1, CANCEL } commandID;

   virtual void fileSelected (std::string& file) { }

 private:
   // Prohibited manager-functions
   IFileDialog (const IFileDialog&);
   const IFileDialog& operator= (const IFileDialog&);

   void init ();
   void command (commandID id);

   option opt;
   bool   modal;
};


template <class T>
class TFileDialog : public IFileDialog {
 public:
   typedef void (T::*PACTION)(const std::string&);

   TFileDialog (const std::string& title, T* pNotify,
                const PACTION callback, option dlgOption = NONE)
      : IFileDialog (title, dlgOption), pCaller (pNotify)
      , callerMethod (callback) { }
   TFileDialog (GtkFileSelection* castitem, T* pNotify,
                const PACTION callback, option dlgOption = NONE)
      : IFileDialog (castitem, dlgOption), pCaller (pNotify)
      , callerMethod (callback) { }
   ~TFileDialog () { }

   static TFileDialog* perform (const std::string& title, T* pNotify,
				const PACTION callback, option dlgOption = NONE) {
      return new TFileDialog (title, pNotify, callback, dlgOption); }

 private:
   typedef enum { OK = 1, CANCEL } commandID;

   // Prohibited manager-functions
   TFileDialog (const TFileDialog&);
   const TFileDialog& operator= (const TFileDialog&);

   virtual void fileSelected (std::string& file) { (pCaller->*callerMethod) (file); }

   T*            pCaller;
   const PACTION callerMethod;
};


typedef TFileDialog<Gtk::Object> XFileDialog;

#endif
