#ifndef TOKENIZE_H
#define TOKENIZE_H

//$Id: Tokenize.h,v 1.1 1999/07/31 00:15:08 Markus Exp $

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <string>


// Class to split a string into sub-nodes
class Tokenize {
 public:
   //@Section manager-functions
   Tokenize (const std::string& string) : _string (string), actPos (0), len (0) { }
   virtual ~Tokenize () { }

   //@Section access to sub-nodes
   std::string getActNode () { return _string.substr ((int)actPos, (int)len - 1); }
   std::string getNextNode (const char split);

   void reset () { actPos = len = 0; }

 private:
   int checkIntegrity () const;

   std::string  _string;
   unsigned int actPos, len;
};

#endif
