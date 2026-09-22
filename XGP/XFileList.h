#ifndef XGP_XFILELIST_H
#define XGP_XFILELIST_H

//$Id: XFileList.h,v 1.36 2008/05/18 13:21:27 markus Rel $

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


#include <string>

#include <gdkmm/pixbuf.h>

#include <gtkmm/treeview.h>

#include <ygp-cfg.h>


// Forward declarations
namespace Gtk {
   class PopoverMenu;
}
namespace Gio {
   class Menu;
   class SimpleActionGroup;
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

   FileColumns () : Gtk::TreeModel::ColumnRecord (), icon (), name () { add (icon); add (name); }
};


/**Class for a (columned) list which holds files represented by an icon
   and textual information.

   The model to display should depend on the column-records specified above.
*/
class XFileList : public Gtk::TreeView {
 public:
   /// Constructor of the XFileList widget
   XFileList () : pMenuPopAction (nullptr) { init (); }
   /// Constructor of the XFileList widget; passing a model to display
   XFileList (const Glib::RefPtr<Gtk::TreeModel>& model) : Gtk::TreeView (model)
      , pMenuPopAction (nullptr) { init (); }
   ~XFileList () override;

   static Glib::RefPtr<Gdk::Pixbuf> getIcon4File (const YGP::File& file);

   virtual std::string getFilename (const Gtk::TreeModel::iterator& line) const;
   virtual void setFilename (Gtk::TreeModel::iterator& line, const std::string& file);

 protected:
   void onRightClick (int n_press, double x, double y);

   void startInTerm (const char* file, Gtk::TreeModel::iterator line);
   void startProgram (const char* file, Gtk::TreeModel::iterator line);
   void executeProgram (const char* file, Gtk::TreeModel::iterator line);

   bool execProgram (const char* file, const char* const args[], bool sync);

   /// Method to add menus to the popup menu activated with the right mouse
   /// button (button 3)
   /// \param menu Menu to append entries to
   /// \param actions Action group backing \a menu; add the entries' actions here
   /// \param line Row the popup menu was opened for
   virtual void addMenus (const Glib::RefPtr<Gio::Menu>& menu,
			  const Glib::RefPtr<Gio::SimpleActionGroup>& actions,
			  const Gtk::TreeModel::iterator& line);

   void move (Gtk::TreeModel::iterator line);
   void remove (Gtk::TreeModel::iterator line);

   /// Popup menu after pressing button 3
   Gtk::PopoverMenu* pMenuPopAction;

 private:
   XFileList (const XFileList&) = delete;
   XFileList& operator= (const XFileList&) = delete;

   void init ();
   static void loadIcons (const char* path, const char* files, unsigned int namePrefix);
};

}

#endif
