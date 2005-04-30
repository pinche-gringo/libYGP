#ifndef XATTRENTRY_H
#define XATTRENTRY_H

//$Id: XAttrEntry.h,v 1.18 2005/04/30 02:19:06 markus Rel $

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


#include <ygp-cfg.h>

#if !defined (HAVE_GETTEXT) || !defined (ENABLE_NLS)
#  define dgettext(pkg, text) (text)
#endif

#include <cstdio>
#include <stdexcept>

#include <gtkmm/main.h>
#include <gtkmm/entry.h>
#include <gtkmm/messagedialog.h>


namespace XGP {

/**Class to display and change an attribute inside and entry box. The value of
   the attribute is displayed formatted (localized) when not edited.

   An "attribute" is any of the standard C++ data types and the types direved
   from href="../YGP/classAttributValue.html">AttributValue</a> of the YGP
   library (such as ADate, ANumeric, ...).

   If the curser enters the entry field, the value is displayed unformatted.
   When leaving the field, the new value is checked for vality (causing a
   message box with an error message if the user entered an invalid value).

   The commit() method transfers the input to the attribute.
*/
template <typename T, typename P = Gtk::Entry> class XAttributeEntry : public P {
   typedef P parent;

 public:
   /// Constructor; sets the attribute to handle.
   XAttributeEntry (T& attr)
      : temp (attr), attr_ (attr), inError (false) {
      P::set_text (attr_.toString ());
   }
   /// Destructor
   ~XAttributeEntry () { }

   /// Returns if the field has been changed
   bool hasChanged () const { return temp != attr_; }

   /// Actualizes the value of the attribute with the value entered in the
   /// entry field.
   void commit () { attr_ = temp; }
   /// Actualizes the displayed value with the (changed) value of the
   /// attribute.
   void update () {
      temp = attr_;
      P::set_text (P::has_focus () ? temp.toUnformattedString () : temp.toString ()); }
   /// Actualizes the displayed value with the passed value. The value of the
   /// attribute is not (yet) changed (this happens at commit ()).
   void setText (const Glib::ustring& value) {
      temp = value;
      P::set_text (P::has_focus () ? temp.toString () : temp.toUnformattedString ()); }

   /// Returns the handled attribute
   T& getAttribute () { return attr_; }

 protected:
   virtual bool on_focus_in_event (GdkEventFocus* ev) {
      if (inError)
         inError = false;
      else
         P::set_text (temp.toUnformattedString ());
      return P::on_focus_in_event (ev); }
   virtual bool on_focus_out_event (GdkEventFocus* ev) {
      P::on_focus_out_event (ev);
      try {
         temp = P::get_text ();
         P::set_text (temp.toString ());
      }
      catch (std::invalid_argument& e) {
         inError = true;
         Gtk::MessageDialog msg (e.what (), Gtk::MESSAGE_ERROR);
         msg.set_title (Glib::locale_to_utf8 (dgettext (LIBYGP_NAME, "Invalid value!")));
         msg.run ();
         Glib::signal_idle ().connect (mem_fun (*this, &XAttributeEntry::takeFocus));
         return true;
      }
      return false; }

   bool takeFocus () {
      P::grab_focus ();
      return 0; }

 private:
   XAttributeEntry (const XAttributeEntry&);
   const XAttributeEntry& operator= (const XAttributeEntry&);

   T  temp;
   T& attr_;
   bool inError;
};


// Specialication for strings
template <> inline XAttributeEntry<std::string>::XAttributeEntry (std::string& attr) : temp (attr)
     , attr_ (attr), inError (false) { parent::set_text (attr); }

template <> inline void XAttributeEntry<std::string>::update () { parent::set_text (temp = attr_); }
template <> inline void XAttributeEntry<std::string>::setText (const Glib::ustring& value) {
   temp = Glib::locale_from_utf8 (value);
   parent::set_text (value); }
template <> inline bool XAttributeEntry<std::string>::on_focus_in_event (GdkEventFocus* ev) {
   return parent::on_focus_in_event (ev); }
template <> inline bool XAttributeEntry<std::string>::on_focus_out_event (GdkEventFocus* ev) {
   temp = parent::get_text ();
   return parent::on_focus_out_event (ev); }

// Specialication for Glib::ustrings
template <> inline XAttributeEntry<Glib::ustring>::XAttributeEntry (Glib::ustring& attr) : temp (attr)
     , attr_ (attr), inError (false) { parent::set_text (attr); }

template <> inline void XAttributeEntry<Glib::ustring>::update () { parent::set_text (temp = attr_); }
template <> inline void XAttributeEntry<Glib::ustring>::setText (const Glib::ustring& value) { parent::set_text (temp = value); }
 template <> inline bool XAttributeEntry<Glib::ustring>::on_focus_in_event (GdkEventFocus* ev) {
   return parent::on_focus_in_event (ev); }
template <> inline bool XAttributeEntry<Glib::ustring>::on_focus_out_event (GdkEventFocus* ev) {
   temp = parent::get_text ();
   return parent::on_focus_out_event (ev); }

}


#endif
