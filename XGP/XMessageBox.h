#ifndef XMESSAGEBOX_H
#define XMESSAGEBOX_H

//$Id: XMessageBox.h,v 1.1 1999/11/13 01:18:07 Markus Rel $

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


#include <vector.h>

#include <gtk--/dialog.h>

#include "SmartPtr.h"


// Forward declarations
class Gtk_HBox;
class Gtk_Label;
class Gtk_Button;
class Gtk_Pixmap;

definePtr (Gtk_Label);
definePtr (Gtk_HBox);
definePtr (Gtk_Pixmap);


// Class to display simple messages in a dialog-window. This window contains
// action-buttons and a title (if not a specific one is passed) according to
// the passed flag.
class XMessageBox : public Gtk_Dialog {
 public:
   enum options { INFO, QUESTION, WARNING, ERROR, CRITICAL, TYPEMASK = 0xf,
                  OK = 16, RETRY = 32, YES = 64, NO = 128, CANCEL = 256,
                  END = CANCEL, YESNO = YES | NO, YESNOCANCEL = YESNO | CANCEL,
                  OKCANCEL = OK | CANCEL, RETRYCANCEL = RETRY | CANCEL,
                  OKRETRYCANCEL = OK | RETRYCANCEL };

   static int show (const string& text,int flags = OK | INFO,
                    unsigned int defButton = 0) {
      show (text, "", flags, defButton); }
   static int show (const string& text, const string& title = "",
                    int flags = OK | INFO, unsigned int defButton = 0);

 protected:
   XMessageBox (const string& text, const string& title = "",
                int flags = OK | INFO, unsigned int defButton = 0);
   ~XMessageBox ();

   virtual void perform (int action);

 private:
   typedef Gtk_Dialog parent;

   // Prohibited manager-functions
   XMessageBox ();
   XMessageBox (const XMessageBox&);
   
   const XMessageBox& operator= (const XMessageBox&);

   int ret;

   vector<Gtk_Button*> buttons;
   PGtk_Label txt;
   PGtk_HBox  client;
   PGtk_Pixmap icon;
};

#endif
