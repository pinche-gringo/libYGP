#ifndef TEST_H
#define TEST_H

//$Id: Test.h,v 1.3 2002/05/24 07:01:16 markus Exp $

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <iostream>
#include <iomanip>

#define ERROROUT(x) { ++cErrors; cout << "    -> Failed (" << x << "; line " << __LINE__ << ")\n" << flush; }
#define TEST(x) { if (!(x)) { ERROROUT (#x) } }


#ifdef VERBOSE
#  define DEBUG 1
#  define PRINT(x) { cout << "Checking: " << x << flush; }
#  define check(x) { PRINT (#x "\n"); TEST(x) }
#else
#  define DEBUG 0
#  define check(x) TEST(x)
#  define PRINT(x) 
#endif

#endif
