// PROJECT     : libYGP
// SUBSYSTEM   : Test/XValue
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
#include <stdexcept>

#include <glibmm/init.h>

#include <XGP/XValue.h>

#include "Test.h"

int main() {
    unsigned int cErrors(0);

    std::cout << "Testing XValue...\n";
    Glib::init();
    check(Glib::Value<YGP::ADate>::value_type() == G_TYPE_STRING);
    check(Glib::Value<YGP::ANumeric>::value_type() == G_TYPE_STRING);

    // An uninitialised string yields an undefined attribute
    Glib::Value<YGP::ADate> date;
    date.init(Glib::Value<YGP::ADate>::value_type());
    check(!date.get().isDefined());

    YGP::ADate d(12, 11, 2005);
    date.set(d);
    try {
        check(date.get() == d);
        check(date.get().getDay() == 12);
        check(date.get().getMonth() == 11);
        check(date.get().getYear() == 2005);
    }
    catch (std::invalid_argument& e) {
        ERROROUT("ADate round-trip: " << e.what());
    }

    Glib::Value<YGP::AYear> year;
    year.init(Glib::Value<YGP::AYear>::value_type());
    year.set(YGP::AYear(1999));
    check(year.get() == YGP::AYear(1999));

    Glib::Value<YGP::ATime> time;
    time.init(Glib::Value<YGP::ATime>::value_type());
    YGP::ATime t(13, 14, 15);
    time.set(t);
    try {
        check(time.get() == t);
    }
    catch (std::invalid_argument& e) {
        ERROROUT("ATime round-trip: " << e.what());
    }

    Glib::Value<YGP::ATimestamp> stamp;
    stamp.init(Glib::Value<YGP::ATimestamp>::value_type());
    YGP::ATimestamp ts(12, 11, 2005, 13, 14, 15);
    stamp.set(ts);
    try {
        check(stamp.get() == ts);
    }
    catch (std::invalid_argument& e) {
        ERROROUT("ATimestamp round-trip: " << e.what());
    }

    Glib::Value<YGP::ANumeric> num;
    num.init(Glib::Value<YGP::ANumeric>::value_type());
    check(!num.get().isDefined());
    num.set(YGP::ANumeric(4711));
    try {
        check(num.get() == YGP::ANumeric(4711));
    }
    catch (std::invalid_argument& e) {
        ERROROUT("ANumeric round-trip: " << e.what());
    }

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
