#ifndef TRACE_H
#define TRACE_H

//$Id: Trace.h,v 1.5 2002/04/19 07:08:53 markus Rel $

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


#ifdef DEBUG
#include <iomanip.h>
#include <iostream.h>

#define TRACE(x)            cout << x << '\n' << flush;
#else
#define TRACE(x)

#endif

#if DEBUG > 0
#define TRACE1(x)           TRACE(x)
#else
#define TRACE1(x)
#endif

#if DEBUG > 1
#define TRACE2(x)           TRACE(x)
#else
#define TRACE2(x)
#endif

#if DEBUG > 2
#define TRACE3(x)           TRACE(x)
#else
#define TRACE3(x)
#endif

#if DEBUG > 3
#define TRACE4(x)           TRACE(x)
#else
#define TRACE4(x)
#endif

#if DEBUG > 4
#define TRACE5(x)           TRACE(x)
#else
#define TRACE5(x)
#endif

#if DEBUG > 5
#define TRACE6(x)           TRACE(x)
#else
#define TRACE6(x)
#endif

#if DEBUG > 6
#define TRACE7(x)           TRACE(x)
#else
#define TRACE7(x)
#endif

#if DEBUG > 7
#define TRACE8(x)           TRACE(x)
#else
#define TRACE8(x)
#endif

#if DEBUG > 8
#define TRACE9(x)           TRACE(x)
#else
#define TRACE9(x)
#endif

#endif
