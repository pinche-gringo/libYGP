#ifndef DYNTRACE_H
#define DYNTRACE_H

//$Id: DynTrace.h,v 1.2 2006/07/17 00:55:33 markus Rel $

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


#include <map>
#include <string>
#include <iomanip>
#include <iostream>

#include <YGP/Check.h>


#ifdef TRACE_H
#  undef TRACE
#  undef TRACE0
#  undef TRACE1
#  undef TRACE2
#  undef TRACE3
#  undef TRACE4
#  undef TRACE5
#  undef TRACE6
#  undef TRACE7
#  undef TRACE8
#  undef TRACE9
#endif

#define _TRACE(obj, level, x)     if ((obj).getLevel () >= (level)) std::cout << x << '\n' << std::flush;
#define OTRACE0(obj, x)           _TRACE ((obj), 0, x)
#define OTRACE1(obj, x)           _TRACE ((obj), 1, x)
#define OTRACE2(obj, x)           _TRACE ((obj), 2, x)
#define OTRACE3(obj, x)           _TRACE ((obj), 3, x)
#define OTRACE4(obj, x)           _TRACE ((obj), 4, x)
#define OTRACE5(obj, x)           _TRACE ((obj), 5, x)
#define OTRACE6(obj, x)           _TRACE ((obj), 6, x)
#define OTRACE7(obj, x)           _TRACE ((obj), 7, x)
#define OTRACE8(obj, x)           _TRACE ((obj), 8, x)
#define OTRACE9(obj, x)           _TRACE ((obj), 9, x)
#define OTRACE(obj, x)            OTRACE0 ((obj), x)

#define TRACE0(x)                 _TRACE (_dt_, 0, x)
#define TRACE1(x)                 _TRACE (_dt_, 1, x)
#define TRACE2(x)                 _TRACE (_dt_, 2, x)
#define TRACE3(x)                 _TRACE (_dt_, 3, x)
#define TRACE4(x)                 _TRACE (_dt_, 4, x)
#define TRACE5(x)                 _TRACE (_dt_, 5, x)
#define TRACE6(x)                 _TRACE (_dt_, 6, x)
#define TRACE7(x)                 _TRACE (_dt_, 7, x)
#define TRACE8(x)                 _TRACE (_dt_, 8, x)
#define TRACE9(x)                 _TRACE (_dt_, 9, x)
#define TRACE(x)                  TRACE0(x)


namespace YGP {

/**Class for writing trace-messages, which can be dynamically switched
   on and off at runtime.

   To use this class, define a (static) object in a class/method/... which
   you want to trace dynamically. If you name this object _dt_, you can
   use one of the above TRACE-macros (thus replacing the TRACE-macros from
   YGP/Trace.h, or else use the OTRACE-macros accordingly.
 */
class DynTrace {
 public:
   DynTrace (const char* subsystem);
   ~DynTrace ();

   int getLevel () const { return level; }
   void setLevel (int newLevel) { level = newLevel; }

 private:
   // Prohibited manager methods
   DynTrace ();
   DynTrace (const DynTrace& other);

   const DynTrace& operator= (const DynTrace& other);

   int level;
};


/**Class to manage the created DynTraces classes.
 */
class DynTraceManager {
   friend class DynTrace;

 private:
   /**Functor for comparing pointers (which actually compares the values the
      pointers refers to).
   */
   struct lessDereferenced : public std::binary_function<const char*, const char*, bool> {
   /// Access to the functor
   /// \param x: First object to compare
   /// \param y: Second object to compare
   /// \returns bool: True, if the string the  object x points to is
   ///     smaller than the string the second objects points to.
   bool operator() (const char* x, const char* y) const {
      Check1 (x); Check1 (y);
      return strcmp (x, y) < 0; }
   };

 public:
   static const std::map<const char*, DynTrace*, lessDereferenced>& getObjects () { return traceObjs; }

 private:
   static void remove (const DynTrace* dynTrace);
   static void add (const char* name, DynTrace* traceObj) {
      Check1 (traceObjs.find (name) == traceObjs.end ());
      traceObjs[name] = traceObj; }

   DynTraceManager ();
   DynTraceManager (const DynTraceManager& other);
   const DynTraceManager& operator= (const DynTraceManager& other);
   ~DynTraceManager ();

   static std::map<const char*, DynTrace*, lessDereferenced> traceObjs;
};


}

#endif
