//$Id: LoginDlg.cpp,v 1.1 2004/10/16 06:24:56 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : XGP - Login Dialog
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.10.2004
//COPYRIGHT   : Copyright (C) 2004, 2005

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


#include <gtkmm/label.h>
#include <gtkmm/table.h>

#include "LoginDlg.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// (Default-)Constructor
/// \param title: Title to display for dialog
//-----------------------------------------------------------------------------
ILoginDialog::ILoginDialog (const Glib::ustring& title)
   : XGP::XDialog (OKCANCEL), pClient (new Gtk::Table (2, 2)) {
   set_title (title);

   pClient->show ();

   txtUser = manage (new Gtk::Entry ());
   txtPassword = manage (new Gtk::Entry ());
   txtPassword->set_visibility (false);

   pClient->attach (*manage (new Gtk::Label (_("Userid:"))), 0, 1, 0, 1,
		    Gtk::SHRINK, Gtk::SHRINK, 5, 5);
   pClient->attach (*manage (new Gtk::Label (_("Password:"))), 0, 1, 1, 2,
		    Gtk::SHRINK, Gtk::SHRINK, 5, 5);
   pClient->attach (*txtUser, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND,
		    Gtk::FILL | Gtk::EXPAND, 5, 5);
   pClient->attach (*txtPassword, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND,
		    Gtk::FILL | Gtk::EXPAND, 5, 5);

   get_vbox ()->pack_start (*pClient, false, false, 5);

   txtUser->signal_changed ().connect (mem_fun (*this, &ILoginDialog::inputChanged));
   txtPassword->signal_changed ().connect (mem_fun (*this, &ILoginDialog::inputChanged));

   inputChanged ();
   show_all_children ();
   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ILoginDialog::~ILoginDialog () {
   delete pClient;
}


//-----------------------------------------------------------------------------
/// Callback, if one of the edit-fields is changed
//-----------------------------------------------------------------------------
void ILoginDialog::inputChanged () {
   Check3 (ok); Check3 (txtUser); Check3 (txtPassword);
   ok->set_sensitive (txtUser->get_text_length ()
		      && txtPassword->get_text_length ());
}

}
