#ifndef XGP_ANIMWINDOW_H
#define XGP_ANIMWINDOW_H

//$Id: AnimWindow.h,v 1.4 2008/06/08 12:11:28 markus Rel $

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


#include <gdkmm/window.h>


namespace XGP {


/**Class animating objects.
 *
 * \note Create on heap (with new) as this class deletes itself when
 *       the animation has been finished.
 */
class AnimatedWindow : public sigc::trackable {
 public:
   virtual ~AnimatedWindow ();

   void animate ();

   virtual void getEndPos (int& x, int& y) = 0;
   virtual void start ();
   virtual void cleanup ();
   virtual void finish ();

 protected:
   AnimatedWindow (Glib::RefPtr<Gdk::Window> window);

   void animateTo (int x, int y);

   Glib::RefPtr<Gdk::Window> win;

 private:
   AnimatedWindow ();
   AnimatedWindow (const AnimatedWindow& other);
   const AnimatedWindow& operator= (const AnimatedWindow& other);

   bool animationStep ();

   unsigned int steps;
};

}

#endif
