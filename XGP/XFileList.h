#ifndef XFILELIST_H
#define XFILELIST_H

//$Id: XFileList.h,v 1.4 2000/02/24 22:16:36 Markus Exp $

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
#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
namespace Gtk {
   class Pixmap;
}

using namespace Gtk;

#else 
typedef Gtk_Pixmap      Pixmap;
typedef Gtk_CList       CList;
#endif

class dirEntry;

typedef SmartPtr<Pixmap> PPixmap;


// Class for a (columned) list which holds files represented by an icon
// and textual information.
// The icons of the files are loaded by default from all Icon.*-files in the
// package-data-directory; user-specified can be added to or overridden from
// this list.
class XFileList : public CList {
 public:
   XFileList (int columns, const gchar *titles[] = 0)
     : CList (columns, titles) { }
   XFileList (GtkCList *castitem) : CList (castitem) { }
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

   static PPixmap iconDir;
   static PPixmap iconDef;
   static PPixmap iconExe;

   map<string, Pixmap*> icons;
};

#endif
