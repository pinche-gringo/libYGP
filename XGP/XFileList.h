#ifndef XFILELIST_H
#define XFILELIST_H

//$Id: XFileList.h,v 1.16 2003/02/09 22:55:15 markus Exp $

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

#include <map.h>
#include <vector.h>

#include <gtk--/clist.h>
#include <gdk--/pixmap.h>

#include "SmartPtr.h"


// Forward declarations
struct File;
namespace Gtk {
   class Menu;
}


// Class for a (columned) list which holds files represented by an icon
// and textual information.
//
// The icons of the files are loaded by default from all Icon_*.xpm-files in
// the package-data-directory (/usr/local/share/General by default);
// user-specified ones can be added to or overridden from this list.
//
// Note: Only the name is used to determine the icon; not the path!
class XFileList : public Gtk::CList {
 public:
   XFileList (int columns, const gchar *titles[] = 0)
      : CList (columns, titles), pMenuPopAction (NULL) { }
   XFileList (GtkCList *castitem) : CList (castitem), pMenuPopAction (NULL) { }
   virtual ~XFileList ();

   unsigned int loadIcons (const char* path, const char* files,
                           unsigned int namePrefix = 0);

   // Insert data
   gint append (const File* file, const Gtk::SArray& text);
   gint prepend (const File* file, const Gtk::SArray& text);
   gint insert (const File* file, gint row, const Gtk::SArray& text);

   void setIcon (int row, const File& pFile);

   virtual string getFilename (unsigned int line) const;
   virtual void setFilename (unsigned int line, const string& file);

 protected:
   virtual void realize_impl ();
   virtual gint listSelected (GdkEvent* ev);

   void move (unsigned int line);
   void remove (unsigned int line);

   void startInTerm (const char* file, unsigned int line);
   void startProgram (const char* file, unsigned int line);
   void executeProgram (const char* file, unsigned int line);

   bool execProgram (const char* file, const char* const args[], bool sync);

   virtual void addMenus (Gtk::Menu& menu, unsigned int line) { };

 private:
   // Prohibited manager-functions
   XFileList ();
   XFileList& operator= (const XFileList&);

   static const char* iconDirectory[];
   static const char* iconDefault[];
   static const char* iconExecuteable[];

   static Gdk_Pixmap iconDir;
   static Gdk_Pixmap iconDef;
   static Gdk_Pixmap iconExe;

   map<string, Gdk_Pixmap> icons;

   Gtk::Menu* pMenuPopAction;
};

#endif
