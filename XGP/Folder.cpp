//$Id: Folder.cpp,v 1.5 2003/11/14 00:23:56 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Folder
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
//AUTHOR      : Markus Schwab
//CREATED     : 04.07.2003
//COPYRIGHT   : Anticopyright (A) 2003

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
#include <YGP/Trace_.h>

#include "XGP/Folder.h"


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

   int childHeight, childWidth;
   child.get_size_request (childWidth, childHeight);

   Check3 (actCol <= colWidths.size ());
   unsigned int lineWidth (0);
   for (unsigned int i (0); i < actCol; ++i)
      lineWidth += colWidths[i];
   TRACE1 ("Folder::add (Gtk::Widget&) - Adding: " << childWidth << " to "
           << lineWidth << " -> " << width);
   
   if ((childWidth + lineWidth + 5) < static_cast<unsigned int> (width - 30)) {
      lineWidth += childWidth + 5;
      if (actCol >= cols) {
         view.resize (rows, ++cols);
         colWidths.push_back (0);
      }
   }
   else {
      view.resize (++rows, cols);
      actCol = 0;
      lineWidth = childWidth + 5;
   }
   Check3 (actCol < colWidths.size ());
   if (colWidths[actCol] < static_cast<unsigned int> (childWidth + 12))
       colWidths[actCol] = childWidth + 5;

   // Add the control to the table
   ++actCol;
   TRACE3 ("Folder::add (Gtk::Widget&) - Adding to: " << actCol << '/' << rows);
   view.attach (child, actCol, actCol + 1, rows, rows + 1, Gtk::SHRINK,
                Gtk::SHRINK, 0, 0);
}


//-----------------------------------------------------------------------------
/// Resize-request: Resort the children
//-----------------------------------------------------------------------------
void Folder::on_size_allocate (GtkAllocation* size) {
   Check1 (size); Check1 (size->width >= -1); Check1 (size->height >= -1);
   TRACE9 ("Folder::on_size_allocate (GtkAllocation*) -> new size: "
           << size->width << " * " << size->height);

   Gtk::ScrolledWindow::on_size_allocate (size);

   if (width != size->width) {
      width = size->width;
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
