#ifndef XFILEENTRY_H
#define XFILEENTRY_H

//$Id: XFileEntry.h,v 1.12 2003/11/14 20:28:08 markus Rel $

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

#include <gtkmm/entry.h>

#include <YGP/DirSrch.h>


namespace XGP {

/**Enhances the ordinary entry field with completing the input to the first
   file matching it. Of course, only if the entered part is a valid file; a
   behaviour analogue to the web browsers.

   During construction the type of files to complete can be specified. See the
   documentation of <a
   href="../YGP/classIDirectorySearch.html">IDirectorySearch</a> for
   details.
*/
class XFileEntry : public Gtk::Entry {
 public:
   /// Constructor
   /// \param fileAttrs: Attributes the files to display must have (default:
   /// Normal files, no directories)
   XFileEntry (int fileAttrs = YGP::IDirectorySearch::FILE_NORMAL) : Entry ()
      , attrs (fileAttrs) { }
   /// Destructor
   ~XFileEntry () { }

 protected:
   virtual bool on_key_press_event (GdkEventKey* ev);

 private:
   int attrs;
};

}

#endif
