#ifndef XABOUT_H
#define XABOUT_H

//$Id: XAbout.h,v 1.1 1999/11/13 01:18:07 Markus Exp $

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

class Gtk_HBox;
class Gtk_Label;
class Gtk_Button;
class Gdk_Pixmap;
class Gtk_Pixmap;

definePtr (Gtk_HBox);
definePtr (Gtk_Label);
definePtr (Gtk_Button);
definePtr (Gtk_Pixmap);


// Class to show a dialog with about-information for a program. It is assumed,
// that the program is distributed under the GNU public license.
// It is possible to specify icons for programmer and application.
class XAbout : public Gtk_Dialog {
 public:
   XAbout (const string& author, const string& program);
   ~XAbout ();

   void setIconProgram (Gdk_Pixmap& icon);
   void setIconAuthor  (Gdk_Pixmap& icon);

 private:
   // Prohibited manager-functions
   XAbout (const XAbout&);

   const XAbout& operator= (const XAbout&);

   void command (int) {
      delete_self ();
   }

   PGtk_Button ok;
   PGtk_Label  appl;
   PGtk_Label  writer;
   PGtk_Label  gpl;
   PGtk_HBox   client;
   PGtk_Pixmap iconPrg;
   PGtk_Pixmap iconAuthor;
};

#endif
