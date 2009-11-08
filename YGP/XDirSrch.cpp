//$Id: XDirSrch.cpp,v 1.14 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : XDirectorySearch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.14 $
//AUTHOR      : Markus Schwab
//CREATED     : 17.10.2002
//COPYRIGHT   : Copyright (C) 2002 - 2004, 2007 - 2009

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

#include <boost/tokenizer.hpp>

#include "YGP/Path.h"
#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/FileRExp.h"


namespace YGP {


typedef boost::tokenizer<boost::char_separator<char> > tokenizer;


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


   FileRegularExpr regexp (NULL);
   bool include (false);

   // Test every file in list
   tokenizer list (files, boost::char_separator<char> (YGP::Path::SEPARATOR_STR));
   for (tokenizer::iterator i (list.begin ()); i != list.end (); ++i) {
      TRACE8 ("_XDSfileIsValid (const string&, const char*) const - Node: " << *i);
      Check3 ((i->at (0) == 'i') || (i->at (0) == 'x'));
      include = (i->at (0) == 'i');

      regexp = i->substr (1).c_str ();
      if (regexp.matches (pFile))                      // Test if file matches
          return include;
   } // end-while

   return !include;          // 'x'-nodes starts list -> Imply leading i*-node
}

//-----------------------------------------------------------------------------
/// Prefixes a list with another list of nodes. Every subnode of the
/// node is prefixed with the specified prefix-character
/// \param list List to change
/// \param prefix Leading character
/// \param node Node to check
/// \remarks The node is added to the beginning of the list
//-----------------------------------------------------------------------------
void _XDSaddNode (std::string& list, char prefix, const std::string& node) {
   TRACE9 ("_XDSaddNode (const char*) const - " << ((prefix == 'i') ? '+' : '-')
           << node);
   Check1 ((prefix == 'i') || (prefix == 'x'));

   std::string temp;

   tokenizer val (node, boost::char_separator<char> (YGP::Path::SEPARATOR_STR));
   for (tokenizer::iterator i (val.begin ()); i != val.end (); ++i)
      list = prefix + (*i) + std::string (1, Path::SEPARATOR) + list;
}

}
