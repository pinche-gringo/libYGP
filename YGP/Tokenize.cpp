//$Id: Tokenize.cpp,v 1.9 2003/07/10 21:24:58 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Tokenize
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.9 $
//AUTHOR      : Markus Schwab
//CREATED     : 3.7.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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


#include "Check.h"
#include "Tokenize.h"


//-----------------------------------------------------------------------------
/// Returns a sub-string consisting of the string from the end of the last
/// separation (skipping of all characters equal to split) or the beginning of
/// the string for the first search to the next occurence of split (or the end
/// of the string). The separating characters are not included.
/// \param split: Character separating the parts
/// \returns \c Next node (empty string at end)
/// \remarks Two following separators are ignored; the method always returns
///     some data (if available)
//-----------------------------------------------------------------------------
std::string Tokenize::getNextNode (const char split) {
   if ((actPos + len) >= _string.length ()) {
      actPos = _string.length ();
      len = 0;
      return "";
   }

   Check3 (!checkIntegrity ());

   actPos += len - 1;
   do {
      len = _string.find (split, ++actPos);
      Check3 (len >= actPos);
   } while (len == actPos);
   if (len++ == -1U)
      len = _string.length () + 1;
   len -= actPos;
   return getActNode ();
}

//-----------------------------------------------------------------------------
/// Checks the status of the object
/// \returns \c Status; 0: OK
//-----------------------------------------------------------------------------
int Tokenize::checkIntegrity () const {
   Check1 (actPos < _string.length ());
   Check1 ((actPos + len) < _string.length ());
   return 0;
}

