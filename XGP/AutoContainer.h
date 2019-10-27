#ifndef XGP_AUTOCONTAINER_H
#define XGP_AUTOCONTAINER_H

//$Id: AutoContainer.h,v 1.7 2008/05/18 13:21:27 markus Rel $

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


#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>


namespace XGP {

/**Class to show child widgets, arranged horizontally (all widgets in a single
   line have the same height; if the width of all children exceeds the width
   of the widget itself, the line (and likely the following ones) is/are
   re-aligned.

   The children are also rearranged, when the widget is resized (similar to the
   folders displayed by nautilus, konqueror or explorer, ...)
*/
class AutoContainer : public Gtk::ScrolledWindow {
  public:
   AutoContainer ();
   AutoContainer (const Glib::RefPtr<Gtk::Adjustment>& hadjustment,
		  const Glib::RefPtr<Gtk::Adjustment>& vadjustment);
   virtual ~AutoContainer ();

   void insert (Gtk::Widget& widget, unsigned int pos);
   virtual void add (Gtk::Widget& widget);
   void remove (Gtk::Widget& widget);

 protected:
   void on_size_allocate (Gtk::Allocation& size);
   void line_size_allocate (Gtk::Allocation& size, Gtk::HBox* line);

   /// The vertical box used to display the other widgets
   Gtk::VBox view;

 private:
   // Prohibited manager functions
   AutoContainer (const AutoContainer& other);
   const AutoContainer& operator= (const AutoContainer& other);

   void init ();
   Gtk::HBox* addLine ();

   bool checkLast (Gtk::HBox* line, Gtk::Widget* child);

   int width;
};

}

#endif
