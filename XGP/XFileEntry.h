#ifndef XFILEENTRY_H
#define XFILEENTRY_H

//$Id: XFileEntry.h,v 1.2 2000/02/02 20:57:55 Markus Exp $

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

#include <gtk--/entry.h>

#include "DirSrch.h"


// Class which enhances an entry-field with completion of the input to filenames
class XFileEntry : public Gtk_Entry {
 public:
   XFileEntry (int fileAttrs = DirectorySearch::FILE_NORMAL) : Gtk_Entry ()
      , attrs (fileAttrs) { }
   XFileEntry (guint max, int fileAttrs = DirectorySearch::FILE_NORMAL)
      : Gtk_Entry (max), attrs (fileAttrs) { }
   XFileEntry (GtkEntry *castitem, int fileAttrs = DirectorySearch::FILE_NORMAL)
      : Gtk_Entry (castitem), attrs (fileAttrs) { }
   ~XFileEntry () { }

 protected:
   virtual gint key_press_event_impl (GdkEventKey* ev);

 private:
   int attrs;
};

#endif
