//PROJECT     : libXGP
//SUBSYSTEM   : XFileEntry
//REFERENCES  :
//TODO        :
//BUGS        :
//AUTHOR      : Markus Schwab
//CREATED     : 8.9.1999
//COPYRIGHT   : Copyright (C) 1999 - 2005, 2007, 2008, 2026

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include <YGP/Trace.h>
#include <YGP/File.h>

#include "XGP/XFileEntry.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Handling of the pressed key; Tries to complete the input to the first file
/// matching the input.
/// \param keyval Symbol of the pressed key
/// \param keycode Hardware keycode of the pressed key
/// \param state Modifier keys held while pressing the key
/// \remarks GTK4 no longer provides on_key_press_event()/GdkEventKey; this
///     relies on Gtk::EventControllerKey instead. As that controller sees
///     the keypress independently of (and not necessarily after) the entry's
///     own built-in text-input handling, get_text() below may not yet
///     reflect the just-pressed key the way it reliably did under GTK3.
//-----------------------------------------------------------------------------
bool XFileEntry::onKeyPressed (guint keyval, guint /*keycode*/, Gdk::ModifierType state) {
   bool rc (false);

   TRACE5 ("XFileEntry::onKeyPressed: Input: " << keyval);

   if (((static_cast<unsigned int> (state) & 0x7) > 1) // I tried to check only non-ctrl-chars
       || (keyval > 0xf000))              // (I've checked keysymdef.h & hope I
      return rc;                          // got the japanese/korean-stuff right)

   if (get_text_length ()) {
      std::string input (get_text ());
      TRACE5 ("XFileEntry::onKeyPressed: Text: " << input);

      input += '*';
      const YGP::File* result;
      YGP::DirectorySearch ds (input);
      if ((result = ds.find (attrs))) {         // If input matches attributes
         int len (get_text_length ());               // Complete name and mark

         input = result->path (); input += result->name ();// part after input
         TRACE7 ("XFileEntry::onKeyPressed: Changed input: " << input);
         if (result->isDirectory ())
            input += YGP::File::DIRSEPARATOR;
         set_text (input);

         Check3 (len);
         set_position (len);
         select_region (len, get_text_length ());
         return true;
      }
   }
   return rc;
}

}
