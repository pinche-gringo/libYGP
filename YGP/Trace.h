#ifndef YGP_TRACE_H
#define YGP_TRACE_H

//$Id: Trace.h,v 1.14 2008/05/18 13:21:27 markus Rel $

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


// Semantic of the traces:
// There are 3 different types of traces: Only text, with a leading
// time (including milliseconds) or with a leading timestamp (date and
// time; including milliseconds).
//
// The format of the trace is specified on a per-module basis by defining either
// TRACELEVEL (only text), TRACELEVEL_TIME (with time) or TRACELEVEL_TS
// respectively (with a timestamp).

// TRACE is defined, when TRACELEVEL* is defined;
// TRACE0 - TRACE9 only if TRACELVEL* is set to a value corresponding to TRACEx


#ifdef TRACELEVEL_TS
#  undef TRACELEVEL
#  define TRACELEVEL TRACELEVEL_TS

#  include <sys/time.h>
#  include <ctime>
#  include <iomanip>
#  include <iostream>

static inline char* printTime () {
   struct timeval actTime;
   gettimeofday (&actTime, NULL);
   static char time[24];
   strftime (time, sizeof (time), "%x %X.", localtime (&actTime.tv_sec));
   std::cout << time << std::setfill ('0') << std::setw (6) << actTime.tv_usec << ": ";
}

#elif defined (TRACELEVEL_TIME)
#  undef TRACELEVEL
#  define TRACELEVEL TRACELEVEL_TIME

#  include <sys/time.h>
#  include <ctime>
#  include <iostream>
#  include <iomanip>

static inline char* printTime () {
   struct timeval actTime;
   gettimeofday (&actTime, NULL);
   static char time[12];
   strftime (time, sizeof (time), "%X.", localtime (&actTime.tv_sec));
   std::cout << time << std::setfill ('0') << std::setw (6) << actTime.tv_usec << ": ";
}

#else
static inline char* printTime () {
}
#endif

#ifdef TRACELEVEL
#include <iomanip>
#include <iostream>

// Flush every trace imediately
#define TRACE(x)            printTime (), std::cout << x << std::endl;
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
