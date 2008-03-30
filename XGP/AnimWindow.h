#ifndef ANIMWINDOW_H
#define ANIMWINDOW_H

//$Id: AnimWindow.h,v 1.1 2008/03/30 17:48:32 markus Rel $

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


#include <gtkmm/window.h>


namespace XGP {

/**Class animating objects.
 *
 * \note: Create on heap (with new) as this class deletes itself when the
 *        animation has been finished.
 */
class AnimatedWindow : public Gtk::Window {
 public:
   virtual ~AnimatedWindow ();

   void animate ();

   virtual void getEndPos (int& x, int& y) = 0;
   virtual void start ();
   virtual void cleanup ();
   virtual void finish ();

 protected:
   AnimatedWindow ();

   void animateTo (int x, int y);

 private:
   AnimatedWindow (const AnimatedWindow& other);
   const AnimatedWindow& operator= (const AnimatedWindow& other);

   bool animationStep ();

   unsigned int steps;
};

}

#endif
