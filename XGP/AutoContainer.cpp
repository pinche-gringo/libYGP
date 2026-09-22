//PROJECT     : libXGP
//SUBSYSTEM   : AutoContainer
//REFERENCES  :
//TODO        :
//BUGS        :
//AUTHOR      : Markus Schwab
//CREATED     : 04.07.2003
//COPYRIGHT   : Copyright (C) 2003, 2004, 2008, 2011, 2026

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


#include <climits>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "XGP/AutoContainer.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
AutoContainer::AutoContainer () : Gtk::ScrolledWindow (), view () {
   init ();
}

//-----------------------------------------------------------------------------
/// Constructor; with two adjustment bars for the scrolled window
/// @param hadjustment: Horizontal adjustment bar
/// @param vadjustment: Vertical adjustment bar
//-----------------------------------------------------------------------------
AutoContainer::AutoContainer (const Glib::RefPtr<Gtk::Adjustment>& hadjustment,
			      const Glib::RefPtr<Gtk::Adjustment>& vadjustment)
   : Gtk::ScrolledWindow (), view () {
   init ();
   set_hadjustment (hadjustment);
   set_vadjustment (vadjustment);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
AutoContainer::~AutoContainer () = default;


//-----------------------------------------------------------------------------
/// Adding a child the the container
/// \param child Child widget to add
//-----------------------------------------------------------------------------
void AutoContainer::add (Gtk::Widget& child) {
   TRACE9 ("AutoContainer::add (Gtk::Widget&) - " << &child);

   view.append (child);
}

//-----------------------------------------------------------------------------
/// Initializes the controls and member variables
//-----------------------------------------------------------------------------
void AutoContainer::init () {
   set_policy (Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
   set_has_frame ();

   view.set_homogeneous ();
   view.set_selection_mode (Gtk::SelectionMode::NONE);
   view.set_row_spacing (5);
   view.set_column_spacing (5);
   view.set_max_children_per_line (UINT_MAX);        // Wrap purely by width

   set_child (view);
}

//----------------------------------------------------------------------------
/// Removes the passed widget from the container
/// \param widget Widget to remove
//----------------------------------------------------------------------------
void AutoContainer::remove (Gtk::Widget& widget) {
   TRACE4 ("AutoContainer::remove (Gtk::Widget&)");

   view.remove (widget);
}

//----------------------------------------------------------------------------
/// Inserts a widget to a given position
/// \param widget Widget to insert
/// \param pos Position where to insert the widget
//----------------------------------------------------------------------------
void AutoContainer::insert (Gtk::Widget& widget, unsigned int pos) {
   TRACE4 ("AutoContainer::insert (Gtk::Widget&, unsigned int) - " << pos);

   view.insert (widget, static_cast<int> (pos));
}

}
