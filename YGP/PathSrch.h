#ifndef PATHSRC_H
#define PATHSRC_H

//$Id: PathSrch.h,v 1.6 2000/04/06 20:48:43 Markus Rel $

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


#include "Tokenize.h"


// Class to split a string of path-nodes into its sub-nodes
class PathSearch : public Tokenize {
 public:
   // Manager-functions
   PathSearch (const std::string& path) : Tokenize (path) { }
   virtual ~PathSearch () { }

   PathSearch& operator= (const std::string& path) {
      return (PathSearch&)Tokenize::operator= (path); }

   // Access to sub-nodes
   std::string getNextNode () { return Tokenize::getNextNode (getSplitChar ()); }

   static char getSplitChar () {
#ifdef UNIX
      return ':';
#else
      return ';';
#endif
   }

 private:
   PathSearch ();
   PathSearch (const PathSearch& other);

   PathSearch& operator= (const PathSearch& other);
};

#endif
