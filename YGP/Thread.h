#ifndef THREAD_H
#define THREAD_H

//$Id: Thread.h,v 1.11 2003/07/10 20:42:50 markus Exp $

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

/**Class to execute a certain function in a thread

   There are two create-functions: The first calles the passed function with
   the parameters directly; the second passed a pointer to the created thread
   and enables to get the paramters with its getArgs-method. This enables
   controlling the thread.
*/
class Thread {
 public:
   typedef void* (*THREAD_FUNCTION) (void*);

   virtual ~Thread ();

   /// Creates a new thread; the argument is passed directly to the thread
   /// \param fnc: Thread function to execute
   /// \param pArgs: Argument to the thread
   static Thread* create (THREAD_FUNCTION fnc, void* pArgs) throw (std::string) {
      return new Thread (fnc, pArgs); }
   /// Creates a new thread; the thread ID is passed to the thread
   /// \param fnc: Thread function to execute
   /// \param pArgs: Argument to the thread
   static Thread* create2 (THREAD_FUNCTION fnc, void* pArgs) throw (std::string) {
      Thread* t = new Thread;
      t->pArgs_ = pArgs;
      t->init (fnc, t);
      return t; }

   /// Returns the argument passed to the thread
   void* getArgs () const { return pArgs_; }

   /// \name Termination of the thread
   //@{
   void cancel ();
   static void* waitForThread (const Thread& id);
   static void* waitForThread (unsigned long threadID);
   void isToCancel () const;
   //@}

   /// Get the ID of the thread
   unsigned long getID () const { return id; }
   /// Get the ID of the currently running thread
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
   Thread (THREAD_FUNCTION fnc, void* pArgs) throw (std::string);

   void ret (void* rc) const;
   void init (THREAD_FUNCTION fnc, void* pArgs) throw (std::string);

   void* pArgs_;

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

/**Class to execute a member-function in a thread.

   There are two create-functions: The first calles the passed function with
   the parameters directly; the second passed a pointer to the created thread
   and enables to get the paramters with its getArgs-method. This enables
   controlling the thread.
*/
template <class T> class OThread : public Thread {
 public:
   typedef void* (T::*THREAD_OBJMEMBER) (void*);

   /// Destructor
   ~OThread () { }

   /// Creates a new thread; the argument is passed directly to the thread
   /// \param obj: Object having a member to execute in a thread
   /// \param fnc: Member to execute as thread
   /// \param pArgs: Argument to the thread
   static OThread<T>* create (T* obj, THREAD_OBJMEMBER fnc, void* pArgs)
                              throw (std::string) {
      return new OThread<T> (obj, fnc, pArgs); }
   /// Creates a new thread; the thread ID is passed to the thread
   /// \param obj: Object having a member to execute in a thread
   /// \param fnc: Member to execute as thread
   /// \param pArgs: Argument to the thread
   static OThread<T>* create2 (T* obj, THREAD_OBJMEMBER fnc, void* pArgs)
                              throw (std::string) {
      return new OThread<T> (obj, fnc, pArgs, true); }


 protected:
   /// Constructor
   /// \param obj: Object having a member to execute in a thread
   /// \param fnc: Member to execute as thread
   /// \param pArgs: Argument to the thread
   OThread (T* obj, THREAD_OBJMEMBER fnc, void* pArgs, bool threadAsArg = false)
      throw (std::string)
      : Thread (), object (obj), callback (fnc), indirect (threadAsArg) {
      // Don't create Thread directly with data, because the thread might start
      // without object and callback being initialized!!
      pArgs_ = pArgs;
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
