#ifndef XGP_XABOUT_H
#define XGP_XABOUT_H

//$Id: XAbout.h,v 1.20 2008/05/18 13:21:27 markus Rel $

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


#include <string>

#include <boost/scoped_ptr.hpp>

#include <XGP/XDialog.h>


// Forward declarations
namespace Gtk {
   class HBox;
   class Label;
   class Button;
   class Image;
}


namespace XGP {

/**Class to show a dialog with the about information for a program. It is
   assumed, that the program is distributed under the GNU public license.

   Furthermore it is possible to specify icons for programmer and application.
*/
class XAbout : public XDialog {
 public:
   ~XAbout ();

   void setIconProgram (const guint8* iconData, int lenData);
   void setIconAuthor  (const guint8* iconData, int lenData);

   static XAbout* create (const Glib::ustring& author, const Glib::ustring& program);

 protected:
   XAbout (const Glib::ustring& author, const Glib::ustring& program);

 private:
   typedef boost::scoped_ptr<Gtk::HBox>   PHBox;
   typedef boost::scoped_ptr<Gtk::Label>  PLabel;
   typedef boost::scoped_ptr<Gtk::Image>  PImage;

   // Prohibited manager-functions
   XAbout (const XAbout&);

   const XAbout& operator= (const XAbout&);

   PLabel writer;
   PLabel gpl;
   PImage pIconAuthor;
   PImage pIconProgramm;
   PHBox  client;
};

}

#endif
