#ifndef THREAD_H
#define THREAD_H

//$Id: Thread.h,v 1.1 2002/05/01 00:42:15 markus Exp $

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

#include <string>

// Class to execute a certain function 
class Thread {
 public:
   typedef void* (*THREAD_FUNCTION) (void*);

   virtual ~Thread ();

   static Thread* create (THREAD_FUNCTION fnc, void* paArgs) throw (std::string) {
      return new Thread (fnc, paArgs); }

   void* getArgs () const { return paArgs_; }

   void ret (void* rc) const;
   void cancel ();

   static void* waitForThread (const Thread& id);

   void isToCancel () const;

 protected:
   Thread ();
   Thread (THREAD_FUNCTION fnc, void* paArgs) throw (std::string);

   void init (THREAD_FUNCTION fnc, void* paArgs) throw (std::string);

   void* paArgs_;

 private:
#ifdef HAVE_PTHREAD_H
   pthread_t id;
#else
   pid_t id;
   bool  canceled;
#endif
};

template <class T> class OThread : public Thread {
 public:
   typedef void* (T::*THREAD_OBJMEMBER) (void*);

   ~OThread () { }

   static OThread<T>* create (T& obj, THREAD_OBJMEMBER fnc, void* paArgs)
                              throw (std::string) {
      OThread<T>* t (new OThread<T> (obj, fnc, paArgs)); }

 protected:
   OThread (T& obj, THREAD_OBJMEMBER fnc, void* paArgs) throw (std::string) 
      : Thread (), object (obj), callback (fnc) {
      // Don't create Thread directly with data, because the thread might start
      // without object and callback beeing initialized!!
      init (&proxy, this);
      paArgs_ = paArgs; }

 private:
   static void* proxy (void* pArgs) {
      OThread<T>* thread (static_cast <OThread<T>*> (pArgs));
      void* rc = ((thread->object).*(thread->callback)) (thread->getArgs ());
      delete thread;
      return rc; }

   T& object;
   THREAD_OBJMEMBER callback;
};

#endif
