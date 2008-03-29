#ifndef TRACE_H
#define TRACE_H

//$Id: Trace.h,v 1.13 2008/03/29 17:35:17 markus Rel $

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


// Semantic of that traces: TRACE is defined, when TRACELEVEL is defined;
// TRACE0 - TRACE9 only if TRACELVEL is set to a value corresponding to TRACEx

#ifdef TRACELEVEL
#include <iomanip>
#include <iostream>

// Flush every trace imediately
#define TRACE(x)            std::cout << x << std::endl;
#endif

#if defined (TRACELEVEL) && (TRACELEVEL >= 0)
#define TRACE0(x)           TRACE(x)
#else
#define TRACE0(x)
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
