#ifndef XGP_TRACEDLG_H
#define XGP_TRACEDLG_H

//$Id: TraceDlg.h,v 1.3 2008/05/18 13:21:27 markus Rel $

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
   class TreeView;
}


namespace XGP {

/**Class to display a dialog to change the level of registered trace-objects.

   This dialog is available from the Help-menu of the application, if
   addHelpMenu() is called with <tt>withDynTrace</tt> set to true.
 */
class TraceDlg : public XGP::XDialog {
 public:
   TraceDlg ();
   virtual ~TraceDlg ();

   /// Creates a dialog
   static TraceDlg* create () {
      TraceDlg* dlg (new TraceDlg);
      dlg->signal_response ().connect (mem_fun (*dlg, &TraceDlg::free));
      return dlg;
   }

 private:
   //Prohibited manager functions
   TraceDlg (const TraceDlg& other);
   const TraceDlg& operator= (const TraceDlg& other);

   void levelChanged (const Glib::ustring& path, const Glib::ustring& value);

   virtual void okEvent ();
   Gtk::TreeView& lstObjects;
};

}

#endif
