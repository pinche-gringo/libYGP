#ifndef YGP_THREAD_H
#define YGP_THREAD_H

//$Id: Thread.h,v 1.24 2008/05/18 13:21:27 markus Rel $

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


#include <ygp-cfg.h>

#ifdef HAVE_LIBPTHREAD
#  include <pthread.h>
#else
#  if !defined (HAVE_BEGINTHREAD) && defined (HAVE__BEGINTHREAD)
#     define HAVE_BEGINTHREAD
#     define beginthread    _beginthread
#     define endthread      _endthread
#  endif

#  if HAVE_WINDOWS_H
#     define WIN32_LEAN_AND_MEAN
#     include <windows.h>
      typedef void* (WINTHREAD_FUNCTION) (void*);
#  elif !defined (HAVE_BEGINTHREAD)
#      include <unistd.h>
#      include <sys/types.h>
#   endif
#endif

#include <YGP/Exception.h>


namespace YGP {

/**Class to execute a certain function in a thread

   There are two create-functions: The first calles the passed function with
   the parameters directly; the second passed a pointer to the created thread
   and enables to get the paramters with its getArgs-method. This enables
   controlling the thread.
*/
class Thread {
 public:
   /// Declaration of prototype of callback.
   typedef void* (*THREAD_FUNCTION) (void*);

   virtual ~Thread ();

   /// Creates a new thread; the argument is passed directly to the thread function
   /// \param fnc Thread function to execute
   /// \param pArgs Argument to the thread
   static Thread* create (THREAD_FUNCTION fnc, void* pArgs) throw (YGP::ExecError) {
      return new Thread (fnc, pArgs); }
   /// Creates a new thread; a pointer to the thread ID is passed to the thread function
   /// \param fnc Thread function to execute
   /// \param pArgs Argument to the thread
   static Thread* create2 (THREAD_FUNCTION fnc, void* pArgs) throw (YGP::ExecError) {
      Thread* t = new Thread;
      t->pArgs_ = pArgs;
      t->init (fnc, t);
      return t; }

   /// Returns the argument passed to the thread
   void* getArgs () const { return pArgs_; }

   /// \name Termination of the thread
   //@{
   void allowCancelation (bool allow = true);
   void cancel ();
   static void* waitForThread (const Thread& id);
   static void* waitForThread (unsigned long threadID);
   void isToCancel () const;
   //@}

   /// Get the ID of the thread
   unsigned long getID () const { return (unsigned long)id; }
   /// Get the ID of the currently running thread
   static unsigned long currentID () {
#ifdef HAVE_LIBPTHREAD
      return (unsigned long)pthread_self ();
#elif defined HAVE_BEGINTHREAD
#  ifdef HAVE_WINDOWS_H
      return (unsigned long)GetCurrentThreadId ();
#  else
#     warning Method Thread::currentID () does not work!
      return (unsigned long)-1;
#  endif
#else
      return (unsigned long)getpid ();
#endif
   }

 protected:
   Thread ();
   Thread (THREAD_FUNCTION fnc, void* pArgs) throw (YGP::ExecError);

   void ret (void* rc) const;
   void init (THREAD_FUNCTION fnc, void* pArgs) throw (YGP::ExecError);

   void* pArgs_;             ///< Pointer to (array of) arguments to the thread

 private:
   Thread (const Thread&);
   Thread& operator= (const Thread&);

#ifdef HAVE_LIBPTHREAD
   pthread_t id;
#else
   bool  canceled;

#  ifdef HAVE_BEGINTHREAD
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
   /// Declaration of type of callback
   typedef void* (T::*THREAD_OBJMEMBER) (void*);

   /// Destructor
   ~OThread () { }

   /// Creates a new thread; the argument is passed directly to the thread function
   /// \param obj Object having a member to execute in a thread
   /// \param fnc Member to execute as thread
   /// \param pArgs Argument to the thread
   static OThread<T>* create (T* obj, THREAD_OBJMEMBER fnc, void* pArgs) throw (YGP::ExecError) {
      return new OThread<T> (obj, fnc, pArgs); }
   /// Creates a new thread; a pointer to the thread is passed to the thread function
   /// \param obj Object having a member to execute in a thread
   /// \param fnc Member to execute as thread
   /// \param pArgs Argument to the thread
   static OThread<T>* create2 (T* obj, THREAD_OBJMEMBER fnc, void* pArgs) throw (YGP::ExecError) {
      return new OThread<T> (obj, fnc, pArgs, true); }


 protected:
   /// Constructor
   /// \param obj Object having a member to execute in a thread
   /// \param fnc Member to execute as thread
   /// \param pArgs Argument to the thread
   /// \param threadAsArg Flag, if the thread expects its argument directly
   OThread (T* obj, THREAD_OBJMEMBER fnc, void* pArgs, bool threadAsArg = false)
      throw (YGP::ExecError)
      : Thread (), indirect (threadAsArg), object (obj), callback (fnc) {
      // Don't create Thread directly with data, because the thread might start
      // without object and callback being initialized!!
      pArgs_ = pArgs;
      init (&proxy, this); }

 private:
   OThread (const OThread&);
   OThread& operator= (const OThread&);

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

}

#endif
