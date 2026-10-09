//$Id: AnimWindow.cpp,v 1.1 2008/05/18 13:18:21 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : XGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 07.04.2008
//COPYRIGHT   : Copyright (C) 2008

// This file is part of libYGP.
//
// libYGP is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include "AnimWindow.h"


//-----------------------------------------------------------------------------
/// Constructor
/// \param parent: Gtk::Fixed containing the widget
/// \param widget: Widget to animate
//-----------------------------------------------------------------------------
AnimWindow::AnimWindow (Gtk::Fixed& parent, Gtk::Widget& widget)
   : XGP::AnimatedWindow (parent, widget) {
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
AnimWindow::~AnimWindow () {
}


//-----------------------------------------------------------------------------
/// Retrieves the position where to animate the widget to
//-----------------------------------------------------------------------------
void AnimWindow::getEndPos (double& x, double& y) {
   x = 200;
   y = 100;
}
