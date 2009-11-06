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

#include <boost/tokenizer.hpp>

#warning This class is deprecated! Please use boost::tokenizer or YGP::Path


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

   \deprecated Please use boost::tokenize or YGP::Path
*/
class PathSearch {
 public:
   /// Constructor; with path to analyse
   /// \param p Path to search
   PathSearch (const std::string& p) : path (p, boost::char_separator<char> (":")),
      i (path.begin ()) { }
   /// Destructor
   ~PathSearch () { }

   /// Assignemnt operator; Specify other path to split
   /// \param newPath: New path to tokenise
   PathSearch& operator= (const std::string& newPath) {
      path.assign (newPath);
      i = path.begin ();
      return *this; }

   /// \name Accessing the sub-parts
   //@{
   /// Returns the actual node of the path
   std::string getActNode () { return (i != path.end ()) ? *i : std::string (); }
   /// Returns the current node of the path "as is".
   std::string getNextNode () { return *i++; }
   /// Returns the current node of the path "expanded" (see expandNode ())
   std::string getNextExpandedNode () {
      return expandNode (getNextNode ()); }
   //@}

   static std::string expandNode (const std::string& input);

   /// Resets the search-objects so that the whole path is searched again
   void reset () { i = path.begin (); }

   /// Character separating nodes in a path. Under UNIX-like operating systems
   /// this is the double-point (:); under DOS/Windoze the semicolon (;)
   static const char PATHSEPARATOR;

 private:
   PathSearch ();
   PathSearch (const PathSearch& other);

   PathSearch& operator= (const PathSearch& other);

   typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

   tokenizer path;
   tokenizer::iterator i;
};

}

#endif
