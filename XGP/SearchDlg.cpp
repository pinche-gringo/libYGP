//$Id: SearchDlg.cpp,v 1.1 2004/12/22 16:51:51 markus -Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : XGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 18.12.2004
//COPYRIGHT   : Copyright (C) 2004

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


#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>

#include <YGP/Internal.h>

#include "SearchDlg.h"


namespace XGP {

Glib::ustring SearchDialog::last;


//-----------------------------------------------------------------------------
/// (Default-)Constructor
//-----------------------------------------------------------------------------
SearchDialog::SearchDialog ()
   : XGP::XDialog (OKCANCEL) {
   set_title (_("Search"));

   Gtk::Label* lblFind (new Gtk::Label (_("_Find:"), true));
   find = new Gtk::Entry;
   lblFind->set_mnemonic_widget (*find);
   find->signal_changed ().connect (mem_fun (*this, &SearchDialog::inputChanged));

   Gtk::Box* client (new Gtk::HBox ());
   client->pack_start (*manage (lblFind), false, false, 5);
   client->pack_start (*manage (find), Gtk::PACK_EXPAND_WIDGET, 5);

   find->set_text (last);
   ok->set_sensitive (last.size ());

   get_vbox ()->pack_start (*manage (client), false, false, 5);
   show_all_children ();
   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
SearchDialog::~SearchDialog () {
}

//-----------------------------------------------------------------------------
/// Handling of the OK button; closes the dialog with commiting data
//-----------------------------------------------------------------------------
void SearchDialog::okEvent () {
   signalFind.emit (last = find->get_text ());
}

//-----------------------------------------------------------------------------
/// Callback after changing the text to find
//-----------------------------------------------------------------------------
void SearchDialog::inputChanged () {
   ok->set_sensitive (find->get_text_length ());
}

}
