// $Id: StackTrc.cpp,v 1.16 2005/01/09 02:10:25 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : StackTrace
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.16 $
//AUTHOR      : Markus Schwab
//CREATED     : 7.12.2000
//COPYRIGHT   : Copyright (C) 2000 - 2005

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

#ifdef _MSC_VER
#pragma warning(disable:4786)   // disable warning about truncating debug info
#endif


#include <ygp-cfg.h>

#include <cstdio>
#include <malloc.h>

#include <signal.h>

#ifdef HAVE_BACKTRACE
#  include <execinfo.h>
#endif

#include "YGP/Log.h"
#include "YGP/Check.h"
#include "YGP/StackTrc.h"

// Define for logging (printf for testing; YGP::LOGINFO for real)
#define PRINT LOGINFO
//#define PRINT puts


extern "C" {

//-----------------------------------------------------------------------------
/// Signalhandler; dumps stack when signal is caught
/// \param sig: Number of signal caught
/// \remarks A stacktrace might be useful (only?) after a segmentation fault
//-----------------------------------------------------------------------------
void handleSignal (int sig) {
   signal (SIGSEGV, SIG_DFL);       // Restore signal-handlers and reraise the
#ifdef HAVE_SIGBUS
   signal (SIGBUS, SIG_DFL);            // signal (to abort program correctly)
#endif

   dumpStack ();
   raise (sig);
}

//-----------------------------------------------------------------------------
/// Dumps the calling-sequence into the log-file
/// \remarks The layout of the stack is like the following: - argument(s) (if
///     any) - address to return to (address of caller) - pointer to local
///     vars of caller - local variables (if any) Because of that, the
///     calling-sequence can be found by examining the two value before the
///     local variables; the first contains the address of the caller and the
///     second allows the follow the stack down to the stack for the caller.
//-----------------------------------------------------------------------------
void dumpStack () {
#ifdef HAVE_BACKTRACE
   void* traces[15];
   size_t size (backtrace (traces, sizeof (traces)));
   char** lines (backtrace_symbols (traces, size));

   for (unsigned int i (0); i < size; ++i)
      PRINT (lines[i]);

   free (lines);
#else
   char* pBuffer = (char*)malloc (64);

   sprintf (pBuffer, "dumpStack @ %p", dumpStack);
   PRINT (pBuffer);

   // Let pStack point to base-pointer
   int* pStack = ((int*)(&pBuffer)) + 1;

   int levels (15);

   while (*pStack) {
      // Check if the stack is aligned
      Check3 ((unsigned int)pStack
              == ((unsigned int)pStack & ~(sizeof (int) - 1)));

      // The address of the caller is next (behind) the basepointer
      sprintf (pBuffer, "Called by ?? @ %p", (void*)pStack[1]);
      PRINT (pBuffer);

      if (!levels--)
         break;

      pStack = (int*)*pStack;
   }

   free (pBuffer);
#endif
}

} // end extern "C"
