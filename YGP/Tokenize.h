#ifndef TOKENIZE_H
#define TOKENIZE_H

//$Id: Tokenize.h,v 1.10 2002/12/15 22:22:01 markus Rel $

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
//
// This sub-strings are extracted from the beginning of the string (or the
// character behind the last found sub-string) to a passed seperator (or to the
// end of string). The seperator-character is not included in the result.
//
// If no further sub-string is available an empty string is returned. That also
// means that two seperators behind each other are silently skipped and the
// next following sub-string (if any) is returned.
class Tokenize {
 public:
   //Section manager-functions
   Tokenize (const std::string& string) : _string (string), actPos (0), len (0) { }
   ~Tokenize () { }

   Tokenize& operator= (const std::string& string) {
      _string = string;
      reset ();
      return *this; }

   //Section access
   operator const std::string& () const { return _string; }
   const std::string& data () const { return _string; }

   //Section access to sub-nodes
   std::string getActNode () const { return _string.substr ((int)actPos, len - 1); }
   std::string getNextNode (const char split);

   void reset () { actPos = len = 0; }

 protected:
   int checkIntegrity () const;
   unsigned int actPos, len;
   std::string  _string;

 private:
   Tokenize (const Tokenize& other);
   Tokenize& operator= (const Tokenize& other);
};

#endif
