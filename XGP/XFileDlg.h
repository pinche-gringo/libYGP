#ifndef XFILEDLG_H
#define XFILEDLG_H

//$Id: XFileDlg.h,v 1.8 2002/04/11 18:33:46 markus Rel $

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


// Class to select file(s) to in- or exclude; files can either entered or
// selected
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
