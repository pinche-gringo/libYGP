// $Id: Thread.cpp,v 1.1 2003/10/19 15:36:36 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/Thread
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 19.10.2003
//COPYRIGHT   : Anticopyright (A) 2001 - 2003

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


#include <Internal.h>

#include <unistd.h>

#include <Mutex.h>
#include <Thread.h>

#include "Test.h"

unsigned int count (0);
Mutex waitThread;
Mutex waitParent;

const unsigned int COUNT (1000);

void* sum (void*) {
   waitParent.lock ();
   waitThread.lock ();
   std::cout << "In Thread\n";
   for (unsigned int i (0); i < COUNT; ++i)
       ++count;
   waitParent.unlock ();
   return NULL;
}

int main (int argc, char* argv[]) {
   unsigned int cErrors (0);

   Thread* thread (Thread::create (&sum, NULL));
   check (thread);
   while (waitThread.trylock ()) {
      waitThread.unlock ();
      sleep (0);
   }
   waitParent.lock ();
   check (count == COUNT);

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}
