#ifndef XFILELIST_H
#define XFILELIST_H

//$Id: XFileList.h,v 1.10 2002/04/11 07:59:35 markus Rel $

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
using namespace Gtk;

struct File;


// Class for a (columned) list which holds files represented by an icon
// and textual information.
// The icons of the files are loaded by default from all Icon_*.xpm-files in
// the package-data-directory; user-specified ones can be added to or overridden
// from this list.
// The user (or whoever) can easily add his own icons by adding files (named
// according to this rule) by adding xpm-files into that directory.
// E.g: To add a special icon for netscape: Put a(n) (xpm-)file called
// Icon_netscape.xpm into /usr/local/share/General (replace /usr/local with
// the prefix where you installed the library into).
// Note: Only the name is inspected to determine the icon; not the path!
class XFileList : public CList {
 public:
   XFileList (int columns, const gchar *titles[] = 0)
     : CList (columns, titles) { }
   XFileList (GtkCList *castitem) : CList (castitem) { }
   virtual ~XFileList ();

   unsigned int loadIcons (const char* path, const char* files,
                           unsigned int namePrefix = 0);

   // Insert data
   gint append (const File* file, const gchar* text[]);
   gint append (const File* file, const vector<string> text);
   gint prepend (const File* file, const gchar* text[]);
   gint prepend (const File* file, const vector<string> text);
   gint insert_row (const File* file, gint row, const gchar* text[]);
   gint insert_row (const File* file, gint row, const vector<string> &text);

 protected:
   virtual void realize_impl ();

 private:
   // Prohibited manager-functions
   XFileList ();
   XFileList& operator= (const XFileList&);

   void setIcon (int row, const File* pFile);

   static const char* iconDirectory[];
   static const char* iconDefault[];
   static const char* iconExecuteable[];

   static Gdk_Pixmap iconDir;
   static Gdk_Pixmap iconDef;
   static Gdk_Pixmap iconExe;

   map<string, Gdk_Pixmap> icons;
};

#endif
