// PROJECT     : libYGP
// SUBSYSTEM   : Test/XAttribute
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

#include <XGP/XAttribute.h>

#include "Test.h"

int main() {
    unsigned int cErrors(0);

    std::cout << "Testing XAttribute...\n";
    Glib::ustring value("Initial");
    YGP::Attribute<Glib::ustring> attr("Name", value);
    check(attr.getValue() == "Initial");
    check(attr.getFormattedValue() == "Initial");

    check(attr.assignFromString("Changed"));
    check(value == "Changed");
    check(attr.getValue() == "Changed");

    check(attr.assign("äöü €", 0));
    check(value == "äöü €");
    check(value.length() == 5);

    check(attr.assignFromString(""));
    check(value.empty());

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
