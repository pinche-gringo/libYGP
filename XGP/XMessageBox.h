#ifndef XMESSAGEBOX_H
#define XMESSAGEBOX_H

//$Id: XMessageBox.h,v 1.3 2000/02/24 22:16:36 Markus Exp $

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
#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
namespace Gtk {
   class HBox;
   class Label;
   class Pixmap;
   class Button;
}
   
using namespace Gtk;

#else 
typedef Gtk_HBox        HBox;
typedef Gtk_Label       Label;
typedef Gtk_Button      Button;
typedef Gtk_Pixmap      Pixmap;
typedef Gtk_Dialog      Dialog;
#endif

typedef SmartPtr<Label>  PLabel;
typedef SmartPtr<HBox>   PHBox;
typedef SmartPtr<Pixmap> PPixmap;


// Class to display simple messages in a dialog-window. This window contains
// action-buttons and a title (if not a specific one is passed) according to
// the passed flag.
class XMessageBox : public Dialog {
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
   // Prohibited manager-functions
   XMessageBox ();
   XMessageBox (const XMessageBox&);
   
   const XMessageBox& operator= (const XMessageBox&);

   int ret;

   vector<Button*> buttons;
   PLabel txt;
   PHBox  client;
   PPixmap icon;
};

#endif
