//$Id: Folder.cpp,v 1.10 2004/09/14 04:03:16 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Folder
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
//AUTHOR      : Markus Schwab
//CREATED     : 04.07.2003
//COPYRIGHT   : Copyright (C) 2003, 2004

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


#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "XGP/Folder.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
Folder::Folder ()
    : Gtk::ScrolledWindow (), view (1, 1), rows (0), cols (0)
      , actCol (0), width (-1) {
   init ();
}

//-----------------------------------------------------------------------------
/// Constructor; with two adjustment bars for the scrolled window
/// @param hadjustment: Horizontal adjustment bar
/// @param vadjustment: Vertical adjustment bar
//-----------------------------------------------------------------------------
Folder::Folder (Gtk::Adjustment& hadjustment, Gtk::Adjustment& vadjustment)
    : Gtk::ScrolledWindow (), view (1, 1), rows (0), cols (0)
      , actCol (0), width (-1) {
   init ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
Folder::~Folder () {
}


//-----------------------------------------------------------------------------
/// Adding a child the the container
/// \param child: Child widget to add
//-----------------------------------------------------------------------------
void Folder::add (Gtk::Widget& child) {
   TRACE9 ("Folder::add (Gtk::Widget&) - Act. pos: " << cols << '/' << rows);

   Gtk::Requisition req;
   child.size_request (req);

   Check3 (actCol <= colWidths.size ());
   unsigned int lineWidth (0);
   for (unsigned int i (0); i < actCol; ++i)
      lineWidth += colWidths[i];
   TRACE1 ("Folder::add (Gtk::Widget&) - Adding: " << req.width << " to "
           << lineWidth << " -> " << width);
   
   if ((req.width + lineWidth + 5) < static_cast<unsigned int> (width - 30)) {
      lineWidth += req.width + 5;
      if (actCol >= cols) {
         view.resize (rows, ++cols);
         colWidths.push_back (0);
      }
   }
   else {
      view.resize (++rows, cols);
      actCol = 0;
      lineWidth = req.width + 5;
   }
   Check3 (actCol < colWidths.size ());
   if (colWidths[actCol] < static_cast<unsigned int> (req.width + 12))
       colWidths[actCol] = req.width + 5;

   // Add the control to the table
   ++actCol;
   TRACE3 ("Folder::add (Gtk::Widget&) - Adding to: " << actCol << '/' << rows);
   view.attach (child, actCol, actCol + 1, rows, rows + 1, Gtk::SHRINK,
                Gtk::SHRINK, 0, 0);
}


//-----------------------------------------------------------------------------
/// Resize-request: Resort the children
//-----------------------------------------------------------------------------
void Folder::on_size_allocate (Gtk::Allocation& size) {
   Check1 (size.get_width () >= -1); Check1 (size.get_height () >= -1);
   TRACE9 ("Folder::on_size_allocate (GtkAllocation*) -> new size: "
           << size.get_width () << " * " << size.get_height ());

   Gtk::ScrolledWindow::on_size_allocate (size);

   if (width != size.get_width ()) {
      width = size.get_width ();
      std::vector<Gtk::Widget*> widgets;
      for (Gtk::Table::TableList::const_iterator i (view.children ().begin ());
           i != view.children ().end (); ++i) {
         Gtk::Widget* obj (i->get_widget ());
         Check3 (obj);
         obj->reference ();
         widgets.push_back (obj);
      }
      while (view.children ().begin () != view.children ().end ())
         view.remove (*view.children ().begin ()->get_widget ());

      rows = 1;
      cols = actCol = 0;
      colWidths.clear ();
      // Gtk seems to return the children in reverse order as they have been
      // added, so add them reversed again
      for (std::vector<Gtk::Widget*>::reverse_iterator i (widgets.rbegin ());
          i != widgets.rend (); ++i) {
         Check1 (*i);
         add (**i);
         (*i)->unreference ();
      }
   }
}

//-----------------------------------------------------------------------------
/// Initializes the controls and member variables
//-----------------------------------------------------------------------------
void Folder::init () {
   set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   set_shadow_type (Gtk::SHADOW_IN);
   Gtk::ScrolledWindow::add (view);
   view.set_spacings (5);
   view.show ();
}

}
