//$Id: XDirSrch.cpp,v 1.4 2003/02/03 03:54:30 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : XDirectorySearch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
//AUTHOR      : Markus Schwab
//CREATED     : 17.10.2002
//COPYRIGHT   : Anticopyright (A) 2002

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

#include "Check.h"
#include "Trace_.h"
#include "FileRExp.h"
#include "PathSrch.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Checks the validity of the filename
//Parameters: files: Nodes of files to in/exclude
//            pFile: Pointer to filename
//Returns   : bool: True: Name valid
//Requires  : pFile is ASCIIZ-string
//Remarks   : If the list starts with an x-node, a leading i*-node is assumed
/*--------------------------------------------------------------------------*/
bool _XDSfileIsValid (const std::string& files, const char* pFile) {
   TRACE9 ("_fileIsValid (const char*) const - " << pFile);
   Check1 (pFile);

   if (files.empty ())
      return true;

   PathSearch  list (files);
   std::string node;

   FileRegularExpr regexp (NULL);
   bool include (false);

   // Test every file in list
   while (!((node = list.getNextNode ()).empty ())) {
      TRACE8 ("_fileIsValid (const string&, const char*) const - Node: " << node);
      Check3 ((node[0] == 'i') || (node[0] == 'x'));

      include = node[0] == 'i';
      node.replace (0, 1, 0, '\0');

      regexp = node.c_str ();
      if (regexp.matches (pFile))                      // Test if file matches
          return include;
   } // end-while

   return !include;          // 'x'-nodes starts list -> Imply leading i*-node
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends a (list of) node to a list. Every subnode in the node is
//            prefixed with the prefix-character
//Parameters: list: List to change
//            prefix: Leading character
//            node: Node to check
//Remarks   : The node is added to the beginning of the list
/*--------------------------------------------------------------------------*/
void _XDSaddNode (std::string& list, char prefix, const std::string& node) {
   PathSearch  l (node);
   std::string temp;

   while (!(temp = l.getNextNode ()).empty ())
      list = prefix + temp + (std::string)(PathSearch::PATHSEPARATOR + list);
}
