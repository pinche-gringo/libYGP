#ifndef XFILELIST_H
#define XFILELIST_H

//$Id: XFileList.h,v 1.1 1999/11/13 01:18:07 Markus Exp $

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


#include <map.h>
#include <string>

#include <gtk--/clist.h>

#include "SmartPtr.h"


// Forward declarations
class dirEntry;
class Gdk_Pixmap;

definePtr (Gdk_Pixmap);


// Class for a (columned) list which holds files represented by an icon
// and textual information.
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

 private:
   XFileList ();
   XFileList& operator= (const XFileList&);

   void setIcon (const dirEntry* pFile);

   static const char* iconDirectory[];
   static const char* iconDefault[];
   static const char* iconExecuteable[];

   static PGdk_Pixmap iconDir;
   static PGdk_Pixmap iconDef;
   static PGdk_Pixmap iconExe;

   map<string, Gdk_Pixmap*> icons;
};

#endif
