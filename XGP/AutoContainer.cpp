//$Id: AutoContainer.cpp,v 1.2 2003/11/12 23:37:46 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : AutoContainer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
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


#include <gtk/gtkmain.h>

#define CHECK 9
#define TRACELEVEL 9
#include <Check.h>
#include <Trace_.h>

#include "AutoContainer.h"


#include <gtkmm/label.h>


//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
AutoContainer::AutoContainer () : Gtk::ScrolledWindow (), width (-1) {
   init ();
}

//-----------------------------------------------------------------------------
/// Constructor; with two adjustment bars for the scrolled window
/// @param hadjustment: Horizontal adjustment bar
/// @param vadjustment: Vertical adjustment bar
//-----------------------------------------------------------------------------
AutoContainer::AutoContainer (Gtk::Adjustment& hadjustment, Gtk::Adjustment& vadjustment)
    : Gtk::ScrolledWindow (), width (-1) {
   init ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
AutoContainer::~AutoContainer () {
}


//-----------------------------------------------------------------------------
/// Adding a child the the container
/// \param child: Child widget to add
//-----------------------------------------------------------------------------
void AutoContainer::add (Gtk::Widget& child) {
   TRACE9 ("AutoContainer::add (Gtk::Widget&) - To " << view.children ().size ());
   TRACE9 ("AutoContainer::add (Gtk::Widget&) - Child " << &child);
   Check2 (view.children ().size ());

   Check3 (view.children ().size ());
   Gtk::Box_Helpers::Child& line_ (view.children ()[view.children ().size () - 1]);
   Gtk::HBox* line (dynamic_cast<Gtk::HBox*> (line_.get_widget ()));

   // Check if the widget fits into the line
   GtkRequisition lineReq, childReq;
   line->size_request (&lineReq);
   child.size_request (&childReq);

   if (lineReq.width && ((childReq.width + lineReq.width + 10) > width)) {
      TRACE9 ("AutoContainer::checkLast (Gtk::HBox*, Gtk::Widget*) - Handle " <<  &child);
      line->show ();
      line = addLine ();
   }

   line->pack_start (child, Gtk::PACK_SHRINK, 5);
}

//-----------------------------------------------------------------------------
/// Resize-request: Resize a line
/// \param size: New size of the container
//-----------------------------------------------------------------------------
void AutoContainer::line_size_allocate (GtkAllocation* size, Gtk::HBox* line) {
   Check1 (size); Check1 (size->width >= -1); Check1 (size->height >= -1);
   TRACE9 ("AutoContainer::line_size_allocate (GtkAllocation*, Gtk::HBox*) - Resize "
           << line  << " to " << size->width << " * " << size->height);
   Check3 (line);

   GtkRequisition lineReq;
   line->size_request (&lineReq);

   if ((size->width - 8) < lineReq.width) {
      // First find line which has been resized
       for (Gtk::Box::BoxList::const_iterator i (view.children ().begin ());
            i != view.children ().end (); ++i)
           if ((i->get_widget () == line) && (line->children ().size () > 1)) {
              TRACE9 ("AutoContainer::line_size_allocate (GtkAllocation*, Gtk::HBox*) - Moving last elem");

              Gtk::Box::BoxList::reverse_iterator j (line->children ().rbegin ());
              Gtk::Widget* obj (j->get_widget ());
              Check3 (obj);
              obj->reference ();
              line->remove (*obj);

              if (++i == view.children ().end ())
                 line = addLine ();
              else
                 line = dynamic_cast<Gtk::HBox*> (i->get_widget ());

              line->pack_start (*obj, Gtk::PACK_SHRINK, 5);
              line->reorder_child (*obj, 0);
              obj->unreference ();
           }
   }
}

//-----------------------------------------------------------------------------
/// Resize-request: Re-arrangethe children
/// \param size: New size of the container
//-----------------------------------------------------------------------------
void AutoContainer::on_size_allocate (GtkAllocation* size) {
   Check1 (size); Check1 (size->width >= -1); Check1 (size->height >= -1);
   TRACE9 ("AutoContainer::on_size_allocate (GtkAllocation*) - new size: "
           << size->width << " * " << size->height);

   Gtk::ScrolledWindow::on_size_allocate (size);

   if (width != size->width) {
      width = size->width;
      std::vector<Gtk::Widget*> widgets;
      for (Gtk::Box::BoxList::const_iterator i (view.children ().begin ());
           i != view.children ().end (); ++i) {
         Gtk::Box_Helpers::Child& line_ (*i);
         Gtk::HBox& line (*dynamic_cast<Gtk::HBox*> (line_.get_widget ()));

         for (Gtk::Box::BoxList::const_iterator j (line.children ().begin ());
              j != line.children ().end (); ++j) {
            Gtk::Widget* obj (j->get_widget ());
            TRACE9 ("AutoContainer::on_size_allocate (GtkAllocation*) - Storing " << obj);
            Check3 (obj);
            obj->reference ();
            widgets.push_back (obj);
         }
      }

      Gtk::Box::BoxList& children (view.children ());
      children.erase (children.begin (), children.end ());
      Check3 (view.children ().empty ());
      addLine ();
      Check3 (view.children ().size () == 1);

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
/// \returns Gtk::HBox*: The new created line
//-----------------------------------------------------------------------------
Gtk::HBox* AutoContainer::addLine () {
   TRACE9 ("AutoContainer::addLine ()");

   Gtk::HBox* line (new Gtk::HBox ());
   line->signal_size_allocate ().connect
       (bind (slot (*this, &AutoContainer::line_size_allocate), line));
   line->show ();
   view.pack_start (*manage (line), Gtk::PACK_SHRINK, 5);
   return line;
}

//----------------------------------------------------------------------------
/// Removes the passed widget from the container
/// \param widget: Widget to remove 
//----------------------------------------------------------------------------
void AutoContainer::remove (Gtk::Widget& widget) {
   TRACE4 ("AutoContainer::remove (Gtk::Widget&)");

   for (Gtk::Box::BoxList::const_iterator i (view.children ().begin ());
        i != view.children ().end (); ++i) {
      Gtk::Box_Helpers::Child& line_ (*i);
      Gtk::HBox& line (*dynamic_cast<Gtk::HBox*> (line_.get_widget ()));

      if (&line == &widget) {
         view.remove (widget);
         return;
      }
      else
         for (Gtk::Box::BoxList::const_iterator j (line.children ().begin ());
              j != line.children ().end (); ++j)
            if (j->get_widget () == &widget) {
               line.remove (widget);
               return;
            }
   }
   Check2 (0);
}
