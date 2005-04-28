#ifndef ENUMENTRY_H
#define ENUMENTRY_H

//$Id: EnumEntry.h,v 1.1 2005/04/28 22:41:32 markus Rel $

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
