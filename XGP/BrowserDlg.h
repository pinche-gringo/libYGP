#ifndef BROWSERDLG_H
#define BROWSERDLG_H

//$Id: BrowserDlg.h,v 1.2 2003/01/14 22:40:29 markus Exp $

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

#include <XAttrEntry.h>

#include <XDialog.h>

namespace Gtk {
   class Box;
   class RadioButton;
}


class BrowserDlg : public XDialog {
 public:
   BrowserDlg (string& cmd);
   virtual ~BrowserDlg ();

   static BrowserDlg* perform (string& cmd) {
      return new BrowserDlg (cmd); }

 private:
   //Prohibited manager functions
   BrowserDlg (const BrowserDlg& other);
   const BrowserDlg& operator= (const BrowserDlg& other);

   void control (unsigned int cmd);
   virtual void okEvent ();

   Gtk::Box*         pboxOther;
   Gtk::RadioButton* aBrowsers[5];
   static const char* browserNames[4];

   typedef XAttributeEntry<string> XStringEntry;

   XStringEntry path;
};

#endif
