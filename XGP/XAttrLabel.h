#ifndef XATTRLABEL_H
#define XATTRLABEL_H

//$Id: XAttrLabel.h,v 1.14 2005/05/12 04:05:38 markus Rel $

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


#include <cstdio>

#include <gtkmm/label.h>


namespace XGP {

/**Class to display an attribute in a formated style while preservering its
   other abilities.

   An "attribute" is any of the standard C++ data types (except of strings, as
   they don't have a defined formattation) and the types of the YGP library
   (such as ADate, ANumeric, ...).

   This class only stores a reference to the attribute (which means it must
   not be deleted before this widget).
*/
template <class T> class XAttributeLabel : public Gtk::Label {
 public:
   /// Constructor; pass the attribute to manage
   XAttributeLabel (const T& attr)
      : Gtk::Label ("", false), attr_ (attr) { update (); }
   /// Constructor; pass the attribute to manage. It is aligned according to
   /// the values.
   XAttributeLabel (const T& attr, gfloat x, gfloat y)
      : Gtk::Label ("", x, y, false), attr_ (attr) { update (); }
   /// Constructor; pass the attribute to manage. It is aligned according to
   /// the values.
   XAttributeLabel (const T& attr, Gtk::AlignmentEnum xalign,
                    Gtk::AlignmentEnum yalign = Gtk::ALIGN_CENTER)
      : Gtk::Label ("", xalign, yalign, false), attr_ (attr) {
      update (); }
   /// Destructor
   ~XAttributeLabel () { }

   /// Actualizes the displayed value with that of the attribute
   void update () { set_text (attr_.toString ()); }
   /// Returns a reference to the handled attribute
   T& getAttribute () { return const_cast<T&> (attr_); }

 private:
   XAttributeLabel (const XAttributeLabel&);
   const XAttributeLabel& operator= (const XAttributeLabel&);

   const T& attr_;
};

// Specialization of XAttributeLabel for C++'s number types
template <> inline void XAttributeLabel<short>::update () {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel<unsigned short>::update () {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel<int>::update () {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel<unsigned int>::update () {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel<long>::update () {
   char buffer[20];
   sprintf (buffer, "%ld", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel<unsigned long>::update () {
   char buffer[20];
   sprintf (buffer, "%lu", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel<double>::update () {
   char buffer[20];
   sprintf (buffer, "%lf", attr_);
   set_text (buffer);
}


/**Class to display an attribute in a formated style while preservering its
   other abilities.

   An "attribute" is any of the standard C++ data types (except of strings, as
   they don't have a defined formattation) and the types of the YGP library
   (such as ADate, ANumeric, ...).

   This class only stores the attribute itself (meaning: the value of the
   provided attribute is copied). Use this class (instead of XAttributeLabel)
   if you only want to display something without storing the displayed value
   by yourself.
*/
template <class T> class XAttributeLabel2 : public Gtk::Label {
 public:
   /// Constructor; pass the attribute to manage. It is aligned according to
   /// the values.
   XAttributeLabel2 (const T& attr)
      : Gtk::Label ("", false), attr_ (attr) { update (); }
   /// Constructor; pass the attribute to manage. It is aligned according to
   /// the values.
   XAttributeLabel2 (const T& attr, gfloat x, gfloat y)
      : Gtk::Label ("", x, y, false), attr_ (attr) { update (); }
   /// Constructor; pass the attribute to manage. It is aligned according to
   /// the values.
   XAttributeLabel2 (const T& attr, Gtk::AlignmentEnum xalign,
                     Gtk::AlignmentEnum yalign = Gtk::ALIGN_CENTER)
      : Gtk::Label ("", xalign, yalign, false), attr_ (attr) {
      update (); }
   /// Destructor
   ~XAttributeLabel2 () { }

   /// Actualizes the displayed value with that of the attribute
   void update () { set_text (attr_.toString ()); }
   /// Returns a reference to the handled attribute
   T& getAttribute () { return const_cast<T&> (attr_); }

 private:
   XAttributeLabel2 (const XAttributeLabel2&);
   const XAttributeLabel2& operator= (const XAttributeLabel2&);

   const T attr_;
};

// Specialization of XAttributeLabel2 for C++'s number types
template <> inline void XAttributeLabel2<short>::update () {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel2<unsigned short>::update () {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel2<int>::update () {
   char buffer[20];
   sprintf (buffer, "%d", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel2<unsigned int>::update () {
   char buffer[20];
   sprintf (buffer, "%u", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel2<long>::update () {
   char buffer[20];
   sprintf (buffer, "%ld", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel2<unsigned long>::update () {
   char buffer[20];
   sprintf (buffer, "%lu", attr_);
   set_text (buffer);
}

template <> inline void XAttributeLabel2<double>::update () {
   char buffer[20];
   sprintf (buffer, "%lf", attr_);
   set_text (buffer);
}

}

#endif
