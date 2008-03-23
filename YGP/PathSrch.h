#ifndef PATHSRC_H
#define PATHSRC_H

//$Id: PathSrch.h,v 1.17 2008/03/23 13:56:12 markus Exp $

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
