#ifndef MUTEX_H
#define MUTEX_H

//$Id: Mutex.h,v 1.4 2002/10/23 05:47:17 markus Rel $

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


#include <gzo-cfg.h>

#ifdef HAVE_LIBPTHREAD
#  include <pthread.h>
#elif SYSTEM == WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  error Not yet implemented!
#endif


// Class for mutual exclusive devices
class Mutex {
 public:
#ifdef HAVE_LIBPTHREAD
   Mutex ()  { pthread_mutex_init (&id, NULL); }
   ~Mutex () { pthread_mutex_destroy (&id); }

   bool trylock () { return !pthread_mutex_trylock (&id); }
   void lock () { pthread_mutex_lock (&id); }
   void unlock () { pthread_mutex_unlock (&id); }
#elif SYSTEM == WINDOWS
   Mutex () : hMutex (CreateMutex (NULL, false, NULL)) { }
   ~Mutex () { ReleaseMutex (hMutex); }

   bool trylock () { return WaitForSingleObject (hMutex, 0) == WAIT_OBJECT_0; }
   void lock () { WaitForSingleObject (hMutex, INFINITE); }
   void unlock () { ReleaseMutex (hMutex); }
#endif

 private:
#ifdef HAVE_LIBPTHREAD
   pthread_mutex_t id;
#elif SYSTEM == WINDOWS
   HANDLE hMutex;
#endif
};

#endif
