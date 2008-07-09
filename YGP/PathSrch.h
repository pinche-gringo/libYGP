#ifndef YGP_PATHSRC_H
#define YGP_PATHSRC_H

//$Id: PathSrch.h,v 1.19 2008/05/18 13:21:27 markus Rel $

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


#include <ygp-cfg.h>

#include <YGP/Tokenize.h>


namespace YGP {

/**Class to split a string of path-nodes into its sub-nodes

   This sub-strings are extracted from the beginning of the string (or the
   character behind the last found node) to the next path-seperator (or to the
   end of string). The seperator-character is not included in the result.

   If no further sub-string is available an empty string is returned. That
   also means that two seperators behind each other are silently skipped and
   the next following sub-string (if any) is returned.

   The character to seperate the nodes differ between the operating systems.
   Under UNIX (or UNIX-like systems) it's the double-point (:), in DOS,
   Windoze and OS/2 its the semicolon (;).
*/
class PathSearch : public Tokenize {
 public:
   /// Constructor; with path to analyze
   PathSearch (const std::string& path) : Tokenize (path) { }
   /// Destructor
   ~PathSearch () { }

   /// Assignemnt operator; Specify other path to split
   PathSearch& operator= (const std::string& path) {
      Tokenize::operator= (path);
      return *this; }

   /// \name Accessing the sub-parts
   //@{
   /// Returns the current node of the path "as is".
   std::string getNextNode () { return Tokenize::getNextNode (PATHSEPARATOR); }
   /// Returns the current node of the path "expanded" (see expandNode ())
   std::string getNextExpandedNode () {
      return expandNode (getNextNode ()); }
   //@}

   static std::string expandNode (const std::string& input);

   /// Character separating nodes in a path. Under UNIX-like operating systems
   /// this is the double-point (:); under DOS/Windoze the semicolon (;)
   static const char PATHSEPARATOR;

 private:
   PathSearch ();
   PathSearch (const PathSearch& other);

   PathSearch& operator= (const PathSearch& other);
};

}

#endif
