//PROJECT     : libXGP
//SUBSYSTEM   : XGP - Login Dialog
//REFERENCES  :
//TODO        :
//BUGS        :
//AUTHOR      : Markus Schwab
//CREATED     : 16.10.2004
//COPYRIGHT   : Copyright (C) 2004 - 2006, 2008, 2011, 2026

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


#include <unistd.h>

#include <gtkmm/label.h>
#include <gtkmm/grid.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include "LoginDlg.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param title Title to display for dialog
//-----------------------------------------------------------------------------
LoginDialog::LoginDialog (const Glib::ustring& title)
   : XGP::XDialog (NONE), sigLogin (), pClient (Gtk::make_managed<Gtk::Grid> ()),
     txtUser (), txtPassword () {
   set_title (title.size () ? title : _("Enter login information"));

   txtPassword.set_visibility (false);

   Gtk::Label* lbl (Gtk::make_managed<Gtk::Label> (_("_Userid:"), true));
   lbl->set_mnemonic_widget (txtUser);
   lbl->set_margin (5);
   pClient->attach (*lbl, 0, 0);

   lbl = Gtk::make_managed<Gtk::Label> (_("_Password:"), true);
   lbl->set_mnemonic_widget (txtPassword);
   lbl->set_margin (5);
   pClient->attach (*lbl, 0, 1);

   txtUser.set_hexpand ();
   txtUser.set_margin (5);
   pClient->attach (txtUser, 1, 0);

   txtPassword.set_hexpand ();
   txtPassword.set_margin (5);
   pClient->attach (txtPassword, 1, 1);

   get_content_area ()->append (*pClient);

   txtUser.signal_changed ().connect (sigc::mem_fun (*this, &LoginDialog::inputChanged));

   ok = add_button (_("_Login"), LOGIN);
   set_default_widget (*ok);

   cancel = add_button (_("_Cancel"), static_cast<int> (Gtk::ResponseType::CANCEL));

   inputChanged ();
   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
LoginDialog::~LoginDialog () {
}


//-----------------------------------------------------------------------------
/// Callback, if one of the edit-fields is changed
//-----------------------------------------------------------------------------
void LoginDialog::inputChanged () {
   Check3 (ok);
   ok->set_sensitive (txtUser.get_text_length ());
}

//-----------------------------------------------------------------------------
/// Sets the current passed user in the dialog
//-----------------------------------------------------------------------------
void LoginDialog::setCurrentUser () {
#ifdef HAVE_GETLOGIN
   Glib::ustring user;
   setUser (getlogin ());
#endif
}

//-----------------------------------------------------------------------------
/// Callback after clicking on a button in the dialog
/// \param id ID of clicked button
//-----------------------------------------------------------------------------
void LoginDialog::command (int id) {
   if (id == LOGIN) {
      Check3 (txtUser.get_text_length ());
      if (sigLogin.emit (txtUser.get_text (), txtPassword.get_text ()))
	 response (static_cast<int> (Gtk::ResponseType::OK));
   }
   else
      XDialog::command (id);
   }
}
