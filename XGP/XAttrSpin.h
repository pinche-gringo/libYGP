#ifndef XGP_XATTSPIN_H
#define XGP_XATTRSPIN_H

//$Id: XAttrSpin.h,v 1.5 2008/05/18 13:21:27 markus Rel $

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


#include <ygp-cfg.h>

#if defined (HAVE_GETTEXT) && defined (ENABLE_NLS)
#  include <libintl.h>
#else
#  define dgettext(pkg, text) (text)
#endif

#include <cstdio>
#include <stdexcept>

#include <glibmm/main.h>
#include <glibmm/convert.h>

#include <gtkmm/spinbutton.h>
#include <gtkmm/messagedialog.h>

#include <YGP/ANumeric.h>


namespace XGP {

/**Class to display and change an attribute inside and spin-entry. The value of
   the attribute is displayed formatted (localized) when not edited.

   An "attribute" is any of the numeric C++ data types and
   <a href="../YGP/classANumeric.html">ANumeric</a> of the YGP library.

   If the curser enters the entry field, the value is displayed unformatted.
   When leaving the field, the new value is checked for vality (causing a
   message box with an error message if the user entered an invalid value).

   The commit() method transfers the input to the attribute.
*/
template <typename T, typename P = Gtk::SpinButton> class XAttributeSpinEntry : public P {
   typedef P parent;

 public:
   /// Constructor; sets the attribute to handle.
   XAttributeSpinEntry (T& attr, const Glib::RefPtr<Gtk::Adjustment>& adjustment,
			double climbRate = 0.0, guint digits = 0)
   : P (adjustment, climbRate, digits), attr_ (attr), inError (false) {
      P::set_numeric (false);
      P::signal_realize ().connect
	 (mem_fun (*this, &XGP::XAttributeSpinEntry<T, P>::update));
   }
   XAttributeSpinEntry (T& attr, double climbRate = 0.0, guint digits = 0)
   : P (climbRate, digits), attr_ (attr), inError (false) {
      P::set_numeric (false);
      P::signal_realize ().connect
	 (mem_fun (*this, &XGP::XAttributeSpinEntry<T, P>::update));
   }

   /// Destructor
   ~XAttributeSpinEntry () { }

   /// Returns if the field has been changed
   bool hasChanged () const {
      try {
	 YGP::ANumeric value (P::get_text ());
	 return value != attr_;
      }
      catch (std::invalid_argument&) {
	 return true;
      }
   }

   /// Actualizes the value of the attribute with the value entered in the
   /// entry field.
   void commit () { attr_ = (T)YGP::ANumeric (P::get_text ()); }
   /// Actualizes the displayed value with the (changed) value of the
   /// attribute.
   void update () { setValue (attr_); }
   /// Actualizes the displayed value with the passed value. The value of the
   /// attribute is not (yet) changed (this happens at commit ()).
   void setValue (T value) {
      if (P::has_focus ())
         P::set_value (value);
      else
	 P::set_text (YGP::ANumeric::toString (value));
   }

   /// Returns the handled attribute
   T& getAttribute () { return attr_; }

 protected:
   virtual bool on_focus_in_event (GdkEventFocus* ev) {
      if (inError)
         inError = false;
      else {
	 P::set_numeric ();
         P::set_value ((int)YGP::ANumeric (P::get_text ()));
      }
      return P::on_focus_in_event (ev); }
   virtual bool on_focus_out_event (GdkEventFocus* ev) {
      P::on_focus_out_event (ev);
      try {
         YGP::ANumeric temp (P::get_text ());
	 P::set_numeric (false);
         P::set_text (temp.toString ());
      }
      catch (std::invalid_argument& e) {
         inError = true;
         Gtk::MessageDialog msg (e.what (), Gtk::MESSAGE_ERROR);
         msg.set_title (Glib::locale_to_utf8 (dgettext (LIBYGP_NAME, "Invalid value!")));
         msg.run ();
         Glib::signal_idle ().connect (mem_fun (*this, &XAttributeSpinEntry::takeFocus));
         return true;
      }
      return false; }

   bool takeFocus () {
      P::grab_focus ();
      return 0; }

 private:
   XAttributeSpinEntry (const XAttributeSpinEntry&);
   const XAttributeSpinEntry& operator= (const XAttributeSpinEntry&);

   T& attr_;
   bool inError;
};


// Specialication for YGP::ANumeric
template <> inline
XAttributeSpinEntry<YGP::ANumeric>::XAttributeSpinEntry (YGP::ANumeric& attr, const Glib::RefPtr<Gtk::Adjustment>& adjustment,
			double climbRate, guint digits)
   : parent (adjustment, climbRate, digits), attr_ (attr), inError (false) {
      parent::set_numeric (false);
      parent::signal_realize ().connect
	 (mem_fun (*this, &XGP::XAttributeSpinEntry<YGP::ANumeric, parent>::update));
   }
template <> inline
XAttributeSpinEntry<YGP::ANumeric>::XAttributeSpinEntry (YGP::ANumeric& attr, double climbRate, guint digits)
   : parent (climbRate, digits), attr_ (attr), inError (false) {
      parent::set_numeric (false);
      parent::signal_realize ().connect
	 (mem_fun (*this, &XGP::XAttributeSpinEntry<YGP::ANumeric, parent>::update));
   }

/// Actualizes the value of the attribute with the value entered in the
/// entry field.
template <> inline
void XAttributeSpinEntry<YGP::ANumeric>::commit () { attr_ = parent::get_text (); }

   /// Actualizes the displayed value with the passed value. The value of the
   /// attribute is not (yet) changed (this happens at commit ()).
template <> inline
void XAttributeSpinEntry<YGP::ANumeric>::setValue (YGP::ANumeric value) {
   if (parent::has_focus ())
      parent::set_value ((int)value);
   else
      parent::set_text (value.toString ());
   }
}


#endif
