#ifndef XFILEDLG_H
#define XFILEDLG_H

//$Id: XFileDlg.h,v 1.2 1999/11/15 00:15:40 Markus Exp $

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


// Class to select file(s) to in- or exclude; files can either entered or
// selected
class XFileDialog : public Gtk_FileSelection {
 public:
   typedef void (Gtk_Object::*PACTION)(const string&);
   typedef enum { NONE, ASK_OVERWRITE, MUST_EXIST } option;

   XFileDialog (const string& title, Gtk_Object* pNotify,
                const PACTION callback, option dlgOption = NONE);
   XFileDialog (GtkFileSelection* castitem, Gtk_Object* pNotify,
                const PACTION callback, option dlgOption = NONE);
   ~XFileDialog ();

   static XFileDialog* perform (const string& title, Gtk_Object* pNotify,
				const PACTION callback, option dlgOption = NONE) {
      return new XFileDialog (title, pNotify, callback, dlgOption); }

 private:
   typedef enum { OK, CANCEL } commandID;

   // Prohibited manager-functions
   XFileDialog (const XFileDialog&);
   const XFileDialog& operator= (const XFileDialog&);

   void command (commandID id);

   void init () {
      connect_to_method (get_ok_button ()->clicked, this, &command, OK);
      connect_to_method (get_cancel_button ()->clicked, this, &command, CANCEL);
      show (); }

   option        opt;
   Gtk_Object*   pCaller;
   const PACTION callerMethod;
};

#endif
