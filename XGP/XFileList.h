#ifndef XFILELIST_H
#define XFILELIST_H

//$Id: XFileList.h,v 1.26 2003/12/09 04:39:13 markus Rel $

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

#if !defined (HAVE_GETTEXT) || !defined (ENABLE_NLS)
#  define dgettext(pkg, text) (text)
#endif


// Forward declarations
namespace Gtk {
   class Menu;
   class ListStore;
   class TreeStore;
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


/**Abstract baseclass for XFileStore.

   This class provides the model for the XFileList widget; holding <a
   href="../YGP/structFile.html">File</a> objects and providing access methods
   to that object.
 */
class IFileStore {
 public:
   virtual ~IFileStore ();

   static void loadIcons (const char* path, const char* files,
                          unsigned int namePrefix = 0);

   Gtk::TreeModel::iterator setIcon (Gtk::TreeModel::iterator row,
                                     const YGP::File& pFile);

   /// Returns the columns of the model.
   const FileColumns& getColumns () const { return cols; }

   /// Gets the name of the file for the specified line in the model.
   virtual std::string getFilename (unsigned int line) = 0;
   /// Sets the file for the specified line in the model.
   virtual void setFilename (unsigned int line, const std::string& file) = 0;

 protected:
   IFileStore (const FileColumns& columns);

   /// Columns of the file listbox
   const FileColumns& cols;

 private:
   // Prohibited manager-functions
   IFileStore ();
   IFileStore& operator= (const IFileStore&);
};


/**Class for a (columned) list which holds files represented by an icon
   and textual information.

   The icons of the files are loaded by default from all
   <tt>Icon_*.xpm</tt>-files in the package-data-directory
   (<tt>&lt;prefix&gt;/share/YGP</tt> by default); user-specified ones can be
   added to or overridden from this list.

   \note Only the file name is used to determine the icon; not the whole path!

   Template, to be used with any Gtk::TreeModel.
*/
template <class Parent>
class XFileStore : public Parent, public IFileStore {
   friend class XFileList;

 public:
   /// Destructor
   virtual ~XFileStore () { }

   /// \name Inserting data
   //@{
   /// Appends a line showing the passed file to the list.
   /// \param file: Pointer to file to append (or NULL)
   Gtk::TreeModel::iterator append (const YGP::File* file) {
      return file ? setIcon (Parent::append (), *file) : Parent::append (); }
   /// Prepends a line showing the passed file to the list.
   /// \param file: Pointer to file to prepend (or NULL)
   Gtk::TreeModel::iterator prepend (const YGP::File* file) {
      return file ? setIcon (Parent::prepend (), *file) : Parent::prepend (); }
   /// Inserts a line showing the passed file to the list.
   /// \param file: Pointer to file to append (or NULL)
   /// \param row: Row before which the line should be inserted.
   Gtk::TreeModel::iterator insert (const YGP::File* file, Gtk::TreeModel::iterator row) {
      return file ? setIcon (Parent::insert (row), *file) : Parent::insert (row); }
   //@}

   /// Creates the model (basing on the specified columns)
   static Glib::RefPtr<XFileStore<Parent> >
      create (const FileColumns& columns) {
      return Glib::RefPtr<XFileStore<Parent> > (new XFileStore<Parent> (columns));
   }

   /// Gets the name of the file for the specified line in the model.
   virtual std::string getFilename (unsigned int line) {
      return children ()[line][cols.name]; }
   /// Sets the file for the specified line in the model.
   virtual void setFilename (unsigned int line, const std::string& file) {
      children ()[line][cols.name] = Glib::locale_to_utf8 (file); }

   /// Returns the adress of the IFileStore-parent
   const IFileStore* getBaseAddress () const {
      return static_cast<const IFileStore*> (this); }

 protected:
   /// Constructor; from the specified columns
   XFileStore (const FileColumns& columns) : Parent (columns)
      , IFileStore (columns) { }

 private:
   // Prohibited manager-functions
   XFileStore ();
   XFileStore& operator= (const XFileStore&);
};

/// Type definition of an XFileStore for a List
typedef XFileStore<Gtk::ListStore> XFileListStore;
/// Type definition of an XFileStore for a Tree
typedef XFileStore<Gtk::TreeStore> XFileTreeStore;



/**Class for a (columned) list which holds files represented by an icon
   and textual information.

   This class is intended to show data of the XFileStore template above.
*/
class XFileList : public Gtk::TreeView {
 public:
   /// Constructor of the XFileList widget
   XFileList () : Gtk::TreeView (), pMenuPopAction (NULL) { }
   /// Constructor of the XFileList widget; passing a model to display
   template <class T>
      XFileList (const Glib::RefPtr<XGP::XFileStore<T> >& model) : Gtk::TreeView ()
      , pMenuPopAction (NULL) { set_model (model); }
   virtual ~XFileList ();

   /// Sets the model to display. Note that both the icon and the name of the
   /// file are displayed in one column
   template <class T> void set_model (const Glib::RefPtr<XGP::XFileStore<T> >& model) {
      Gtk::TreeView::Column* pColumn = Gtk::manage (new Gtk::TreeView::Column
                                                    (Glib::locale_to_utf8
                                                     (dgettext (LIBYGP_NAME, "File"))));

      pColumn->pack_start (model->getColumns ().icon, false);
      pColumn->pack_start (model->getColumns ().name);

      append_column (*pColumn);
      fileModel = const_cast<XGP::IFileStore*> (model->getBaseAddress ());
      Gtk::TreeView::set_model (model); }

   virtual std::string getFilename (unsigned int line) const;
   virtual void setFilename (unsigned int line, const std::string& file);

 protected:
   virtual bool on_event (GdkEvent* event);

   void startInTerm (const char* file, unsigned int line);
   void startProgram (const char* file, unsigned int line);
   void executeProgram (const char* file, unsigned int line);

   bool execProgram (const char* file, const char* const args[], bool sync);

   /// Method to add menus to the popup menu activated with the right mouse
   /// button (button 3)
   virtual void addMenus (Gtk::Menu& menu, unsigned int line) { };

   void move (unsigned int line);
   void remove (unsigned int line);

   /// Popup menu after pressing button 3
   Gtk::Menu* pMenuPopAction;

 private:
   IFileStore* fileModel;
};

}

#endif
