#ifndef TOKENIZE_H
#define TOKENIZE_H

//$Id: Tokenize.h,v 1.3 1999/09/26 01:54:46 Markus Rel $

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


// Class to split a string into sub-nodes
class Tokenize {
 public:
   //Section manager-functions
   Tokenize (const std::string& string) : _string (string), actPos (0), len (0) { }
   virtual ~Tokenize () { }

   const Tokenize& operator= (std::string& string) {
      _string = string;
      reset ();
      return *this; }

   //Section access
   operator const std::string& () const { return _string; }

   //Section access to sub-nodes
   std::string getActNode () { return _string.substr ((int)actPos, (int)len - 1); }
   std::string getNextNode (const char split);

   void reset () { actPos = len = 0; }

 private:
   int checkIntegrity () const;

   std::string  _string;
   unsigned int actPos, len;
};

#endif
