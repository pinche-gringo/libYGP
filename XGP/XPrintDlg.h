#ifndef XPRINTDLG_H
#define XPRINTDLG_H

//$Id: XPrintDlg.h,v 1.1 1999/11/15 00:17:16 Markus Rel $

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

#include <gtk--/dialog.h>

#include "SmartPtr.h"

// Forward declarations
#ifndef __STRING__
class string
#endif

class ofstream;

class Gtk_HBox;
class Gtk_Label;
class Gtk_Entry;
class Gtk_Button;

definePtr (Gtk_HBox);
definePtr (Gtk_Label);
definePtr (Gtk_Entry);
definePtr (Gtk_Button);


// Class to select file(s) to in- or exclude; files can either entered or
// selected
class XPrintDialog : public Gtk_Dialog {
 public:
   typedef void (Gtk_Object::*PACTION)(ofstream&);

   XPrintDialog (Gtk_Object* pNotify, const PACTION callback);
   XPrintDialog (GtkDialog* castitem, Gtk_Object* pNotify,
                 const PACTION callback);
   ~XPrintDialog ();

   static XPrintDialog* perform (Gtk_Object* pNotify, const PACTION callback) {
      return new XPrintDialog (pNotify, callback); }

 private:
   typedef enum { OK, CANCEL } commandID;

   // Prohibited manager-functions
   XPrintDialog (const XPrintDialog&);
   const XPrintDialog& operator= (const XPrintDialog&);

   void command (commandID id);

   void init ();

   Gtk_Object*   pCaller;
   const PACTION callerMethod;

   PGtk_Button ok;
   PGtk_Button cancel;
   PGtk_Label  lblCommand;
   PGtk_Entry  txtCommand;
   PGtk_HBox   boxCommand;
};

#endif
