#ifndef TRACE_H
#define TRACE_H

//$Id: Trace.h,v 1.6 2002/05/24 06:52:42 markus Exp $

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


#ifdef TRACELEVEL
#include <iomanip.h>
#include <iostream.h>

#define TRACE(x)            cout << x << '\n' << flush;
#else
#define TRACE(x)

#endif

#if TRACELEVEL > 0
#define TRACE1(x)           TRACE(x)
#else
#define TRACE1(x)
#endif

#if TRACELEVEL > 1
#define TRACE2(x)           TRACE(x)
#else
#define TRACE2(x)
#endif

#if TRACELEVEL > 2
#define TRACE3(x)           TRACE(x)
#else
#define TRACE3(x)
#endif

#if TRACELEVEL > 3
#define TRACE4(x)           TRACE(x)
#else
#define TRACE4(x)
#endif

#if TRACELEVEL > 4
#define TRACE5(x)           TRACE(x)
#else
#define TRACE5(x)
#endif

#if TRACELEVEL > 5
#define TRACE6(x)           TRACE(x)
#else
#define TRACE6(x)
#endif

#if TRACELEVEL > 6
#define TRACE7(x)           TRACE(x)
#else
#define TRACE7(x)
#endif

#if TRACELEVEL > 7
#define TRACE8(x)           TRACE(x)
#else
#define TRACE8(x)
#endif

#if TRACELEVEL > 8
#define TRACE9(x)           TRACE(x)
#else
#define TRACE9(x)
#endif

#endif
