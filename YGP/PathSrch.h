#ifndef PATHSRC_H
#define PATHSRC_H

//$Id: PathSrch.h,v 1.2 1999/07/31 03:30:09 Markus Exp $

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

#include "Tokenize.h"


// Class to split a string of path-nodes into its sub-nodes
class PathSearch : public Tokenize {
 public:
   //@Section manager-functions
   PathSearch (const std::string& path) : Tokenize (path) { }
   virtual ~PathSearch () { }

   //@Section access to sub-nodes
   std::string getNextNode () { return Tokenize::getNextNode (SPLIT_CHAR); }

 private:
   static const char SPLIT_CHAR;
};

#endif
