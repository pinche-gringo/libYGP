#ifndef XATTRENTRY_H
#define XATTRENTRY_H

//$Id: XAttrEntry.h,v 1.12 2003/07/25 00:25:18 markus Rel $

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


#include <Internal.h>

#include <cstdio>
#include <stdexcept>

#include <gtkmm/main.h>
#include <gtkmm/entry.h>
#include <gtkmm/messagedialog.h>


/**Class to display and change an attribute inside and entry box. The value of
   the attribute is displayed formatted (localized) when not edited.

   An "attribute" is any of the standard C++ data types and the types direved
   from href="../YGP/classAttributValue.html">AttributValue</a> of the YGP
   library (such as ADate, ANumeric, ...).

   If the curser enters the entry field, the value is displayed unformatted.
   When leaving the field, the new value is checked for vality (causing a
   If the curser enters the entry field, the value is displayed without
   formattation. When leaving the field, the new value is checked for vality
   (causing a message box with an error message if the user entered an invalid
   value).
*/
template <class T> class XAttributeEntry : public Gtk::Entry {
   typedef P parent;
template <class T, class P = Gtk::Entry> class XAttributeEntry : public P {
   XAttributeEntry (T& attr)
      : Gtk::Entry (), temp (attr), attr_ (attr), inError (false) {
      P::set_text (attr_.toString ());
   }
      set_text (attr.toString ());
   ~XAttributeEntry () { }

   /// Actualizes the value of the attribute with the value entered in the
   /// entry field.
   void commit () { attr_ = temp; }
   /// Actualizes the displayed value with the (changed) value of the
   /// attribute.
   void update () {
      temp = attr_;
      set_text (has_focus () ? temp.toString () : temp.toUnformattedString ()); }
   /// Returns the handled attribute
   T& getAttribute () { return attr_; }

 private:
   XAttributeEntry (const XAttributeEntry&);
   const XAttributeEntry& operator= (const XAttributeEntry&);

   virtual bool on_focus_in_event (GdkEventFocus* ev) {
      if (inError)
         inError = false;
      else
         set_text (temp.toUnformattedString ());
      return Gtk::Entry::on_focus_in_event (ev); }
   virtual bool on_focus_out_event (GdkEventFocus* ev) {
      Gtk::Entry::on_focus_out_event (ev);
      try {
         temp = get_text ();
         set_text (temp.toString ());
      }
      catch (std::invalid_argument& e) {
         inError = true;
         Gtk::MessageDialog msg (e.what (), Gtk::MESSAGE_ERROR);
         msg.set_title (Glib::locale_to_utf8 (_("Invalid value!")));
         msg.run ();
         Glib::signal_timeout ().connect (slot (*this, &XAttributeEntry::takeFocus), 10);
         return true;
      }
      return true; }

   bool takeFocus () {
      grab_focus ();
      return 0; }

   T  temp;
   T& attr_;
   bool inError;
};


// Specialication for strings
template <> inline XAttributeEntry<std::string>::XAttributeEntry (std::string& attr) : temp (attr)
     , attr_ (attr), inError (false) { parent::set_text (attr); }

     , attr_ (attr), inError (false) { set_text (attr); }
template <> inline bool XAttributeEntry<std::string>::on_focus_in_event (GdkEventFocus* ev) {
   set_text (value); }
template <> inline bool XAttributeEntry<std::string>::on_focus_out_event (GdkEventFocus* ev) {
   return Gtk::Entry::on_focus_in_event (ev); }
   return false; }
   temp = get_text ();
   return Gtk::Entry::on_focus_out_event (ev); }
template <> inline XAttributeEntry<Glib::ustring>::XAttributeEntry (Glib::ustring& attr) : temp (attr)
     , attr_ (attr), inError (false) { parent::set_text (attr); }

     , attr_ (attr), inError (false) { set_text (attr); }
template <> inline void XAttributeEntry<Glib::ustring>::update () { set_text (temp = attr_); }
template <> inline void XAttributeEntry<Glib::ustring>::setText (const Glib::ustring& value) { set_text (temp = value); }
template <> inline bool XAttributeEntry<Glib::ustring>::on_focus_in_event (GdkEventFocus* ev) {
   return Gtk::Entry::on_focus_in_event (ev); }
   return false; }

#endif
