#ifndef XPRINTDLG_H
#define XPRINTDLG_H

//$Id: XPrintDlg.h,v 1.4 2000/03/10 21:09:05 Markus Rel $

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

#if (GTKMM_MAJOR_VERSION > 1) || ((GTKMM_MAJOR_VERSION == 1) && GTKMM_MINOR_VERSION > 0)
namespace Gtk {
   class HBox;
   class Label;
   class Entry;
   class Button;
   class Gtk_Entry;
}

using namespace Gtk;

#else
class Gtk_HBox;
class Gtk_Label;
class Gtk_Entry;
class Gtk_Button;
class Gtk_Object;

typedef Gtk_HBox        HBox;
typedef Gtk_Label       Label;
typedef Gtk_Entry       Entry;
typedef Gtk_Button      Button;
typedef Gtk_Dialog      Dialog;
typedef Gtk_Object      Object;
#endif

typedef SmartPtr<HBox>   PHBox;
typedef SmartPtr<Label>  PLabel;
typedef SmartPtr<Entry>  PEntry;
typedef SmartPtr<Button> PButton;


// Class to select file(s) to in- or exclude; files can either entered or
// selected
class XPrintDialog : public Dialog {
 public:
   typedef void (Object::*PACTION)(ofstream&);

   XPrintDialog (Object* pNotify, const PACTION callback);
   XPrintDialog (GtkDialog* castitem, Object* pNotify,
                 const PACTION callback);
   ~XPrintDialog ();

   static XPrintDialog* perform (Object* pNotify, const PACTION callback) {
      return new XPrintDialog (pNotify, callback); }

 private:
   typedef enum { OK, CANCEL } commandID;

   // Prohibited manager-functions
   XPrintDialog (const XPrintDialog&);
   const XPrintDialog& operator= (const XPrintDialog&);

   void command (commandID id);

   void init ();

   Object*   pCaller;
   const PACTION callerMethod;

   PButton ok;
   PButton cancel;
   PLabel  lblCommand;
   PEntry  txtCommand;
   PHBox   boxCommand;
};

#endif
