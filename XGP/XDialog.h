#ifndef XDIALOG_H
#define XDIALOG_H

//$Id: XDialog.h,v 1.1 2003/01/04 08:12:09 markus Exp $

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

   void addButton (Gtk::Button& button);

 private:
   XDialog (const XDialog& other);
   const XDialog& operator= (const XDialog& other);

   void basicCommand (buttons cmd);
   void addButtons (unsigned int buttons);
   
   Gtk::Button* ok;
   Gtk::Button* cancel;
};

#endif
