#ifndef XGP_XDIALOG_H
#define XGP_XDIALOG_H

//$Id: XDialog.h,v 1.17 2008/06/08 12:42:40 markus Rel $

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include <gtkmm/dialog.h>


namespace Gtk {
   class Button;
}


namespace XGP {

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
   /// Possible buttons of the dialog: Any combination of OK and/or Cancel
   typedef enum { NONE = 0,                         ///< Don't add any buttons
		  OK = 1,                                ///< Add an OK-button
		  CANCEL = 2,                         ///< Add a Cancel-button
		  OKCANCEL = (OK | CANCEL)        ///< Add OK & Cancel-buttons
   } buttons;

   XDialog (const Glib::ustring& title, Gtk::Window& parent,
            unsigned int buttons = OKCANCEL, bool modal = false);
   XDialog (const Glib::ustring& title, unsigned int buttons = OKCANCEL, bool modal = false);
   XDialog (unsigned int buttons = OKCANCEL, bool modal = false);

   void init (unsigned int buttons = OKCANCEL);

   //@Events of dialog; to be implemented
   virtual bool isDataOK ();
   virtual void okEvent ();
   virtual void cancelEvent ();
   virtual void command (int action);
   void free (int);

   void on_response (int cmd);

   Gtk::Button* ok;                                ///< OK button of the dialog
   Gtk::Button* cancel;                        ///< Cancel button of the dialog

 private:
   XDialog (const XDialog& other);
   const XDialog& operator= (const XDialog& other);

   void init ();
   void handleOK ();
};

}

#endif
