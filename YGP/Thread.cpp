//$Id: Thread.cpp,v 1.3 2002/05/09 22:21:59 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Thread
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 28.4.2002
//COPYRIGHT   : Anticopyright (A) 2002

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


#include "Internal.h"

#include <errno.h>
#include <assert.h>

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#else
#  if SYSTEM == UNIX
#     include <unistd.h>
#     include <sys/wait.h>
#  else
#     include <stdio.h>
#  endif
#include <stdlib.h>
#endif

#define DEBUG 0
#include <Trace_.h>

#include "Thread.h"

/*--------------------------------------------------------------------------*/
//Purpose   : Defaultconstructor
/*--------------------------------------------------------------------------*/
Thread::Thread () : paArgs_ (NULL), id (0) {
   TRACE3 ("Thread::Thread ()");
#ifndef HAVE_LIBPTHREAD
   canceled = false;
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: fnc: Function to be called in the thread
//            paArgs: (Array of) pointer to argument(s)
/*--------------------------------------------------------------------------*/
Thread::Thread (THREAD_FUNCTION fnc, void* paArgs) throw (std::string)
   : paArgs_ (paArgs) {
   TRACE3 ("Thread::Thread (THREAD_FUNCTION, void*)");
   init (fnc, paArgs);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
Thread::~Thread () {
   TRACE3 ("Thread::~Thread ()");
   waitForThread (*this);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Initializes the tread
//Parameters: fnc: Function to be called in the thread
//            paArgs: (Array of) pointer to argument(s)
/*--------------------------------------------------------------------------*/
void Thread::init (THREAD_FUNCTION fnc, void* paArgs) throw (std::string) {
#ifdef HAVE_LIBPTHREAD
   if (pthread_create (&id, NULL, fnc, this) != 0) {
      std::string err (_("Can't create thread!\nReason: %1"));
      err.replace (err.find ("%1"), 2, strerror (errno));
      throw (err);
   }
#else
   canceled = false;
   id = fork ();
   switch (id) {
   case 0:                                  // Child-part: Call passed function
      ret (fnc (this));

   case -1: {                        // Error creating process: Throw exception
      std::string
      std::string err (_("Can't create background-process!\nReason: %1"));
      err.replace (err.find ("%1"), 2, strerror (errno));
      throw (err);
   }
   } // end-switch
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Ends the thread
/*--------------------------------------------------------------------------*/
void Thread::ret (void* rc) const {
#ifdef HAVE_LIBPTHREAD
   pthread_exit (rc);
#else
   exit (rc ? *static_cast<int*> (rc) : -1);
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Ends the thread
/*--------------------------------------------------------------------------*/
void Thread::cancel () {
#ifdef HAVE_LIBPTHREAD
   pthread_cancel (id);
#else
   canceled = true;
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Waits for the passed thread to terminate
//Parameters: id: Thread to wait for
//Returns   : void* 
/*--------------------------------------------------------------------------*/
void* Thread::waitForThread (const Thread& id) {
#ifdef HAVE_LIBPTHREAD
   void* rc;
   pthread_join (id.id, &rc);
   return rc;
#else
   int rc;
   wait (id.id, NULL, 0);
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the tread should be canceled and does so, if yes
/*--------------------------------------------------------------------------*/
void Thread::isToCancel () const {
#ifdef HAVE_LIBPTHREAD
   pthread_testcancel ();
#else
   if (canceled)
      exit (-1);
#endif
}
