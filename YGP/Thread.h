#ifndef THREAD_H
#define THREAD_H

//$Id: Thread.h,v 1.10 2002/12/15 22:21:16 markus Rel $

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

#if SYSTEM == WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <process.h>
   typedef void* (WINTHREAD_FUNCTION) (void*);
#endif

#if !defined (HAVE_LIBPTHREAD) && !defined (HAVE_BEGINTHREAD)
#  include <unistd.h>
#  include <sys/types.h>
#endif

#include <string>

// Class to execute a certain function in a thread
// There are two create-functions: The first calles the passed function with
// the parameters directly; the second passed a pointer to the created thread
// and enables to get the paramters with its getArgs-method. This enables
// controlling the thread.
class Thread {
 public:
   typedef void* (*THREAD_FUNCTION) (void*);

   virtual ~Thread ();

   static Thread* create (THREAD_FUNCTION fnc, void* paArgs) throw (std::string) {
      return new Thread (fnc, paArgs); }
   static Thread* create2 (THREAD_FUNCTION fnc, void* paArgs) throw (std::string) {
      Thread* t = new Thread;
      t->paArgs_ = paArgs;
      t->init (fnc, t);
      return t; }

   void* getArgs () const { return paArgs_; }

   void cancel ();
   static void* waitForThread (const Thread& id);
   static void* waitForThread (unsigned long threadID);
   void isToCancel () const;

   unsigned long getID () const { return id; }
   static unsigned long currentID () {
#ifdef HAVE_LIBPTHREAD
      return pthread_self ();
#elif defined HAVE_BEGINTHREAD
      return GetCurrentThreadId ();
#else
      return getpid ();
#endif
   }

 protected:
   Thread ();
   Thread (THREAD_FUNCTION fnc, void* paArgs) throw (std::string);

   void ret (void* rc) const;
   void init (THREAD_FUNCTION fnc, void* paArgs) throw (std::string);

   void* paArgs_;

 private:
#ifdef HAVE_LIBPTHREAD
   pthread_t id;
#else
   bool  canceled;

#  if defined (HAVE_BEGINTHREAD)
   THREAD_FUNCTION callback;
   static void threadFunction (void* thread);
   unsigned long id;
#  else
   pid_t id;
#  endif
#endif
};

// Class to execute a member-function in a thread.
// There are two create-functions: The first calles the passed function with
// the parameters directly; the second passed a pointer to the created thread
// and enables to get the paramters with its getArgs-method. This enables
// controlling the thread.
template <class T> class OThread : public Thread {
 public:
   typedef void* (T::*THREAD_OBJMEMBER) (void*);

   ~OThread () { }

   static OThread<T>* create (T* obj, THREAD_OBJMEMBER fnc, void* paArgs)
                              throw (std::string) {
      return new OThread<T> (obj, fnc, paArgs); }
   static OThread<T>* create2 (T* obj, THREAD_OBJMEMBER fnc, void* paArgs)
                              throw (std::string) {
      return new OThread<T> (obj, fnc, paArgs, true); }


 protected:
   OThread (T* obj, THREAD_OBJMEMBER fnc, void* paArgs, bool threadAsArg = false)
      throw (std::string)
      : Thread (), object (obj), callback (fnc), indirect (threadAsArg) {
      // Don't create Thread directly with data, because the thread might start
      // without object and callback beeing initialized!!
      paArgs_ = paArgs;
      init (&proxy, this); }

 private:
   static void* proxy (void* pArgs) {
      OThread<T>* thread = static_cast <OThread<T>*> (pArgs);
      void* rc = ((thread->object)->*(thread->callback))
         (thread->indirect ? thread : thread->getArgs ());
      delete thread;
      return rc; }

   bool indirect;
   T* object;
   THREAD_OBJMEMBER callback;
};

#endif
