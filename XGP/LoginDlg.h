#ifndef LOGINDLG_H
#define LOGINDLG_H

//$Id: LoginDlg.h,v 1.6 2006/05/01 02:23:07 markus Rel $

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


#include <gtkmm/entry.h>
#include <gtkmm/stock.h>

#include <YGP/Check.h>
#include <XGP/XDialog.h>

namespace Gtk {
   class Table;
}


namespace XGP {

/**Dialog to enter the information to perform a login (userid and password)

   The password is not displayed openly.
 */
class LoginDialog : public XGP::XDialog {
 public:
   LoginDialog (const Glib::ustring& title);
   virtual ~LoginDialog ();

   /// Creates the dialog (and set it as child of the parent)
   /// \param title: Title of the dialog
   /// \remarks Cares also about freeing the dialog
   static LoginDialog* create (const Glib::ustring& title) {
      LoginDialog* dlg (new LoginDialog (title));
      dlg->signal_response ().connect (mem_fun (*dlg, &LoginDialog::free));
      return dlg;
   }

   /// Sets the passed user in the dialog
   /// \param user: User to set
   void setUser (const Glib::ustring& user) {
      txtUser.set_text (user);
      txtUser.select_region (0, user.size ());
   }
   void setCurrentUser ();

   /// Sets the passed password in the dialog
   /// \param pwd: Password to set
   void setPassword (const Glib::ustring& pwd) {
      txtPassword.set_text (pwd);
   }

   /// Signal emitted, when Login is selected
   sigc::signal<bool, const Glib::ustring&, const Glib::ustring&> sigLogin;

 protected:
   Gtk::Table* pClient;             ///< Pointer to the client information area
   Gtk::Entry txtUser;                 ///< Textfield, where user enters the ID
   Gtk::Entry txtPassword;       ///< Textfield, where user enters the password

   enum { LOGIN };

   /// Callback after clicking on a button in the dialog
   /// \param id: ID of clicked button
   virtual void command (int id);

 private:
   // Prohibited manager functions
   LoginDialog (const LoginDialog& other);
   const LoginDialog& operator= (const LoginDialog& other);

   void inputChanged ();
};

}

#endif
