#ifndef BROWSERDLG_H
#define BROWSERDLG_H

//$Id: BrowserDlg.h,v 1.10 2004/10/24 00:19:04 markus Exp $

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


#include <string>
#include <vector>

#include <XGP/XAttrEntry.h>

#include <XGP/XDialog.h>


namespace Gtk {
   class Box;
   class RadioButton;
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
   virtual ~BrowserDlg ();

   static BrowserDlg* create (Glib::ustring& cmd);

   /// Returns (a suggestion for) the default browser to use.
   static const char* getDefaultBrowser () { return browserNames[0]; }

 private:
   //Prohibited manager functions
   BrowserDlg (const BrowserDlg& other);
   const BrowserDlg& operator= (const BrowserDlg& other);

   void control (unsigned int cmd);
   virtual void okEvent ();

   Gtk::Box*         pboxOther;
   std::vector<Gtk::RadioButton*> aBrowsers;
   static const char*             browserNames[];

   typedef XAttributeEntry<Glib::ustring> XStringEntry;

   XStringEntry path;
};

}

#endif
