#ifndef XPRINTDLG_H
#define XPRINTDLG_H

//$Id: XPrintDlg.h,v 1.13 2003/07/20 08:17:00 markus Rel $

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


#include <iosfwd>

#include <XDialog.h>

#include "SmartPtr.h"

namespace Gtk {
   class HBox;
   class Label;
   class Entry;
   class Gtk_Entry;
}

/**Class to display a dialog to enter the command to print some text.

   This is a very basic interface; only supporting the line printer.
*/
class IPrintDialog : public XDialog {
 public:
   IPrintDialog ();
   ~IPrintDialog ();

   /// Method to display the dialog
   static IPrintDialog* perform () { return new IPrintDialog (); }

   typedef SmartPtr<Gtk::HBox>   PHBox;
   typedef SmartPtr<Gtk::Label>  PLabel;
   typedef SmartPtr<Gtk::Entry>  PEntry;
   typedef SmartPtr<Gtk::Button> PButton;

 private:
   // Prohibited manager-functions
   IPrintDialog (const IPrintDialog&);
   const IPrintDialog& operator= (const IPrintDialog&);

   virtual void okEvent ();
   /// Print to the passed stream.
   /// \param stream: Stream to write to (for printing)
   virtual void printToStream (std::ostream& stream) { }

   void init ();

   PLabel  lblCommand;
   PEntry  txtCommand;
   PHBox   boxCommand;
};


/**Templated version of the IPrintDialog which allows a (typesafe)
   notification of the caller about user input.
*/
template <class T>
class TPrintDialog : public IPrintDialog {
 public:
   typedef void (T::*PCALLBACK)(std::ostream&);

   /// Constructor
   /// \param parent: Window to notify of the print command
   /// \param callback: Method of \c parent to call for printing
   TPrintDialog (T& parent, PCALLBACK callback) : IPrintDialog ()
       , pCaller (parent), callerMethod (callback) { }
   /// Destructor
   ~TPrintDialog () { }

   /// Method to display the dialog
   /// \param parent: Window to notify of the print command
   /// \param callback: Method of \c parent to call for printing
   static TPrintDialog* perform (T& parent, PCALLBACK callback) {
      TPrintDialog<T>* dlg (new TPrintDialog<T> (parent, callback));
      dlg->get_window ()->set_transient_for (parent.get_window ());
      return dlg;
   }

 protected:
   /// Print to the passed stream.
   /// \param stream: Stream to write to (for printing)
   virtual void printToStream (std::ostream& stream) {
       (pCaller.*callerMethod) (stream);
   }

 private:
   T& pCaller;
   PCALLBACK callerMethod;
};


#endif
