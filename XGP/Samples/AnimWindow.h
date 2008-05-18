#ifndef ANIMWINDOW_H
#define ANIMWINDOW_H

//$Id: AnimWindow.h,v 1.1 2008/05/18 13:18:21 markus Rel $

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


#include <XGP/AnimWindow.h>


class AnimWindow : public XGP::AnimatedWindow {
 public:
   virtual ~AnimWindow ();
   static AnimWindow* create (Glib::RefPtr<Gdk::Window> window) {
      return new AnimWindow (window);
   }

   void getEndPos (int& x, int& y);

 protected:
   AnimWindow (Glib::RefPtr<Gdk::Window> window);

 private:
   AnimWindow ();
   AnimWindow (const AnimWindow& other);
   const AnimWindow& operator= (const AnimWindow& other);
};

#endif
