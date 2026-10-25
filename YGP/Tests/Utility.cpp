// PROJECT     : libYGP
// SUBSYSTEM   : Test/Utility
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 25.10.2026
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

#include <YGP/Utility.h>

#include "Test.h"

int main() {
    unsigned int cErrors(0);

    std::cout << "Testing Utility ...\n";

    // Every special character is converted, also directly following each other
    std::string html("<<b>&<i>\"x\"''");
    YGP::convertUTF82HTML(html);
    check(html == "&lt;&lt;b&gt;&amp;&lt;i&gt;&quot;x&quot;&lsquo;&lsquo;");

    html = "plain text";
    YGP::convertUTF82HTML(html);
    check(html == "plain text");

    html = "&lt;&lt;&amp;&#65;&#x42;";
    YGP::convertHTML2UTF8(html);
    check(html == "<<&AB");

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
