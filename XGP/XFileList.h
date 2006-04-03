#ifndef XFILELIST_H
#define XFILELIST_H

//$Id: XFileList.h,v 1.33 2006/04/03 20:32:26 markus Rel $

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


#include <string>

#include <gdkmm/pixbuf.h>

#include <gtkmm/treeview.h>

#include <ygp-cfg.h>


// Forward declarations
namespace Gtk {
   class Menu;
}
namespace YGP {
   struct File;
}
class XFileList;


namespace XGP {

/**Class representing the (minimal) columns in the (file) view.
*/
class FileColumns : public Gtk::TreeModel::ColumnRecord {
 public:
   Gtk::TreeModelColumn <Glib::RefPtr<Gdk::Pixbuf> > icon; ///< Icon for the file
   Gtk::TreeModelColumn <std::string>                name; ///< Name of the file

   FileColumns () { add (icon); add (name); }
};


/**Class for a (columned) list which holds files represented by an icon
   and textual information.

   The model to display should depend on the column-records specified above.
*/
class XFileList : public Gtk::TreeView {
 public:
   /// Constructor of the XFileList widget
   XFileList () : pMenuPopAction (NULL) { init (); }
   /// Constructor of the XFileList widget; passing a model to display
   XFileList (const Glib::RefPtr<Gtk::TreeModel>& model) : Gtk::TreeView (model)
      , pMenuPopAction (NULL) { init (); }
   virtual ~XFileList ();

   static Glib::RefPtr<Gdk::Pixbuf> getIcon4File (const YGP::File& file);

   virtual std::string getFilename (const Gtk::TreeIter& line) const;
   virtual void setFilename (Gtk::TreeIter& line, const std::string& file);

 protected:
   virtual bool on_event (GdkEvent* event);

   void startInTerm (const char* file, Gtk::TreeIter line);
   void startProgram (const char* file, Gtk::TreeIter line);
   void executeProgram (const char* file, Gtk::TreeIter line);

   bool execProgram (const char* file, const char* const args[], bool sync);

   /// Method to add menus to the popup menu activated with the right mouse
   /// button (button 3)
   virtual void addMenus (Gtk::Menu& menu, const Gtk::TreeIter& line);

   void move (Gtk::TreeIter line);
   void remove (Gtk::TreeIter line);

   /// Popup menu after pressing button 3
   Gtk::Menu* pMenuPopAction;

 private:
   void init ();
   static void loadIcons (const char* path, const char* files, unsigned int namePrefix);
};

}

#endif
