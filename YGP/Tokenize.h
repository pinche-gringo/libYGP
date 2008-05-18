#ifndef YGP_TOKENIZE_H
#define YGP_TOKENIZE_H

//$Id: Tokenize.h,v 1.19 2008/05/18 13:21:27 markus Rel $

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


#include <string>

namespace YGP {

/**Class to split a string into sub-nodes

   This sub-strings are extracted from the beginning of the string (or the
   character behind the last found sub-string) to a passed separator (or to the
   end of string). The separator-character is not included in the result.

   If no further sub-string is available an empty string is returned. That also
   means that two separators behind each other are silently skipped and the
   next following sub-string (if any) is returned.
*/
class Tokenize {
 public:
   /// Constructor; specify the string to split
   Tokenize (const std::string& string) : actPos (0), len (0), _string (string) { }
   /// Copy constructor; the same string as the object is holding will be split,
   /// although the tokenization starts from the start
   Tokenize (const Tokenize& other) : actPos (0), len (0), _string (other._string) { }
   /// Destructor
   ~Tokenize () { }

   /// Assignment operator; the same string as the object is holding will be
   /// split, although the tokenization starts from the start
   Tokenize& operator= (const Tokenize& other) {
      _string = other._string;
      reset ();
      return *this; }
   /// Assignment operator; assigns a new string to tokenize.
   Tokenize& operator= (const std::string& string) {
      _string = string;
      reset ();
      return *this; }

   /// \name Accessing the splitted string
   //@{
   /// Casting to a std::string
   operator const std::string& () const { return _string; }
   /// Accessing the whole handled string
   const std::string& data () const { return _string; }
   /// Accessing the remaining string
   std::string remaining () const { return _string.substr (getEndPosition ()); }
   //@}

   /// \name Accessing the sub-parts
   //@{
   /// Returns the current sub-part of the handled string or the whole string,
   /// if splitting has not started yet.
   std::string getActNode () const { return _string.substr ((int)actPos, len - 1); }
   std::string getNextNode (const char split);
   unsigned int getEndPosition () const { return actPos + len; }

   /// Resets the internal data; so next separting will start at the beginning
   void reset () { actPos = len = 0; }

 protected:
   int checkIntegrity () const;
   unsigned int actPos;           ///< Starting position of the actual sub-node
   unsigned int len;                         ///< Length of the actual sub-node
   std::string  _string;                                 ///< String to process
};

}

#endif
