#ifndef MUTEX_H
#define MUTEX_H

//$Id: Mutex.h,v 1.9 2004/10/14 04:00:37 markus Rel $

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


#include <ygp-cfg.h>

#ifdef HAVE_LIBPTHREAD
#  include <pthread.h>
#elif HAVE_WINDOWS_H
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  error Not yet implemented!
#endif


namespace YGP {

/**Class for mutual exclusive devices.

   A mutual exclusive device is useful for protecting shared data structures
   from concurrent modifications, and implementing critical sections and
   monitors.

   A mutex has two possible states: unlocked (not owned by any thread), and
   locked (owned by one thread). A mutex can never be owned by two different
   threads simultaneously. A thread attempting to lock a mutex that is already
   locked by another thread is suspended until the owning thread unlocks the
   mutex first.
*/
class Mutex {
 public:
#ifdef HAVE_LIBPTHREAD
   /// Constructor; creates an (unlocked) mutex
   Mutex ()  { pthread_mutex_init (&id, NULL); }
   /// Destructor; the mutex is destroyed
   ~Mutex () { pthread_mutex_destroy (&id); }

   /// Tries to lock the mutex, but without blocking, if it is already locked
   bool trylock () { return !pthread_mutex_trylock (&id); }
   /// Lock the mutex; the thread blocks, if it is already locked
   void lock () { pthread_mutex_lock (&id); }
   /// Unlock a previously locked mutex
   void unlock () { pthread_mutex_unlock (&id); }
#elif HAVE_WINDOWS_H
   /// Constructor; creates an (unlocked) mutex
   Mutex () : hMutex (CreateMutex (NULL, false, NULL)) { }
   /// Destructor; the mutex is destroyed
   ~Mutex () { ReleaseMutex (hMutex); }

   /// Tries to lock the mutex, but without blocking, if it is already locked
   bool trylock () { return WaitForSingleObject (hMutex, 0) == WAIT_OBJECT_0; }
   /// Lock the mutex; the thread blocks, if it is already locked
   void lock () { WaitForSingleObject (hMutex, INFINITE); }
   /// Unlock a previously locked mutex
   void unlock () { ReleaseMutex (hMutex); }
#endif

 private:
#ifdef HAVE_LIBPTHREAD
   pthread_mutex_t id;
#elif HAVE_WINDOWS_H
   HANDLE hMutex;
#endif
};

}

#endif
