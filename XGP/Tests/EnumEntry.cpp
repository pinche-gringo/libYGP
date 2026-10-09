// PROJECT     : libYGP
// SUBSYSTEM   : Test/EnumEntry
// AUTHOR      : Markus Schwab
// CREATED     : 9.10.2026
// COPYRIGHT   : Copyright (C) 2026

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

#include <iostream>

#include <gtk/gtk.h>
#include <gtkmm/init.h>

#include <YGP/MetaEnum.h>

#include <XGP/EnumEntry.h>

#include "Test.h"

class Numbers : public YGP::MetaEnum {
  public:
    Numbers() {
        insert(std::make_pair(1, "One"));
        insert(std::make_pair(2, "Two"));
        insert(std::make_pair(10, "Ten"));
    }
};

int main() {
    unsigned int cErrors(0);

    if (!gtk_init_check()) {
        std::cout << "Testing EnumEntry... skipped (no display)\n";
        return 77;
    }
    Gtk::init_gtkmm_internals();

    std::cout << "Testing EnumEntry...\n";
    Numbers values;
    XGP::EnumEntry entry(values);

    // The values are listed in the order of the MetaEnum
    check(entry.get_active_row_number() == -1);
    entry.set_active(0);
    check(entry.get_active_text() == "One");
    entry.set_active(1);
    check(entry.get_active_text() == "Two");
    entry.set_active(2);
    check(entry.get_active_text() == "Ten");
    entry.set_active(3);
    check(entry.get_active_row_number() == -1);

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
