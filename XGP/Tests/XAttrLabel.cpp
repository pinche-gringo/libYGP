// PROJECT     : libYGP
// SUBSYSTEM   : Test/XAttrLabel
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

#include <YGP/ADate.h>
#include <YGP/ANumeric.h>

#include <XGP/XAttrLabel.h>

#include "Test.h"

int main() {
    unsigned int cErrors(0);

    if (!gtk_init_check()) {
        std::cout << "Testing XAttrLabel... skipped (no display)\n";
        return 77;
    }
    Gtk::init_gtkmm_internals();

    std::cout << "Testing XAttrLabel...\n";
    int i(-42);
    XGP::XAttributeLabel<int> lblInt(i);
    check(lblInt.get_text() == "-42");
    i = 17;
    check(lblInt.get_text() == "-42");
    lblInt.update();
    check(lblInt.get_text() == "17");
    check(&lblInt.getAttribute() == &i);

    unsigned long ul(4000000000UL);
    XGP::XAttributeLabel<unsigned long> lblULong(ul, 1.0, 0.5);
    check(lblULong.get_text() == "4000000000");
    check(lblULong.get_xalign() == 1.0f);
    check(lblULong.get_yalign() == 0.5f);

    double dbl(1.5);
    XGP::XAttributeLabel<double> lblDouble(dbl);
    check(lblDouble.get_text() == "1.500000");

    YGP::ANumeric num(4711);
    XGP::XAttributeLabel<YGP::ANumeric> lblNum(num);
    check(lblNum.get_text() == num.toString());

    YGP::ADate date(12, 11, 2005);
    XGP::XAttributeLabel<YGP::ADate> lblDate(date);
    check(lblDate.get_text() == date.toString());

    // XAttributeLabel2 stores a copy of the attribute
    short s(5);
    XGP::XAttributeLabel2<short> lbl2(s);
    check(lbl2.get_text() == "5");
    s = 6;
    lbl2.update();
    check(lbl2.get_text() == "5");
    check(&lbl2.getAttribute() != &s);

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
