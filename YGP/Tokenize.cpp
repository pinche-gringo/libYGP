//$Id: Tokenize.cpp,v 1.14 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Tokenize
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.14 $
//AUTHOR      : Markus Schwab
//CREATED     : 3.7.1999
//COPYRIGHT   : Copyright (C) 1999 - 2004, 2008

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


#include "YGP/Check.h"
#include "YGP/Tokenize.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Returns a sub-string consisting of the string from the end of the last
/// separation (skipping of all characters equal to split) or the beginning of
/// the string for the first search to the next occurence of split (or the end
/// of the string). The separating characters are not included.
/// \param split Character separating the parts
/// \returns std::string Next node (empty string at end)
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
/// \returns int Status; 0: OK
//-----------------------------------------------------------------------------
int Tokenize::checkIntegrity () const {
   Check1 (actPos < _string.length ());
   Check1 ((actPos + len) < _string.length ());
   return 0;
}

}

