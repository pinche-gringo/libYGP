#ifndef ENUMENTRY_H
#define ENUMENTRY_H

//$Id: EnumEntry.h,v 1.2 2008/03/30 13:39:17 markus Rel $

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


#include <gtkmm/comboboxtext.h>


namespace YGP {
   class MetaEnum;
};



namespace XGP {

/**Class enabling to select a value of a MetaEnum from a combobox offering all
   its values.
*/
class EnumEntry : public Gtk::ComboBoxText {
 public:
   EnumEntry (const YGP::MetaEnum& values);
   virtual ~EnumEntry ();

 private:
   // Protected manager functions
   EnumEntry ();
   EnumEntry (const EnumEntry& other);
   const EnumEntry& operator= (const EnumEntry& other);
};

}

#endif
