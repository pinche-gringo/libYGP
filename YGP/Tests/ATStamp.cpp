// $Id: ATStamp.cpp,v 1.9 2008/03/29 17:35:17 markus Rel $

// PROJECT     : libYGP
// SUBSYSTEM   : Test/ATStamp
// REFERENCES  :
// TODO        :
// BUGS        :
// REVISION    : $Revision: 1.9 $
// AUTHOR      : Markus Schwab
// CREATED     : 27.8.2001
// COPYRIGHT   : Copyright (C) 2001 - 2005, 2008

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

#include <clocale>
#include <iostream>
#include <stdexcept>

#include <YGP/ATStamp.h>

#include "Test.h"

int main(int argc, char* argv[]) {
    unsigned int cErrors(0);

    std::cout << "Testing ATimestamp...\n";
    YGP::ATimestamp now;
    check(!now.isDefined());
    YGP::ATimestamp early(false);
    check(early.isDefined());
    check(now < early);

    now = YGP::ATimestamp::now();
    check(now.isDefined());
    check(now > early);

    // The formatted value must be parsable again (in every available locale)
    for (const char* locale : {"C", "de_AT.UTF-8", "en_US.UTF-8", "en_GB.UTF-8", "fr_FR.UTF-8"}) {
        if (!setlocale(LC_ALL, locale))
            continue;
        try {
            const YGP::ATimestamp value(12, 11, 2005, 13, 14, 15);
            check(YGP::ATimestamp(value.toString()) == value);
        }
        catch (std::invalid_argument& e) {
            ERROROUT(locale << ": " << e.what());
        }
        try {
            const YGP::ATimestamp value(1, 2, 1999, 1, 2, 3);
            check(YGP::ATimestamp(value.toString()) == value);
        }
        catch (std::invalid_argument& e) {
            ERROROUT(locale << ": " << e.what());
        }
    }
    setlocale(LC_ALL, "C");

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
