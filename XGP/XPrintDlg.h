#ifndef XGP_XPRINTDLG_H
#define XGP_XPRINTDLG_H

//$Id: XPrintDlg.h,v 1.22 2008/05/18 13:21:27 markus Rel $

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


#include <cstdio>

#include <boost/scoped_ptr.hpp>

#include <XGP/XDialog.h>

namespace Gtk {
   class Box;
   class Label;
   class Entry;
}

namespace XGP {

/**Class to display a dialog to enter the command to print some text.

   This is a very basic interface; only supporting the line printer.
*/
class PrintDialog : public XDialog {
 public:
   PrintDialog ();
   ~PrintDialog () override;

   static PrintDialog* create ();

   /// Signal emitted, when OK is selected
   sigc::signal<void (FILE*)> sigPrint;

 private:
   // Prohibited manager-functions
   PrintDialog (const PrintDialog&) = delete;
   const PrintDialog& operator= (const PrintDialog&) = delete;

   void okEvent () override;

   void init ();

   using PHBox = boost::scoped_ptr<Gtk::Box>;
   using PLabel = boost::scoped_ptr<Gtk::Label>;
   using PEntry = boost::scoped_ptr<Gtk::Entry>;

   PLabel  lblCommand;
   PEntry  txtCommand;
   PHBox   boxCommand;
};

}

#endif
