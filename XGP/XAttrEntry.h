#ifndef XATTRENTRY_H
#define XATTRENTRY_H

//$Id: XAttrEntry.h,v 1.3 2003/02/03 03:47:24 markus Exp $

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

#include <gtk--/main.h>
#include <gtk--/entry.h>

#include <XMessageBox.h>


// Class to display and change an attribute inside and entry box. The value of
// the attribute is displayed formatted when not edited.
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

   virtual gint focus_in_event_impl (GdkEventFocus* ev) {
      if (!inError)
         set_text (temp.toUnformatedString ());
      return Gtk::Entry::focus_in_event_impl (ev); }
   virtual gint focus_out_event_impl (GdkEventFocus* ev) {
      if (inError)
         return true;

      try {
         temp = get_text ();
         set_text (temp.toString ());
      }
      catch (std::invalid_argument& e) {
         inError = true;
         XMessageBox::Show (e.what (), _("Invalid value!"),
                            XMessageBox::ERROR | XMessageBox::OK);
         Gtk::Main::timeout.connect (slot (this, &XAttributeEntry::takeFocus), 10);
         return true;
      }
      return false; }

   bool inError;
   T  temp;
   T& attr_;

   int takeFocus () {
      grab_focus ();
      inError = false;
      return 0;
   }
};


// Specialication for strings
XAttributeEntry<string>::XAttributeEntry (string& attr) : attr_ (attr), temp (attr) {
   set_text (attr); }

void XAttributeEntry<string>::update () { set_text (temp = attr_); }
gint XAttributeEntry<string>::focus_in_event_impl (GdkEventFocus* ev) {
   return Gtk::Entry::focus_in_event_impl (ev); }
gint XAttributeEntry<string>::focus_out_event_impl (GdkEventFocus* ev) {
   temp = get_text (); }

#endif
