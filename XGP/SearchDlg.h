#ifndef XGP_SEARCHDLG_H
#define XGP_SEARCHDLG_H

//$Id: SearchDlg.h,v 1.4 2008/05/18 13:21:27 markus Rel $

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

   /// Creates a search-dialog
   /// \param parent: Parent window
   static SearchDialog* create (const Glib::RefPtr<Gdk::Window>& parent) {
      SearchDialog* dlg (new SearchDialog);
      dlg->get_window ()->set_transient_for (parent);
      dlg->signal_response ().connect (mem_fun (*dlg, &SearchDialog::free));
      return dlg;
   }

   /// Signal emitted, when OK is selected
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
