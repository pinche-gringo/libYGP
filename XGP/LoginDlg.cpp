//$Id: LoginDlg.cpp,v 1.11 2008/03/30 13:39:17 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : XGP - Login Dialog
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.11 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.10.2004
//COPYRIGHT   : Copyright (C) 2004 - 2006, 2008, 2011

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
#include <gtkmm/table.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include "LoginDlg.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param title Title to display for dialog
//-----------------------------------------------------------------------------
LoginDialog::LoginDialog (const Glib::ustring& title)
   : XGP::XDialog (NONE), sigLogin (), pClient (new Gtk::Table (2, 2)), txtUser (), txtPassword () {
   set_title (title.size () ? title : _("Enter login information"));

   pClient->show ();

   txtPassword.set_visibility (false);

   Gtk::Label* lbl (new Gtk::Label (_("_Userid:"), true));
   lbl->set_mnemonic_widget (txtUser);
   pClient->attach (*manage (lbl), 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 5, 5);

   lbl = new Gtk::Label (_("_Password:"), true);
   lbl->set_mnemonic_widget (txtPassword);
   pClient->attach (*manage (lbl), 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 5, 5);
   pClient->attach (txtUser, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND,
		    Gtk::FILL | Gtk::EXPAND, 5, 5);
   pClient->attach (txtPassword, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND,
		    Gtk::FILL | Gtk::EXPAND, 5, 5);

   get_vbox ()->pack_start (*manage (pClient), false, false, 5);

   txtUser.signal_changed ().connect (mem_fun (*this, &LoginDialog::inputChanged));

   ok = new Gtk::Button (_("_Login"), true);
   get_action_area ()->pack_start (*ok, false, false, 5);
   ok->set_can_default ();
   ok->grab_default ();
   ok->signal_clicked ().connect
       (sigc::bind<int> (sigc::mem_fun (*this, &LoginDialog::command), LOGIN));

   cancel = add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

   inputChanged ();
   show_all_children ();
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
	 response (Gtk::RESPONSE_OK);
   }
   else
      XDialog::command (id);
   }
}
