//$Id: Thread.cpp,v 1.27 2008/06/10 21:47:20 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Thread
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.27 $
//AUTHOR      : Markus Schwab
//CREATED     : 28.4.2002
//COPYRIGHT   : Copyright (C) 2002 - 2004, 2006, 2008

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


#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif


#include "YGP/Internal.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>

#include <string>

#include "YGP/Check.h"
#include "YGP/Trace.h"

#include "YGP/Thread.h"

#if HAVE_LIBPTHREAD
#   include <unistd.h>
#   include <sys/wait.h>
#else
#  ifdef HAVE_BEGINTHREAD
#     include <process.h>
#     include <map>
#     include "YGP/Mutex.h"

      // Map of wait-mutexes (1 per thread
      static std::map<unsigned long, YGP::Mutex> mutexes;
      static std::map<unsigned long, void*> rcs;
#  endif
#  include <cstdio>
#endif


namespace YGP {

//-----------------------------------------------------------------------------
/// Defaultconstructor; create the object but no actual thread
//-----------------------------------------------------------------------------
Thread::Thread ()
   : pArgs_ (NULL)
#ifdef HAVE_LIBPTHREAD
   , id (0)
#else
   , canceled (false)
#  ifdef HAVE_BEGINTHREAD
   , callback (NULL)
   , id (0)
#  else
   , id (0)
#  endif
#endif
 {
   TRACE3 ("Thread::Thread ()");
}

//-----------------------------------------------------------------------------
/// Constructor; Create object and a thread and passes pArgs as arguments
/// \param fnc Function to be called in the thread
/// \param pArgs Pointer to argument(s)
/// \throw YGP::ExecError describing the error
//-----------------------------------------------------------------------------
Thread::Thread (THREAD_FUNCTION fnc, void* pArgs) throw (YGP::ExecError)
   : pArgs_ (pArgs)
#ifdef HAVE_LIBPTHREAD
   , id ()
#endif
{
   TRACE3 ("Thread::Thread (THREAD_FUNCTION, void*)");
   init (fnc, pArgs);

   TRACE9 ("Thread::Thread (THREAD_FUNCTION, void*) -id = " << (int)id);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
Thread::~Thread () {
   TRACE3 ("Thread::~Thread ()");
}


//-----------------------------------------------------------------------------
/// Creates the actual thread from the passed function
/// \param fnc Function to be called in the thread
/// \param pArgs Pointer to parameters
/// \throw YGP::ExecError describing the error
//-----------------------------------------------------------------------------
void Thread::init (THREAD_FUNCTION fnc, void* pArgs) throw (YGP::ExecError) {
#ifdef HAVE_LIBPTHREAD
   if (pthread_create (&id, NULL, fnc, pArgs) != 0) {
#elif  defined (HAVE_BEGINTHREAD)
   callback = fnc;
   mutexes[id = _beginthread (threadFunction, 0, this)].lock ();
   if (id == -1U) {
      mutexes[-1U].unlock ();
#endif

#if defined (HAVE_LIBPTHREAD) || defined (HAVE_BEGINTHREAD)
      std::string err (_("Can't create thread!\nReason: %1"));
      err.replace (err.find ("%1"), 2, strerror (errno));
      throw (YGP::ExecError (err));
   }
#else
   canceled = false;
   id = fork ();
   switch (id) {
   case 0:                                  // Child-part: Call passed function
      ret (fnc (this));

   case -1: {                        // Error creating process: Throw exception
      std::string err (_("Can't create background-process!\nReason: %1"));
      err.replace (err.find ("%1"), 2, strerror (errno));
      throw (YGP::ExecError (err));
      }
   } // end-switch
#endif
   TRACE9 ("Thread::init (THREAD_FUNCTION) -id = " << (int)id);
}

//-----------------------------------------------------------------------------
/// Sets the return value for the thread
/// \param rc Returncode
//-----------------------------------------------------------------------------
void Thread::ret (void* rc) const {
#ifdef HAVE_LIBPTHREAD
   pthread_exit (rc);
#elif defined HAVE_BEGINTHREAD
   rcs[id] = rc;
#else
   _exit (rc ? *static_cast<int*> (rc) : -1);
#endif
}

//-----------------------------------------------------------------------------
/// Terminates the thread
/// \remarks Note that not all plattforms (e.g. Windows) support to cancel a
///     thread; in such a case the thread must check itself, if it should be
///     canceled.
//-----------------------------------------------------------------------------
void Thread::cancel () {
#ifdef HAVE_LIBPTHREAD
   pthread_cancel (id);
#else
   canceled = true;
#endif
}

//-----------------------------------------------------------------------------
/// Terminates the thread
/// \remarks Note that not all plattforms (e.g. Windows) support to cancel a
///     thread; in such a case the thread must check itself, if it should be
///     canceled.
//-----------------------------------------------------------------------------
void Thread::allowCancelation (bool allow) {
#ifdef HAVE_LIBPTHREAD
   pthread_setcancelstate (allow ? PTHREAD_CANCEL_ENABLE : PTHREAD_CANCEL_DISABLE, NULL);
#endif
}

//-----------------------------------------------------------------------------
/// Waits for the passed thread to terminate
/// \param id Thread to wait for
/// \returns void* The returncode of the thread
//-----------------------------------------------------------------------------
void* Thread::waitForThread (const Thread& id) {
#ifndef HAVE_BEGINTHREAD
    return waitForThread ((unsigned long)id.id);
#else
   waitForThread (id.id);
   return rcs[id.id];
#endif
}

//-----------------------------------------------------------------------------
/// Waits for the thread with the passed ID to terminate
/// \param id Thread to wait for
/// \returns void* The returncode of the thread
//-----------------------------------------------------------------------------
void* Thread::waitForThread (unsigned long id) {
   TRACE3 ("Thread::waitForThread (unsigned long) - " << id);

#ifdef HAVE_LIBPTHREAD
   void* rc (NULL);
   pthread_join ((pthread_t)id, &rc);
   return rc;
#elif defined HAVE_BEGINTHREAD
   mutexes[id].lock ();
   mutexes[id].unlock ();
   return rcs[id];
#else
   int rc (0);
   waitpid (id, &rc, 0);
   return (void*)rc;
#endif
}

//-----------------------------------------------------------------------------
/// Checks if the thread should be canceled and does so, if yes
//-----------------------------------------------------------------------------
void Thread::isToCancel () const {
#ifdef HAVE_LIBPTHREAD
   pthread_testcancel ();
#elif defined HAVE_BEGINTHREAD
   ExitThread (-1U);
#else
   if (canceled)
      _exit (-1);
#endif
}

#ifdef HAVE_BEGINTHREAD
//-----------------------------------------------------------------------------
/// Dummy thread-routine, because (of course) VC++6 has a differnt signature
/// for a thread-function and can't (of course) not cast it
/// \param params Pointer to thread
//-----------------------------------------------------------------------------
void Thread::threadFunction (void* params) {
   Thread* pThread = reinterpret_cast<Thread*> (params);
   Check1 (pThread);
   rcs[pThread->id] = pThread->callback (pThread);
   mutexes[pThread->id].unlock ();
}
#endif

}
