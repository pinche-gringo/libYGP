#ifndef XABOUT_H
#define XABOUT_H

//$Id: XAbout.h,v 1.6 2000/03/11 15:06:12 Markus Rel $

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

// Forward declarations
#if (GTKMM_MAJOR_VERSION > 1) || ((GTKMM_MAJOR_VERSION == 1) && GTKMM_MINOR_VERSION > 0)
namespace Gtk {
   class HBox;
   class Label;
   class Button;
   class Pixmap;
}

using namespace Gtk;

#else
class Gtk_HBox;
class Gtk_Label;
class Gtk_Widget;
class Gtk_Button;
class Gtk_Pixmap;

typedef Gtk_HBox    HBox;
typedef Gtk_Label   Label;
typedef Gtk_Widget  Widget;
typedef Gtk_Button  Button;
typedef Gtk_Pixmap  Pixmap;
typedef Gtk_Dialog  Dialog;
#endif


// Class to show a dialog with about-information for a program. It is assumed,
// that the program is distributed under the GNU public license.
// It is possible to specify icons for programmer and application.
class XAbout : public Dialog {
 public:
   XAbout (const string& author, const string& program);
   ~XAbout ();

   void setIconProgram (const char* const* iconData);
   void setIconAuthor  (const char* const* iconData);

 private:
   typedef SmartPtr<HBox>    PHBox;
   typedef SmartPtr<Label>   PLabel;
   typedef SmartPtr<Button>  PButton;
   typedef SmartPtr<Pixmap>  PPixmap;

   // Prohibited manager-functions
   XAbout (const XAbout&);

   const XAbout& operator= (const XAbout&);

   void command (int) {
      delete this;
   }

   PButton ok;
   PLabel  appl;
   PLabel  writer;
   PLabel  gpl;
   PPixmap pIconAuthor;
   PPixmap pIconProgramm;
   PHBox   client;
};

#endif
