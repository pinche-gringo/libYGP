// PROJECT     : libXGP
// SUBSYSTEM   : AnimatedWindow
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 20.05.2007
// COPYRIGHT   : Copyright (C) 2007, 2008, 2012, 2026

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

#include <gtkmm/fixed.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "AnimWindow.h"

namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param parent Gtk::Fixed containing the widget
/// \param widget Widget to animate; must be a child of \a parent
//-----------------------------------------------------------------------------
AnimatedWindow::AnimatedWindow(Gtk::Fixed& parent, Gtk::Widget& widget) : fixed(parent), widget(widget), steps(10) {
    TRACE9("AnimatedWindow::AnimatedWindow(Gtk::Fixed&, Gtk::Widget&)");
    Check1(widget.get_parent() == &parent);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
AnimatedWindow::~AnimatedWindow() {
    TRACE9("AnimatedWindow::~AnimatedWindow()");
    connTimer.disconnect();
}

//-----------------------------------------------------------------------------
/// Starts the animation of the object
//-----------------------------------------------------------------------------
void AnimatedWindow::animate() {
    start();
    if (widget.get_mapped()) {
        steps = 10;
        widget.signal_destroy().connect(sigc::mem_fun(*this, &AnimatedWindow::end));
        connTimer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &AnimatedWindow::animationStep), 20);
    }
    else {
        // Not visible: Just move the widget to its end-position
        double x, y;
        getEndPos(x, y);
        fixed.move(widget, x, y);
        end();
    }
}

//-----------------------------------------------------------------------------
/// Performs a single step of the animated
/// \returns bool True, if further steps are to be performed
//-----------------------------------------------------------------------------
bool AnimatedWindow::animationStep() {
    TRACE8("AnimatedWindow::animationStep() - " << steps);

    if (steps--) {
        double x, y;
        getEndPos(x, y);
        animateTo(x, y);
        return true;
    }

    end();
    return false;
}

//-----------------------------------------------------------------------------
/// Ends the animation and deletes the object
//-----------------------------------------------------------------------------
void AnimatedWindow::end() {
    connTimer.disconnect();
    cleanup();
    finish();
    delete this;
}

//-----------------------------------------------------------------------------
/// Moves the widget one step closer to the passed position; in the last step
/// it is placed exactly there.
/// \param x X-coordinate of end-position (relative to the Gtk::Fixed)
/// \param y Y-coordinate of end-position (relative to the Gtk::Fixed)
//-----------------------------------------------------------------------------
void AnimatedWindow::animateTo(double x, double y) {
    double x2, y2;
    fixed.get_child_position(widget, x2, y2);
    TRACE5("AnimatedWindow::animateTo(2x double) - Current " << x2 << '/' << y2);

    if (steps) {
        x = x2 + (x - x2) / (steps + 1);
        y = y2 + (y - y2) / (steps + 1);
    }
    fixed.move(widget, x, y);
    TRACE5("AnimatedWindow::animateTo(2x double) - Moving to " << x << '/' << y);
}

//-----------------------------------------------------------------------------
/// Additional actions before starting the animation
//-----------------------------------------------------------------------------
void AnimatedWindow::start() {}

//-----------------------------------------------------------------------------
/// Called when ending the animation; perform your cleanup here
//-----------------------------------------------------------------------------
void AnimatedWindow::cleanup() {}

//-----------------------------------------------------------------------------
/// Additional actions after ending the animation
//-----------------------------------------------------------------------------
void AnimatedWindow::finish() {}

} // namespace XGP
