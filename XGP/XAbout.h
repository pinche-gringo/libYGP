#ifndef XABOUT_H
#define XABOUT_H

//$Id: XAbout.h,v 1.14 2003/07/25 00:24:09 markus Rel $

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


#include <string>

#include <SmartPtr.h>

#include "XDialog.h"


// Forward declarations
namespace Gtk {
   class HBox;
   class Label;
   class Button;
   class Image;
}


/**Class to show a dialog with the about information for a program. It is
   assumed, that the program is distributed under the GNU public license.

   Furthermore it is possible to specify icons for programmer and application.
*/
class XAbout : public XDialog {
 public:
   XAbout (const Glib::ustring& author, const Glib::ustring& program);
   ~XAbout ();

   void setIconProgram (const char* const* iconData);
   void setIconAuthor  (const char* const* iconData);

   static XAbout* create (const Glib::ustring& author, const Glib::ustring& program);

 private:
   typedef SmartPtr<Gtk::HBox>   PHBox;
   typedef SmartPtr<Gtk::Label>  PLabel;
   typedef SmartPtr<Gtk::Image>  PImage;

   // Prohibited manager-functions
   XAbout (const XAbout&);

   const XAbout& operator= (const XAbout&);

   PLabel appl;
   PLabel writer;
   PLabel gpl;
   PImage pIconAuthor;
   PImage pIconProgramm;
   PHBox  client;
};

#endif
