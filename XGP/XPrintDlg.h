#ifndef XPRINTDLG_H
#define XPRINTDLG_H

//$Id: XPrintDlg.h,v 1.20 2006/05/01 02:23:46 markus Rel $

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


#include <cstdio>

#include <YGP/SmartPtr.h>

#include <XGP/XDialog.h>

namespace Gtk {
   class HBox;
   class Label;
   class Entry;
   class Gtk_Entry;
}

namespace XGP {

/**Class to display a dialog to enter the command to print some text.

   This is a very basic interface; only supporting the line printer.
*/
class PrintDialog : public XDialog {
 public:
   PrintDialog ();
   virtual ~PrintDialog ();

   static PrintDialog* create ();

   /// Signal emitted, when OK is selected
   sigc::signal<void, FILE*> sigPrint;

 private:
   // Prohibited manager-functions
   PrintDialog (const PrintDialog&);
   const PrintDialog& operator= (const PrintDialog&);

   virtual void okEvent ();

   void init ();

   typedef YGP::SmartPtr<Gtk::HBox>   PHBox;
   typedef YGP::SmartPtr<Gtk::Label>  PLabel;
   typedef YGP::SmartPtr<Gtk::Entry>  PEntry;

   PLabel  lblCommand;
   PEntry  txtCommand;
   PHBox   boxCommand;
};

}

#endif
