// $Id: Thread.cpp,v 1.8 2006/06/03 21:32:35 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/Thread
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.8 $
//AUTHOR      : Markus Schwab
//CREATED     : 19.10.2003
//COPYRIGHT   : Copyright (C) 2003 - 2005

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


#include <unistd.h>

#include <ygp-cfg.h>

#if defined HAVE__SLEEP
#  define sleep      _sleep
#endif


#include <YGP/Mutex.h>
#include <YGP/Thread.h>

#include "Test.h"

unsigned int count (0);
YGP::Mutex waitThread;
YGP::Mutex waitParent;

const unsigned int COUNT (1000);

void* sum (void*) {
   waitParent.lock ();
   waitThread.lock ();
   for (unsigned int i (0); i < COUNT; ++i)
       ++count;
   waitParent.unlock ();
   return NULL;
}

int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   try {
      YGP::Thread* thread (YGP::Thread::create (&sum, NULL));
      check (thread);
      while (waitThread.trylock ()) {
	 waitThread.unlock ();
	 sleep (0);
      }
      waitParent.lock ();
      check (count == COUNT);
   }
   catch (YGP::ExecError& e) {
      std::cerr << e.what () << '\n';
      check (0);
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
