#ifndef XABOUT_H
#define XABOUT_H

//$Id: XAbout.h,v 1.10 2003/02/03 03:50:33 markus Exp $

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

// Forward declarations
namespace Gtk {
   class HBox;
   class Label;
   class Button;
   class Pixmap;
}


// Class to show a dialog with about-information for a program. It is assumed,
// that the program is distributed under the GNU public license.
//
// Furthermore it is possible to specify icons for programmer and application.
class XAbout : public XDialog {
 public:
   XAbout (const string& author, const string& program);
   ~XAbout ();

   void setIconProgram (const char* const* iconData);
   void setIconAuthor  (const char* const* iconData);

 private:
   typedef SmartPtr<Gtk::HBox>    PHBox;
   typedef SmartPtr<Gtk::Label>   PLabel;
   typedef SmartPtr<Gtk::Pixmap>  PPixmap;

   // Prohibited manager-functions
   XAbout (const XAbout&);

   const XAbout& operator= (const XAbout&);

   PLabel  appl;
   PLabel  writer;
   PLabel  gpl;
   PPixmap pIconAuthor;
   PPixmap pIconProgramm;
   PHBox   client;
};

#endif
