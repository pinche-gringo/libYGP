#ifndef XFILEDLG_H
#define XFILEDLG_H

//$Id: XFileDlg.h,v 1.6 2000/03/10 21:09:05 Markus Exp $

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


#ifndef __STRING__
class string
#endif

#include <gtk--/fileselection.h>

#if (GTKMM_MAJOR_VERSION > 1) || ((GTKMM_MAJOR_VERSION == 1) && GTKMM_MINOR_VERSION > 0)
using namespace Gtk;

#else
class Gtk_Object;

typedef Gtk_Object         Object;
typedef Gtk_FileSelection  FileSelection;
#endif


// Class to select file(s) to in- or exclude; files can either entered or
// selected
class XFileDialog : public FileSelection {
 public:
   typedef void (Object::*PACTION)(const string&);
   typedef enum { NONE, ASK_OVERWRITE, MUST_EXIST } option;

   XFileDialog (const string& title, Object* pNotify,
                const PACTION callback, option dlgOption = NONE);
   XFileDialog (GtkFileSelection* castitem, Object* pNotify,
                const PACTION callback, option dlgOption = NONE);
   ~XFileDialog ();

   static XFileDialog* perform (const string& title, Object* pNotify,
				const PACTION callback, option dlgOption = NONE) {
      return new XFileDialog (title, pNotify, callback, dlgOption); }

 private:
   typedef enum { OK, CANCEL } commandID;

   // Prohibited manager-functions
   XFileDialog (const XFileDialog&);
   const XFileDialog& operator= (const XFileDialog&);

   void command (commandID id);

   void init () {
#if (GTKMM_MAJOR_VERSION > 1) || ((GTKMM_MAJOR_VERSION == 1) && GTKMM_MINOR_VERSION > 0)
      get_ok_button ()->clicked.connect (bind (slot (this, &command), OK));
      get_cancel_button ()->clicked.connect (bind (slot (this, &command), CANCEL));
#else
      connect_to_method (get_ok_button ()->clicked, this, &command, OK);
      connect_to_method (get_cancel_button ()->clicked, this, &command, CANCEL);
#endif
      show (); }

   option        opt;
   Object*       pCaller;
   const PACTION callerMethod;
};

#endif
