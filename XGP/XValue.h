#ifndef XGP_XVALUE_H
#define XGP_XVALUE_H

//$Id: XValue.h,v 1.6 2008/05/18 13:21:27 markus Rel $

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


#include <YGP/AYear.h>
#include <YGP/ADate.h>
#include <YGP/ATime.h>
#include <YGP/ATStamp.h>
#include <YGP/ANumeric.h>


#include <glibmm/value.h>


namespace Glib {

/**
 * Glib::Value basing on YGP::AttributValues
 */
class ValueBase_Attribute : public ValueBase {
 public:
   typedef const gchar* CType;                   ///< Description of data-type
   /// Returns a string describing the data type
   static GType value_type () {
      return G_TYPE_STRING; }

   /// Creates the parameter specification for the passed data-type
   GParamSpec* create_param_spec (const ustring& name) const {
      return g_param_spec_string
	 (name.c_str (), 0, 0, get_cstring (),
	  GParamFlags (G_PARAM_READABLE | G_PARAM_WRITABLE));
   }

   /// Sets from another AttributValue
   /// \param data Value to set
   void set (const YGP::AttributValue& data) {
      g_value_set_string (&gobject_, data.toString ().c_str ()); }

 protected:
   /// Sets the value as c-string
   /// \param data c-string to set
   /// \remarks never returns 0
   void        set_cstring (const char* data) {
      g_value_set_string (&gobject_, data); }

   /// Returns the value as c-string
   /// \return const char* Value as c-string
   /// \remarks never returns 0
   const char* get_cstring () const {
      const char *const data (g_value_get_string (&gobject_));
      return data ? data : ""; }
};


template <>
class Value<YGP::AYear> : public ValueBase_Attribute {
 public:
   typedef YGP::AttributValue CppType;

   YGP::AYear get () const { return YGP::AYear (get_cstring()); }
};

template <>
class Value<YGP::ADate> : public ValueBase_Attribute {
 public:
   typedef YGP::AttributValue CppType;

   YGP::ADate get () const { return YGP::ADate (get_cstring()); }
};

template <>
class Value<YGP::ATime> : public ValueBase_Attribute {
 public:
   typedef YGP::AttributValue CppType;

   YGP::ATime get () const { return YGP::ATime (get_cstring()); }
};

template <>
class Value<YGP::ATimestamp> : public ValueBase_Attribute {
 public:
   typedef YGP::AttributValue CppType;

   YGP::ATimestamp get () const { return YGP::ATimestamp (get_cstring()); }
   void set (const YGP::ATimestamp& data) {
      set_cstring (data.toString ().c_str ()); }
};

template <>
class Value<YGP::ANumeric> : public ValueBase_Attribute {
 public:
   typedef YGP::AttributValue CppType;

   YGP::ANumeric get () const { return YGP::ANumeric (get_cstring()); }
};

}

#endif
