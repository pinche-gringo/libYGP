#ifndef XDIALOG_H
#define XDIALOG_H

//$Id: XDialog.h,v 1.8 2003/07/25 00:24:09 markus Rel $

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


#include <gtkmm/dialog.h>


namespace Gtk {
   class Button;
}


/**Baseclass for dialogs. Installs OK and/or Cancel buttons and registers
   a command handler for them. Pressing OK is reported with the okEvent()
   method; cancel with the cancelEvent() method. The baseclase destroys the
   dialog in both cases.

   Pressing \c ESC is equal to selecting the \c CANCEL button.
*/
class XDialog : public Gtk::Dialog {
 public:
   virtual ~XDialog ();

 protected:
   typedef enum { NONE = 0, OK = 1, CANCEL = 2, OKCANCEL = (OK | CANCEL) } buttons;

   XDialog (const Glib::ustring& title, Gtk::Window& parent,
            unsigned int buttons = OKCANCEL, bool modal = false, bool use_separator = false);
   XDialog (const Glib::ustring& title, unsigned int buttons = OKCANCEL,
            bool modal = false, bool use_separator = false);
   XDialog (unsigned int buttons = OKCANCEL, bool modal = false,
            bool use_separator = false);

   void init (unsigned int buttons = OKCANCEL);

   //@Events of dialog; to be implemented
   virtual void okEvent ();
   virtual void cancelEvent ();
   virtual void command (int action);
   bool free (GdkEventAny*);

   Gtk::Button* ok;
   Gtk::Button* cancel;

 private:
   XDialog (const XDialog& other);
   const XDialog& operator= (const XDialog& other);

   void on_response (int cmd);

   void init ();
};

#endif
