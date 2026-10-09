// PROJECT     : libYGP
// SUBSYSTEM   : Test/XAttrSpin
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

#include <gtkmm/adjustment.h>

#include <XGP/XAttrSpin.h>

#include "Test.h"

int main() {
    unsigned int cErrors(0);

    if (!gtk_init_check()) {
        std::cout << "Testing XAttrSpin... skipped (no display)\n";
        return 77;
    }
    Gtk::init_gtkmm_internals();

    std::cout << "Testing XAttrSpin...\n";
    int value(10);
    XGP::XAttributeSpinEntry<int> spin(value, Gtk::Adjustment::create(0, 0, 100));
    check(&spin.getAttribute() == &value);

    spin.update();
    check(spin.get_text() == YGP::ANumeric::toString(10));
    check(!spin.hasChanged());

    spin.setValue(42);
    check(spin.get_text() == YGP::ANumeric::toString(42));
    check(spin.hasChanged());
    check(value == 10);
    spin.commit();
    check(value == 42);
    check(!spin.hasChanged());

    spin.set_text("no number");
    check(spin.hasChanged());

    YGP::ANumeric num(17);
    XGP::XAttributeSpinEntry<YGP::ANumeric> spinNum(num, Gtk::Adjustment::create(0, 0, 100));
    spinNum.update();
    check(spinNum.get_text() == num.toString());
    check(!spinNum.hasChanged());
    spinNum.setValue(YGP::ANumeric(23));
    check(spinNum.hasChanged());
    spinNum.commit();
    check(num == YGP::ANumeric(23));

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
