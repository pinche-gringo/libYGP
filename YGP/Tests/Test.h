#ifndef TEST_H
#define TEST_H

//$Id: Test.h,v 1.5 2008/03/29 17:35:17 markus Rel $

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
#include <iomanip>

#define ERROROUT(x) { ++cErrors; std::cout << "    -> Failed (" << x << "; line " << __LINE__ << ")\n" << std::flush; }
#define TEST(x) { if (!(x)) { ERROROUT (#x) } }


#ifdef VERBOSE
#  define DEBUG 1
#  define PRINT(x) { std::cout << "Checking: " << x << std::flush; }
#  define check(x) { PRINT (#x "\n"); TEST(x) }
#else
#  define DEBUG 0
#  define check(x) TEST(x)
#  define PRINT(x)
#endif

#endif
