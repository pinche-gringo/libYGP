#ifndef TEST_H
#define TEST_H

//$Id: Test.h,v 1.1 2001/08/27 15:24:50 markus Exp $

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


#define ERROROUT(x) { ++cErrors; cout << "    -> Failed (" << x << "; line " << __LINE__ << ")\n"; }
#define CHECK(x) { if (!(x)) { ERROROUT (#x) } }


#ifdef VERBOSE
#  define DEBUG 1
#  define check(x) { cout << "Checking: " #x "\n"; CHECK(x) }
#else
#  define DEBUG 0
#  define check(x) CHECK(x)
#endif

#endif
