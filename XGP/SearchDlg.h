#ifndef SEARCHDLG_H
#define SEARCHDLG_H

//$Id: SearchDlg.h,v 1.1 2004/12/22 16:51:51 markus Exp $

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


#include <XGP/XDialog.h>

namespace Gtk {
   class Entry;
}


namespace XGP {

/**Dialog to enter a search string.
 *
 * The dialog emits a signal with the entered search-text.
 */
class SearchDialog : public XGP::XDialog {
 public:
   virtual ~SearchDialog ();

   static SearchDialog* create (const Glib::RefPtr<Gdk::Window>& parent) {
      SearchDialog* dlg (new SearchDialog);
      dlg->get_window ()->set_transient_for (parent);
      dlg->signal_response ().connect (mem_fun (*dlg, &SearchDialog::free));
      return dlg;
   }

   sigc::signal<void, const Glib::ustring&> signalFind;

 protected:
   SearchDialog ();

 private:
   //Prohibited manager functions
   SearchDialog (const SearchDialog& other);
   const SearchDialog& operator= (const SearchDialog& other);

   virtual void okEvent ();

   void inputChanged ();

   Gtk::Entry* find;

   static Glib::ustring last;
};

}

#endif
