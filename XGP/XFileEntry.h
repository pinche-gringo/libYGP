#ifndef XFILEENTRY_H
#define XFILEENTRY_H

//$Id: XFileEntry.h,v 1.8 2003/02/03 03:50:33 markus Exp $

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


// Enhances the ordinary entry field with completing the input to the first file
// matching it (if the entered part is a valid file); analogue to modern web
// browsers.
//
// During construction the type of files to complete can be specified. See the
// documentation of IDirectorySearch for details.
class XFileEntry : public Gtk::Entry {
 public:
   XFileEntry (int fileAttrs = DirectorySearch::FILE_NORMAL) : Entry ()
      , attrs (fileAttrs) { }
   XFileEntry (guint max, int fileAttrs = DirectorySearch::FILE_NORMAL)
      : Entry (max), attrs (fileAttrs) { }
   XFileEntry (GtkEntry *castitem, int fileAttrs = DirectorySearch::FILE_NORMAL)
      : Entry (castitem), attrs (fileAttrs) { }
   ~XFileEntry () { }

 protected:
   virtual gint key_press_event_impl (GdkEventKey* ev);

 private:
   int attrs;
};

#endif
