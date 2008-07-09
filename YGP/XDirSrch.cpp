//$Id: XDirSrch.cpp,v 1.14 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : XDirectorySearch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.14 $
//AUTHOR      : Markus Schwab
//CREATED     : 17.10.2002
//COPYRIGHT   : Copyright (C) 2002 - 2004, 2007, 2008

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

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/FileRExp.h"
#include "YGP/PathSrch.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Checks the validity of the filename
/// \param files Nodes of files to in/exclude
/// \param pFile Pointer to filename
/// \returns bool True: Name valid
/// \pre pFile is ASCIIZ-string
/// \remarks If the list starts with an x-node, a leading i*-node is assumed
//-----------------------------------------------------------------------------
bool _XDSfileIsValid (const std::string& files, const char* pFile) {
   TRACE9 ("_XDSfileIsValid (const char*) const - " << pFile << " in " << files);
   Check1 (pFile);

   if (files.empty ())
      return true;

   PathSearch  list (files);
   std::string node;

   FileRegularExpr regexp (NULL);
   bool include (false);

   // Test every file in list
   while ((node = list.getNextNode ()).size ()) {
      TRACE8 ("_XDSfileIsValid (const string&, const char*) const - Node: " << node);
      Check3 ((node[0] == 'i') || (node[0] == 'x'));

      include = node[0] == 'i';
      node.replace (0, 1, 0, '\0');

      regexp = node.c_str ();
      if (regexp.matches (pFile))                      // Test if file matches
          return include;
   } // end-while

   return !include;          // 'x'-nodes starts list -> Imply leading i*-node
}

//-----------------------------------------------------------------------------
/// Appends a (list of) node to a list. Every subnode in the node is prefixed
/// with the prefix-character
/// \param list List to change
/// \param prefix Leading character
/// \param node Node to check
/// \remarks The node is added to the beginning of the list
//-----------------------------------------------------------------------------
void _XDSaddNode (std::string& list, char prefix, const std::string& node) {
   TRACE9 ("_XDSaddNode (const char*) const - " << ((prefix == 'i') ? '+' : '-')
           << node);
   Check1 ((prefix == 'i') || (prefix == 'x'));

   PathSearch  l (node);
   std::string temp;

   while ((temp = l.getNextNode ()).size ())
      list = prefix + temp + std::string (1, PathSearch::PATHSEPARATOR) + list;
}

}
