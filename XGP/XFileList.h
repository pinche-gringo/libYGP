#ifndef XFILELIST_H
#define XFILELIST_H

//$Id: XFileList.h,v 1.19 2003/06/02 01:32:59 markus Rel $

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


// Forward declarations
struct File;
namespace Gtk {
   class Menu;
   class ListStore;
   class TreeStore;
}
class XFileList;


// Class representing the (minimal) columns in the view
class FileColumns : public Gtk::TreeModel::ColumnRecord {
 public:
   Gtk::TreeModelColumn <Glib::RefPtr<Gdk::Pixbuf> > icon;
   Gtk::TreeModelColumn <std::string>                name;

   FileColumns () { add (icon); add (name); }
};


// Abstract baseclass for XFileStore
class IFileStore {
 public:
   virtual ~IFileStore ();

   static void loadIcons (const char* path, const char* files,
                          unsigned int namePrefix = 0);

   Gtk::TreeModel::iterator setIcon (Gtk::TreeModel::iterator row, const File& pFile);

   const FileColumns& getColumns () const { return cols; }

   virtual std::string getFilename (unsigned int line) = 0;
   virtual void setFilename (unsigned int line, const std::string& file) = 0;

 protected:
   IFileStore (const FileColumns& columns);

   const FileColumns& cols;

 private:
   // Prohibited manager-functions
   IFileStore ();
   IFileStore& operator= (const IFileStore&);
};


// Class for a (columned) list which holds files represented by an icon
// and textual information.
//
// The icons of the files are loaded by default from all Icon_*.xpm-files in
// the package-data-directory (/usr/local/share/General by default);
// user-specified ones can be added to or overridden from this list.
//
// Note: Only the name is used to determine the icon; not the path!
//
// Template, to be used with any Gtk::TreeModel
template <class Parent>
class XFileStore : public Parent, public IFileStore {
   friend class XFileList;

 public:
   virtual ~XFileStore () { }

   // Insert data
   Gtk::TreeModel::iterator append (const File* file) {
      return file ? setIcon (Parent::append (), *file) : Parent::append (); }
   Gtk::TreeModel::iterator prepend (const File* file) {
      return file ? setIcon (Parent::prepend (), *file) : Parent::prepend (); }
   Gtk::TreeModel::iterator insert (const File* file, Gtk::TreeModel::iterator row) {
      return file ? setIcon (Parent::insert (row), *file) : Parent::insert (row); }

   static Glib::RefPtr<XFileStore<Parent> >
      create (const FileColumns& columns) {
      return Glib::RefPtr<XFileStore<Parent> > (new XFileStore<Parent> (columns));
   }

   virtual std::string getFilename (unsigned int line) {
      return children ()[line][cols.name]; }
   virtual void setFilename (unsigned int line, const std::string& file) {
      children ()[line][cols.name] = file; }

 protected:
   XFileStore (const FileColumns& columns) : Parent (columns)
      , IFileStore (columns) { }

 private:
   // Prohibited manager-functions
   XFileStore ();
   XFileStore& operator= (const XFileStore&);

   const IFileStore* getBaseAddress () const {
      return static_cast<const IFileStore*> (this); }
};

typedef XFileStore<Gtk::ListStore> XFileListStore;
typedef XFileStore<Gtk::TreeStore> XFileTreeStore;



// Class for a (columned) list which holds files represented by an icon
// and textual information.
//
// This class is intended to show data of the XFileStore template above.
class XFileList : public Gtk::TreeView {
 public:
   XFileList () : Gtk::TreeView (), pMenuPopAction (NULL) { }
   template <class T>
      XFileList (const Glib::RefPtr<XFileStore<T> >& model) : Gtk::TreeView ()
      , pMenuPopAction (NULL) { set_model (model); }
   virtual ~XFileList ();

   template <class T> void set_model (const Glib::RefPtr<XFileStore<T> >& model) {
      Gtk::TreeView::Column* pColumn = Gtk::manage (new Gtk::TreeView::Column (_("File")));

      pColumn->pack_start (model->getColumns ().icon, false);
      pColumn->pack_start (model->getColumns ().name);

      append_column (*pColumn);
      fileModel = const_cast<IFileStore*> (model->getBaseAddress ());
      Gtk::TreeView::set_model (model); }

   virtual std::string getFilename (unsigned int line) const;
   virtual void setFilename (unsigned int line, const std::string& file);

 protected:
   virtual bool on_event (GdkEvent* event);

   void startInTerm (const char* file, unsigned int line);
   void startProgram (const char* file, unsigned int line);
   void executeProgram (const char* file, unsigned int line);

   bool execProgram (const char* file, const char* const args[], bool sync);

   virtual void addMenus (Gtk::Menu& menu, unsigned int line) { };

   void move (unsigned int line);
   void remove (unsigned int line);

   Gtk::Menu* pMenuPopAction;

 private:
   IFileStore* fileModel;
};

#endif
