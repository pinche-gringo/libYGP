#ifndef XATTRENTRY_H
#define XATTRENTRY_H

//$Id: XAttrEntry.h,v 1.1 2003/01/09 04:04:34 markus Exp $

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

#include <gtk--/entry.h>

#include <XMessageBox.h>


// Class to display and change an attribute inside and entry box. The value of
// the attribute is displayed formatted when not edited.
template <class T> class XAttributeEntry : public Gtk::Entry {
   typedef P parent;
   XAttributeEntry (T& attr)
      : Gtk::Entry (), attr_ (attr) { update (); }
   ~XAttributeEntry () { }

   void update () { set_text (attr_.toString ()); }
   T& getAttribute () { return attr_; }

 private:
   XAttributeEntry (const XAttributeEntry&);
   const XAttributeEntry& operator= (const XAttributeEntry&);

   virtual gint focus_in_event_impl (GdkEventFocus*) {
      set_text (attr.toUformattedString ()); }
   virtual gint focus_out_event_impl (GdkEventFocus*) {
      try {
         attr_ = get_text ();
      }
      catch (std::invalid_argument& e) {
         XMessageBox::perform (_("Invalid value!"),
                               XMessageBox::ERROR | XMessageBox::OK);
         return true;
      }
      update ();
      return false; }

   T& attr_;
};

#endif
