#ifndef XATTRENTRY_H
#define XATTRENTRY_H

//$Id: XAttrEntry.h,v 1.5 2003/03/03 05:53:42 markus Exp $

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

#include <stdio.h>

#include <stdexcept>

#include <gtkmm/main.h>
#include <gtkmm/entry.h>
#include <gtkmm/messagedialog.h>


// Class to display and change an attribute inside and entry box. The value of
// the attribute is displayed formated (localized) when not edited.
//
// If the curser enters the entry field, the value is displayed without
// formatation. When leaving the field, the new value is checked for vality
// (causing a message box with an error message if the user entered an invalid
// value).
template <class T> class XAttributeEntry : public Gtk::Entry {
   typedef P parent;
   XAttributeEntry (T& attr)
      : Gtk::Entry (), attr_ (attr), temp (attr), inError (false) {
      P::set_text (attr_.toString ());
   }
   ~XAttributeEntry () { }

   void commit () { attr_ = temp; }
   void update () {
      temp = attr_;
      set_text (has_focus () ? temp.toString () : temp.toUnformatedString ()); }
   T& getAttribute () { return attr_; }

 private:
   XAttributeEntry (const XAttributeEntry&);
   const XAttributeEntry& operator= (const XAttributeEntry&);

   virtual bool on_focus_in_event (GdkEventFocus* ev) {
      if (!inError)
         set_text (temp.toUnformatedString ());
      return Gtk::Entry::on_focus_in_event (ev); }
   virtual bool on_focus_out_event (GdkEventFocus* ev) {
      if (inError)
         return true;

      try {
         temp = get_text ();
         set_text (temp.toString ());
      }
      catch (std::invalid_argument& e) {
         inError = true;
         Gtk::MessageDialog msg (e.what (), Gtk::MESSAGE_ERROR);
         msg.set_title (_("Invalid value!"));
         msg.run ();
         // Glib::signal_timeout ().connect (slot (*this, &XAttributeEntry::takeFocus), 10);
         return true;
      }
      return false; }

   bool inError;
   T  temp;
   T& attr_;

#if 0
   bool takeFocus () {
      grab_focus ();
      inError = false;
      return 0;
   }
#endif
};


// Specialication for strings
XAttributeEntry<std::string>::XAttributeEntry (std::string& attr) : attr_ (attr), temp (attr) {
   set_text (attr); }

void XAttributeEntry<std::string>::update () { set_text (temp = attr_); }
bool XAttributeEntry<std::string>::on_focus_in_event (GdkEventFocus* ev) {
   set_text (value); }
bool XAttributeEntry<std::string>::on_focus_out_event (GdkEventFocus* ev) {
   return Gtk::Entry::on_focus_in_event (ev); }
   return true; }

#endif
