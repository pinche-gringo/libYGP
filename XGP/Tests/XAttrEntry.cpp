// PROJECT     : libYGP
// SUBSYSTEM   : Test/XAttrEntry
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
#include <string>

#include <gtk/gtk.h>
#include <gtkmm/init.h>

#include <YGP/ANumeric.h>

#include <XGP/XAttrEntry.h>

#include "Test.h"

int main() {
    unsigned int cErrors(0);

    if (!gtk_init_check()) {
        std::cout << "Testing XAttrEntry... skipped (no display)\n";
        return 77;
    }
    Gtk::init_gtkmm_internals();

    std::cout << "Testing XAttrEntry...\n";
    std::string str("Hello");
    XGP::XAttributeEntry<std::string> entStr(str);
    check(entStr.get_text() == "Hello");
    check(!entStr.hasChanged());
    entStr.setText("World");
    check(entStr.get_text() == "World");
    check(entStr.hasChanged());
    check(str == "Hello");
    entStr.commit();
    check(str == "World");
    check(!entStr.hasChanged());

    // update () discards the pending input
    entStr.setText("Discarded");
    entStr.update();
    check(entStr.get_text() == "World");
    check(!entStr.hasChanged());
    check(&entStr.getAttribute() == &str);

    Glib::ustring ustr("äöü");
    XGP::XAttributeEntry<Glib::ustring> entUStr(ustr);
    check(entUStr.get_text() == "äöü");
    entUStr.setText("€");
    check(entUStr.hasChanged());
    entUStr.commit();
    check(ustr == "€");

    YGP::ANumeric num(1234);
    XGP::XAttributeEntry<YGP::ANumeric> entNum(num);
    check(entNum.get_text() == num.toString());
    check(!entNum.hasChanged());
    entNum.setText("4711");
    check(entNum.hasChanged());
    check(num == YGP::ANumeric(1234));
    entNum.commit();
    check(num == YGP::ANumeric(4711));
    check(!entNum.hasChanged());

    num = 99;
    check(entNum.hasChanged());
    entNum.update();
    check(!entNum.hasChanged());
    check(entNum.get_text() == num.toString());

    try {
        entNum.setText("no number");
        check(!"No exception");
    }
    catch (std::invalid_argument&) {
    }

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
