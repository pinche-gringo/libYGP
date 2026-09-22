#ifndef XGP_BROWSERDLG_H
#define XGP_BROWSERDLG_H

//$Id: BrowserDlg.h,v 1.13 2008/05/18 13:21:27 markus Rel $

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


#include <string>
#include <vector>

#include <XGP/XAttrEntry.h>

#include <XGP/XDialog.h>


namespace Gtk {
   class Box;
   class CheckButton;
}


namespace XGP {

/**Class to enable the user to select one of the (popular) UNIX browsers or to
   enter his own choice.

   The dialog does check (at configure time), if the browsers are
   actually available.

   If the gtkhtml library is available, there's also an entry which allows to
   select this widget.
 */
class BrowserDlg : public XDialog {
 public:
   BrowserDlg (Glib::ustring& cmd);
   ~BrowserDlg () override;

   static BrowserDlg* create (Glib::ustring& cmd);

   /// Returns (a suggestion for) the default browser to use.
   static const char* getDefaultBrowser () { return browserNames[0]; }

 private:
   //Prohibited manager functions
   BrowserDlg () = delete;
   BrowserDlg (const BrowserDlg& other) = delete;
   const BrowserDlg& operator= (const BrowserDlg& other) = delete;

   void control (unsigned int cmd);
   void okEvent () override;

   Gtk::Box*         pboxOther;
   std::vector<Gtk::CheckButton*> aBrowsers;
   static const char*             browserNames[];

   using XStringEntry = XAttributeEntry<Glib::ustring>;

   XStringEntry path;
};

}

#endif
