// $Id: StackTrc.cpp,v 1.10 2003/09/11 04:17:20 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : StackTrace
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
//AUTHOR      : Markus Schwab
//CREATED     : 7.12.2000
//COPYRIGHT   : Anticopyright (A) 2000, 2001, 2002

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


#include <stdio.h>
#include <malloc.h>

#include <signal.h>

#include "Log.h"
#include "Check.h"
#include "StackTrc.h"

// Define for logging (printf for testing; LOGINFO for real)
#define PRINT LOGINFO
//#define PRINT puts


extern "C" {

/*--------------------------------------------------------------------------*/
//Purpose   : Signalhandler; dumps stack when signal is caught
//Parameters: sig: Number of signal caught
//Remarks   : A stacktrace might be useful (only?) after a segmentation fault
/*--------------------------------------------------------------------------*/
void handleSignal (int sig) {
   signal (SIGSEGV, SIG_DFL);       // Restore signal-handlers and reraise the
#ifdef HAVE_SIGBUS
   signal (SIGBUS, SIG_DFL);            // signal (to abort program correctly)
#endif

   dumpStack ();
   raise (sig);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Dumps the calling-sequence into the log-file
//Remarks   : The layout of the stack is like the following:
//              - argument(s) (if any)
//              - address to return to (address of caller)
//              - pointer to local vars of caller
//              - local variables (if any)
//
//            Because of that, the calling-sequence can be found by examining
//            the two value before the local variables; the first contains the
//            address of the caller and the second allows the follow the stack
//            down to the stack for the caller.
/*--------------------------------------------------------------------------*/
void dumpStack () {
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
      sprintf (pBuffer, "Called by ?? @ %p", pStack[1]);
      PRINT (pBuffer);

      if (!levels--)
         break;

      pStack = (int*)*pStack;
   }

   free (pBuffer);
}

} // end extern "C"
