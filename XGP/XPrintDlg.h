#ifndef XPRINTDLG_H
#define XPRINTDLG_H

//$Id: XPrintDlg.h,v 1.7 2003/01/04 08:12:42 markus Exp $

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


#include <XDialog.h>

#include "SmartPtr.h"

// Forward declarations
#ifndef __STRING__
class string
#endif

class ofstream;

namespace Gtk {
   class HBox;
   class Label;
   class Entry;
   class Gtk_Entry;
}

using namespace Gtk;

typedef SmartPtr<HBox>   PHBox;
typedef SmartPtr<Label>  PLabel;
typedef SmartPtr<Entry>  PEntry;
typedef SmartPtr<Button> PButton;


// Class to display a dialog to enter the command to print some text.
//
// This is a very basic interface; only supporting the line printer.
class XPrintDialog : public XDialog {
 public:
   typedef void (Object::*PACTION)(ofstream&);

   XPrintDialog (Object* pNotify, const PACTION callback);
   XPrintDialog (GtkDialog* castitem, Object* pNotify,
                 const PACTION callback);
   ~XPrintDialog ();

   static XPrintDialog* perform (Object* pNotify, const PACTION callback) {
      return new XPrintDialog (pNotify, callback); }

 private:
   // Prohibited manager-functions
   XPrintDialog (const XPrintDialog&);
   const XPrintDialog& operator= (const XPrintDialog&);

   virtual void okEvent ();

   void init ();

   Object*   pCaller;
   const PACTION callerMethod;

   PLabel  lblCommand;
   PEntry  txtCommand;
   PHBox   boxCommand;
};

#endif
