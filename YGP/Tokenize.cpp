//$Id: Tokenize.cpp,v 1.5 2002/04/09 20:05:09 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Tokenize
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
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


#include <assert.h>

#include "Tokenize.h"


/*--------------------------------------------------------------------------*/
//Purpose     : Retrieves the next (= first, at first call) node of the string.
//Returns     : Next node (empty string at end)
//Requires    : split != '\0' (operates on strings)
/*--------------------------------------------------------------------------*/
std::string Tokenize::getNextNode (const char split) {
   assert (split != '\0');
   if ((actPos + len) >= _string.length ()) {
      actPos =  _string.length ();
      len = 0;
      return "";
   }

   assert (!checkIntegrity ());

   actPos += len - 1;
   do {
      len = _string.find (split, ++actPos);
      assert (len >= actPos);
   } while (len == actPos);
   if (len++ == -1)
      len = _string.length () + 1;
   len -= actPos;
   return getActNode ();
}

/*--------------------------------------------------------------------------*/
//Purpose     : Checks the status of the object
//Returns     : Status; 0: OK
/*--------------------------------------------------------------------------*/
int Tokenize::checkIntegrity () const {
   assert (actPos < _string.length ());
   assert ((actPos + len) < _string.length ());
   return 0;
}

