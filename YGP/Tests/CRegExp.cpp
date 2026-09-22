// $Id: CRegExp.cpp,v 1.23 2008/03/29 17:35:17 markus Rel $

// PROJECT     : libYGP
// SUBSYSTEM   : Test/CRegExp
// REFERENCES  :
// TODO        :
// BUGS        :
// REVISION    : $Revision: 1.23 $
// AUTHOR      : Markus Schwab
// CREATED     : 27.8.2001
// COPYRIGHT   : Copyright (C) 2001 - 2005, 2008, 2009

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

#include <ygp-cfg.h>

#include <iostream>

#include "Test.h"

#include <YGP/ANumeric.h>
#include <YGP/CRegExp.h>
#include <YGP/Parse.h>
#include <YGP/Trace.h>
#include <YGP/XStream.h>

unsigned int cErrors(0);
std::string strRE, strVal;
YGP::RegularExpression regexp("a");
bool match;

int foundRegExp(const char* pRegExp, unsigned int) {
    check(pRegExp);
    TRACE1("Found regular expression: " << pRegExp);
    strRE = pRegExp;
    regexp = strRE.c_str();
    return YGP::ParseObject::PARSE_OK;
}

int foundValue(const char* pValue, unsigned int) {
    check(pValue);
    check(strRE.size());
    TRACE1("Found value: " << pValue);
    strVal = pValue;
    match = regexp.matches(pValue);
    return YGP::ParseObject::PARSE_OK;
}

int foundResult(const char* pResult, unsigned int) {
    check(pResult);
    PRINT(strRE << " matches " << strVal << " == " << pResult << '\n');
    if ((*pResult != '0') != match)
        ERROROUT("RegExp (\"" << strRE << "\").matches (\"" << strVal << "\") == " << pResult);

    return YGP::ParseObject::PARSE_OK;
}

#if SYSTEM == WINDOWS
static const char* TESTFILE = "..\\YGP\\Tests\\CRegExp.test";
#else
static const char* TESTFILE = "CRegExp.test";
#endif

int main(int argc, char* argv[]) {
    std::cout << "Testing RegularExpression...\n";

    try {
        if (argc > 1) { // If a parameter is passed, treat it as regexp to check
            regexp = argv[1];

            const char* pMatch = ((argc > 2) ? argv[2] : "");
            int result((argc > 3) ? (*argv[3] - '0') : 1);
            PRINT(argv[1] << " matches " << pMatch << " == " << result << '\n');
            int rc(regexp.matches(pMatch));
            if (rc != result)
                ERROROUT("RegExp (\"" << argv[1] << "\").matches (\"" << pMatch << "\") == " << result);
            return !rc;
        }
    }
    catch (std::invalid_argument& e) {
        std::cerr << e.what() << '\n';
    }

    YGP::Xifstream frexexp;
    try {
        frexexp.open(TESTFILE, std::ios::in);
        check(frexexp);
        if (frexexp) {
            frexexp.init();

        } // endif
    } // end-try
    catch (std::string& e) {
        std::cerr << "Error parsing '" << TESTFILE << ": " << e.c_str()
                  << "\nActual position: " << YGP::ANumeric(frexexp.getLine()).toString() << '/'
                  << YGP::ANumeric(frexexp.getColumn()).toString() << '\n';
    }

    if (cErrors) {
#ifdef HAVE_REGEX_H
        std::cout << "    -> Information: Your system's implementation of regular expressions differs\n"
                  << "       from the glibc-implementation these test base upon. Either ignore the\n"
                  << "       error(s) or compile without having HAVE_REGEX_H defined\n";
        cErrors = 0;
#else
        std::cout << "Failures: " << cErrors << '\n';
#endif
    }
    return cErrors ? 1 : 0;
}
