//PROJECT     : libYGP
//SUBSYSTEM   : XGP
//REFERENCES  :
//TODO        :
//BUGS        :
//AUTHOR      : Markus Schwab
//CREATED     : 18.12.2004
//COPYRIGHT   : Copyright (C) 2004, 2006, 2008, 2026

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


#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include "SearchDlg.h"


namespace XGP {

Glib::ustring SearchDialog::last;


//-----------------------------------------------------------------------------
/// (Default-)Constructor
//-----------------------------------------------------------------------------
SearchDialog::SearchDialog ()
   : XGP::XDialog (OKCANCEL), signalFind (), find (Gtk::make_managed<Gtk::Entry> ()) {
   set_title (_("Search"));

   auto* lblFind (Gtk::make_managed<Gtk::Label> (_("_Find:"), true));
   lblFind->set_mnemonic_widget (*find);
   lblFind->set_margin (5);
   find->signal_changed ().connect (sigc::mem_fun (*this, &SearchDialog::inputChanged));
   find->set_hexpand ();
   find->set_margin (5);

   auto* client (Gtk::make_managed<Gtk::Box> ());
   client->append (*lblFind);
   client->append (*find);

   find->set_text (last);
   ok->set_sensitive (last.size ());

   get_content_area ()->append (*client);
   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
SearchDialog::~SearchDialog () = default;

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
