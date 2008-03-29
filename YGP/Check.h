#ifndef CHECK_H
#define CHECK_H

//$Id: Check.h,v 1.7 2008/03/29 17:35:17 markus Rel $

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


#undef Check
#undef Check1
#undef Check2
#undef Check3
#undef Check4
#undef Check5
#undef Check6
#undef Check7
#undef Check8
#undef Check9


// CHECK undefined means no checks at all
#if !defined CHECK || defined (NDEBUG)
// In counterpart to the original assert don't define Check to 0; it's bad style
// to change the program behaviour in debug and release versions.
#define Check(expr)
#define CheckMsg(expr, msg)
#else

#ifdef __cplusplus
extern "C" {
#endif
   int check (const char* expr, const char* file, unsigned int line);
#ifdef __cplusplus
}
#endif

#define Check(expr)        ((expr) ? 0 : check (__STRING(expr), __FILE__, __LINE__))
#define CheckMsg(expr,msg) ((expr) ? 0 : check (msg, __FILE__, __LINE__))
#endif

#if CHECK > 0
#define Check1(expr)  Check(expr)
#else
#define Check1(expr)
#endif

#if CHECK > 1
#define Check2(expr)  Check(expr)
#else
#define Check2(expr)
#endif

#if CHECK > 2
#define Check3(expr)  Check(expr)
#else
#define Check3(expr)
#endif

#if CHECK > 3
#define Check4(expr)  Check(expr)
#else
#define Check4(expr)
#endif

#if CHECK > 4
#define Check5(expr)  Check(expr)
#else
#define Check5(expr)
#endif

#if CHECK > 5
#define Check6(expr)  Check(expr)
#else
#define Check6(expr)
#endif

#if CHECK > 6
#define Check7(expr)  Check(expr)
#else
#define Check7(expr)
#endif

#if CHECK > 7
#define Check8(expr)  Check(expr)
#else
#define Check8(expr)
#endif

#if CHECK > 8
#define Check9(expr)  Check(expr)
#else
#define Check9(expr)
#endif

#endif
