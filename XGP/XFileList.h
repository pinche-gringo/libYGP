#ifndef XFILELIST_H
#define XFILELIST_H

//$Id: XFileList.h,v 1.3 2000/01/23 23:06:37 Markus Rel $

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

#include "SmartPtr.h"


// Forward declarations
class dirEntry;
class Gtk_Pixmap;

definePtr (Gtk_Pixmap);


// Class for a (columned) list which holds files represented by an icon
// and textual information.
// The icons of the files are loaded by default from all Icon.*-files in the
// package-data-directory; user-specified can be added to or overridden from
// this list.
class XFileList : public Gtk_CList {
 public:
   XFileList (int columns, const gchar *titles[] = 0)
     : Gtk_CList (columns, titles) { }
   XFileList (GtkCList *castitem) : Gtk_CList (castitem) { }
   virtual ~XFileList ();

   unsigned int loadIcons (const char* path, const char* files);

   // Insert data
   gint append (const dirEntry* file, const gchar* text[]);
   gint append (const dirEntry* file, const vector<string> text);
   gint prepend (const dirEntry* file, const gchar* text[]);
   gint prepend (const dirEntry* file, const vector<string> text);
   gint insert_row (const dirEntry* file, gint row, const gchar* text[]);
   gint insert_row (const dirEntry* file, gint row, const vector<string> &text);

 protected:
   virtual void realize_impl ();

 private:
   XFileList ();
   XFileList& operator= (const XFileList&);

   void setIcon (int row, const dirEntry* pFile);

   static const char* iconDirectory[];
   static const char* iconDefault[];
   static const char* iconExecuteable[];

   static PGtk_Pixmap iconDir;
   static PGtk_Pixmap iconDef;
   static PGtk_Pixmap iconExe;

   map<string, Gtk_Pixmap*> icons;
};

#endif
