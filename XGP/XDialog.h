#ifndef XDIALOG_H
#define XDIALOG_H

//$Id: XDialog.h,v 1.2 2003/01/09 03:57:56 markus Exp $

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


#include <gtk--/dialog.h>


namespace Gtk {
   class Button;
}


// Baseclass for dialogs; Installs OK and/or Cancel buttons and registers
// a command handler for them. Pressing OK is reported with the okEvent-
// method; cancel with the cancelEvent-method. The baseclase destroys the
// dialog in both cases.
//
// Note that the OK button is added at the beginning and CANCEL at the end
// of the action area.
//
// Pressing ESC is equal to selecting the CANCEL button.
class XDialog : public Gtk::Dialog {
 public:
   virtual ~XDialog ();

 protected:
   typedef enum { NONE = 0, OK = 1, CANCEL = 2, OKCANCEL = (OK | CANCEL) } buttons;
   XDialog (unsigned int buttons = OKCANCEL);
   XDialog (GtkDialog*, unsigned int buttons = OKCANCEL);

   //@Events of dialog; to be implemented
   virtual void okEvent ();
   virtual void cancelEvent ();

   void addButton (Gtk::Button& button, bool start = true);

   Gtk::Button* ok;
   Gtk::Button* cancel;

 private:
   XDialog (const XDialog& other);
   const XDialog& operator= (const XDialog& other);

   void basicCommand (buttons cmd);
   void addButtons (unsigned int buttons);
};

#endif
