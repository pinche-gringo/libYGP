//$Id: XFileEntry.cpp,v 1.3 2001/10/03 16:26:29 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileEntry
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 8.9.1999
//COPYRIGHT   : Anticopyright (A) 1999

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


#define DEBUG 0
#include "Trace_.h"

#include "XFileEntry.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Handling of pressed key; Try to enhance input to complete file
//Parameters: ev: Event, containing input-information
/*--------------------------------------------------------------------------*/
gint XFileEntry::key_press_event_impl (GdkEventKey* ev) {
   assert (ev);
   gint rc (Entry::key_press_event_impl (ev));

   TRACE5 ("XFileEntry::key_press_event_impl: Input: " << ev->keyval);

   if ((ev->state > 1)                 // I tried to check only non-ctrl-chars
       || (ev->keyval > 0xf000))         // (I've checked keysymdef.h & hope I
      return rc;                       // got the japanese/korean-stuff right)

   string input (get_text ());
   if (input.empty ())
      return rc;

   input += '*';

   TRACE5 ("XFileEntry::key_press_event_impl: Text: " << input);

   const File* result;
   DirectorySearch ds (input);
   if (result = ds.find (attrs)) {              // If input matches attributes
      int len (get_text_length ());                  // Complete name and mark

      input = result->path (); input += result->name ();   // part after input
      TRACE7 ("XFileEntry::key_press_event_impl: Changed input: " << input);
      if (result->isDirectory ())
         input += '/';
      set_text (input);
     
      assert (len);
      set_position (len);
      select_region (len, get_text_length ());
      return true;
   }
   return rc;
}
