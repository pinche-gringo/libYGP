#ifndef FOLDER_H
#define FOLDER_H

//$Id: Folder.h,v 1.4 2003/11/12 23:38:45 markus Exp $

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


#include <vector>

#include <gtkmm/table.h>
#include <gtkmm/scrolledwindow.h>


/**Class to show child widgets, aligned horicontally and vertically.

   The children are realigned, when the folder is resized (analogue to the
   folders displayed by nautilus, konqueror or explorer, ...)
*/
class Folder : public Gtk::ScrolledWindow {
  public:
   Folder ();
   Folder (Gtk::Adjustment& hadjustment, Gtk::Adjustment& vadjustment);
   virtual ~Folder ();

   void add (Gtk::Widget& widget);

 protected:
   void on_size_allocate (GtkAllocation* size);

   /// The view used to display the child widgets
   Gtk::Table view;

 private:
   // Prohibited manager functions
   Folder (const Folder& other);
   const Folder& operator= (const Folder& other);

   void init ();

   unsigned int rows;
   unsigned int cols;
   unsigned int actCol;
   int width;
   std::vector<unsigned int> colWidths;
};

#endif
