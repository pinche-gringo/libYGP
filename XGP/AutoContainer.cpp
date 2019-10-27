//$Id: AutoContainer.cpp,v 1.13 2008/03/30 13:39:17 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : AutoContainer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.13 $
//AUTHOR      : Markus Schwab
//CREATED     : 04.07.2003
//COPYRIGHT   : Copyright (C) 2003, 2004, 2008, 2011

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


#include <gtk/gtk.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "XGP/AutoContainer.h"


#include <gtkmm/label.h>


namespace XGP {

//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
AutoContainer::AutoContainer () : Gtk::ScrolledWindow (), view (), width (-1) {
   init ();
}

//-----------------------------------------------------------------------------
/// Constructor; with two adjustment bars for the scrolled window
/// @param hadjustment: Horizontal adjustment bar
/// @param vadjustment: Vertical adjustment bar
//-----------------------------------------------------------------------------
AutoContainer::AutoContainer (const Glib::RefPtr<Gtk::Adjustment>& hadjustment,
			      const Glib::RefPtr<Gtk::Adjustment>& vadjustment)
   : Gtk::ScrolledWindow (hadjustment, vadjustment), view (), width (-1) {
   init ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
AutoContainer::~AutoContainer () {
}


//-----------------------------------------------------------------------------
/// Adding a child the the container
/// \param child Child widget to add
//-----------------------------------------------------------------------------
void AutoContainer::add (Gtk::Widget& child) {
   TRACE9 ("AutoContainer::add (Gtk::Widget&) - To " << view.children ().size ());
   TRACE9 ("AutoContainer::add (Gtk::Widget&) - Child " << &child);
   Check2 (view.children ().size ());

   Gtk::Widget* last_ (view.get_children ().back ());
   Gtk::HBox* line (dynamic_cast<Gtk::HBox*> (last_));

   // Check if the widget fits into the line
   int lineX, lineY, childX, childY;
   line->get_size_request (lineX, lineY);
   child.get_size_request (childX, childY);

   if (lineX && ((childY + lineY + 10) > width)) {
      TRACE9 ("AutoContainer::checkLast (Gtk::HBox*, Gtk::Widget*) - Handle " <<  &child);
      line->show ();
      line = addLine ();
   }

   line->pack_start (child, Gtk::PACK_SHRINK, 5);
}

//-----------------------------------------------------------------------------
/// Resize-request: Resize a line
/// \param size New size of the container
/// \param line Line to resize
//-----------------------------------------------------------------------------
void AutoContainer::line_size_allocate (Gtk::Allocation& size, Gtk::HBox* line) {
   Check1 (size.get_width () >= -1); Check1 (size.get_height () >= -1);
   TRACE9 ("AutoContainer::line_size_allocate (Gtk::Allocation*, Gtk::HBox*) - Resize "
           << line  << " to " << size.get_width () << " * " << size.get_height ());
   Check3 (line);

   int lineX, lineY;
   line->get_size_request (lineX, lineY);

   if ((size.get_width () - 8) < lineY) {
      // First find line which has been resized
      for (std::vector<Gtk::Widget*>::const_iterator i (view.get_children ().begin ());
            i != view.get_children ().end (); ++i)
           if ((*i == line) && (line->get_children ().size () > 1)) {
              TRACE9 ("AutoContainer::line_size_allocate (Gtk::Allocation*, Gtk::HBox*) - Moving last elem");

              std::vector<Gtk::Widget*>::reverse_iterator j (line->get_children ().rbegin ());
              Check3 (*i);
              (*i)->reference ();
              line->remove (**i);

              if (++i == view.get_children ().end ())
                 line = addLine ();
              else
                 line = dynamic_cast<Gtk::HBox*> (*i);

              line->pack_start (**i, Gtk::PACK_SHRINK, 5);
              line->reorder_child (**i, 0);
              (*i)->unreference ();
           }
   }
}

//-----------------------------------------------------------------------------
/// Resize-request: Re-arrange the children
/// \param size New size of the container
//-----------------------------------------------------------------------------
void AutoContainer::on_size_allocate (Gtk::Allocation& size) {
   Check1 (size.get_width () >= -1); Check1 (size.get_height () >= -1);
   TRACE9 ("AutoContainer::on_size_allocate (Gtk::Allocation*) - new size: "
           << size.get_width () << " * " << size.get_height ());

   Gtk::ScrolledWindow::on_size_allocate (size);

   if (width != size.get_width ()) {
      width = size.get_width ();
      std::vector<Gtk::Widget*> widgets;
      for (std::vector<Gtk::Widget*>::const_iterator i (view.get_children ().begin ());
           i != view.get_children ().end (); ++i) {
         Gtk::HBox& line (*dynamic_cast<Gtk::HBox*> (*i));

         for (std::vector<Gtk::Widget*>::const_iterator j (line.get_children ().begin ());
              j != line.get_children ().end (); ++j) {
            TRACE9 ("AutoContainer::on_size_allocate (GtkAllocation*) - Storing " << *j);
            Check3 (*j);
            (*j)->reference ();
            widgets.push_back (*j);
         }
      }

      std::vector<Gtk::Widget*> children (view.get_children ());
      children.erase (children.begin (), children.end ());
      Check3 (view.get_children ().empty ());
      addLine ();
      Check3 (view.get_children ().size () == 1);

      for (std::vector<Gtk::Widget*>::iterator i (widgets.begin ());
           i != widgets.end (); ++i) {
         Check1 (*i);
         add (**i);
         (*i)->unreference ();
         (*i)->show ();
      }
   }
}

//-----------------------------------------------------------------------------
/// Initializes the controls and member variables
//-----------------------------------------------------------------------------
void AutoContainer::init () {
   set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   set_shadow_type (Gtk::SHADOW_IN);
   Gtk::ScrolledWindow::add (view);

   view.show ();
   addLine ();
}

//-----------------------------------------------------------------------------
/// Adds a new line of objects to the widget
/// \returns Gtk::HBox* The new created line
//-----------------------------------------------------------------------------
Gtk::HBox* AutoContainer::addLine () {
   TRACE9 ("AutoContainer::addLine ()");

   Gtk::HBox* line (new Gtk::HBox ());
   line->signal_size_allocate ().connect
       (bind (mem_fun (*this, &AutoContainer::line_size_allocate), line));
   line->show ();
   view.pack_start (*manage (line), Gtk::PACK_SHRINK, 5);
   return line;
}

//----------------------------------------------------------------------------
/// Removes the passed widget from the container
/// \param widget Widget to remove
//----------------------------------------------------------------------------
void AutoContainer::remove (Gtk::Widget& widget) {
   TRACE4 ("AutoContainer::remove (Gtk::Widget&)");

   for (std::vector<Gtk::Widget*>::const_iterator i (view.get_children ().begin ());
        i != view.get_children ().end (); ++i) {
      Gtk::HBox& line (*dynamic_cast<Gtk::HBox*> (*i));

      if (&line == &widget) {
         view.remove (widget);
         return;
      }
      else
         for (std::vector<Gtk::Widget*>::const_iterator j (line.get_children ().begin ());
              j != line.get_children ().end (); ++j)
            if (*j == &widget) {
               line.remove (widget);

               // Remove also the line, if it is empty (and not the last one)
               if ((line.get_children ().empty ()) && (view.get_children ().size () > 1))
                  view.remove (line);
               return;
            }
   }
   Check2 (0);
}

//----------------------------------------------------------------------------
/// Inserts a widget to a given position
/// \param widget Widget to insert
/// \param pos Position where to insert the widget
//----------------------------------------------------------------------------
void AutoContainer::insert (Gtk::Widget& widget, unsigned int pos) {
   TRACE4 ("AutoContainer::insert (Gtk::Widget&, unsigned int) - " << pos);

   for (std::vector<Gtk::Widget*>::const_iterator i (view.get_children ().begin ());
        i != view.get_children ().end (); ++i) {
      Gtk::HBox& line (*dynamic_cast<Gtk::HBox*> (*i));

      unsigned int count (line.get_children ().size ());
      if (pos > count)
         pos -= count;
      else {
         line.pack_start (widget, Gtk::PACK_SHRINK, 5);
         line.reorder_child (widget, pos);
         break;
      }
   }
}

}
