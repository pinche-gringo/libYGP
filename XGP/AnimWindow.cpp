//PROJECT     : libXGP
//SUBSYSTEM   : AnimatedWindow
//REFERENCES  :
//TODO        :
//BUGS        :
//AUTHOR      : Markus Schwab
//CREATED     : 20.05.2007
//COPYRIGHT   : Copyright (C) 2007, 2008, 2012, 2026

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


#include <glibmm/main.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "AnimWindow.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param window Window to animate
//-----------------------------------------------------------------------------
AnimatedWindow::AnimatedWindow (Glib::RefPtr<Gdk::Surface> window)
   : win (window), steps (10) {
   TRACE9 ("AnimatedWindow::AnimatedWindow ()");
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
AnimatedWindow::~AnimatedWindow () {
   TRACE9 ("AnimatedWindow::~AnimatedWindow ()");
}


//-----------------------------------------------------------------------------
/// Starts the animation of the object
//-----------------------------------------------------------------------------
void AnimatedWindow::animate () {
   Check1 (win);

   start ();
   if (win->get_mapped ()) {
      steps = 10;
      Glib::signal_timeout ().connect (sigc::mem_fun (*this, &AnimatedWindow::animationStep), 20);
   }
   else {
      cleanup ();
      finish ();
      delete this;
   }
}

//-----------------------------------------------------------------------------
/// Performs a single step of the animated
/// \returns bool True, if further steps are to be performed
//-----------------------------------------------------------------------------
bool AnimatedWindow::animationStep () {
   TRACE8 ("AnimatedWindow::animationStep () - " << steps);

   if (steps--) {
      int x, y;
      getEndPos (x, y);
      animateTo (x, y);
      return true;
   }

   cleanup ();
   finish ();
   delete this;
   return false;
}

//-----------------------------------------------------------------------------
/// Animates a window to the passed position
/// \param x X-coordinate of end-position (in root coordinates)
/// \param y Y-coordinate of end-position (in root coordinates)
/// \remarks No-op under GTK4: Gdk::Surface/Toplevel expose no way for a
///     client to query or set a toplevel's position (see the class remarks).
//-----------------------------------------------------------------------------
void AnimatedWindow::animateTo (int, int) {
   Check1 (win);
}

//-----------------------------------------------------------------------------
/// Additional actions before starting the animation
//-----------------------------------------------------------------------------
void AnimatedWindow::start () {
}

//-----------------------------------------------------------------------------
/// Called when ending the animation; perform your cleanup here
//-----------------------------------------------------------------------------
void AnimatedWindow::cleanup () {
}

//-----------------------------------------------------------------------------
/// Additional actions after ending the animation
//-----------------------------------------------------------------------------
void AnimatedWindow::finish () {
}

}
