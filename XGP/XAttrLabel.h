#ifndef XATTRIBUTE_H
#define XATTRIBUTE_H

//$Id: XAttrLabel.h,v 1.1 2003/01/04 01:11:14 markus Exp $

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


#include <stdio.h>

#include <gtk--/label.h>


// Class to displaying an attribute in a formatted style while preservering its
// other abilities.
template <class T> class XAttributeLabel : public Gtk::Label {
 public:
   XAttributeLabel (T attr, gfloat x=0.5, gfloat y=0.5)
      : Gtk::Label (NULL, x, y), attr_ (attr) { update (); }
   ~XAttributeLabel () { }

   void update () { set_text (attr.toString ()); }
   T& getAttribute () { return attr_; }

 private:
   XAttributeLabel (const XAttributeLabel&);
   const XAttributeLabel& operator= (const XAttributeLabel&);

   T attr_;
};

// Specialization of XAttributeLabel for C++'s number types
void XAttributeLabel<short>::update () {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   set_text (buffer);
}

void XAttributeLabel<unsigned short>::update () {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   set_text (buffer);
}

void XAttributeLabel<int>::update () {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   set_text (buffer);
}

void XAttributeLabel<unsigned int>::update () {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   set_text (buffer);
}

void XAttributeLabel<long>::update () {
   char buffer[20];
   sprintf (buffer, "%ld", attr_);
   set_text (buffer);
}

void XAttributeLabel<unsigned long>::update () {
   char buffer[20];
   sprintf (buffer, "%lu", attr_);
   set_text (buffer);
}

void XAttributeLabel<double>::update () {
   char buffer[20];
   sprintf (buffer, "%lf", attr_);
   set_text (buffer);
}

#endif