#ifndef XDIRSRCH_H
#define XDIRSRCH_H

//$Id: XDirSrch.h,v 1.4 2003/02/03 03:54:30 markus Exp $

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

bool _XDSfileIsValid (const std::string&, const char*);
void _XDSaddNode (string& list, char prefix, const string& node);

class DirectorySearch;
class RemoteDirSearch;
class PathDirectorySearch;


// Template to add file in- and exlusion to the *DirectorySearch classes.
//
// There exists some predifined (typedef'ed) classes:
//
//   - XDirectorySearch: for DirectorySearch
//   - XRemoteDirSearch: for RemoteDirSearch
//   - XPathDirectorySearch: for PathDirectorySearch
template <class Parent>
class XDirectorySearch : public Parent {
 public:
   //@Section manager-functions
   XDirectorySearch () : Parent () { }
   XDirectorySearch (const string& spec) : Parent (spec) { }
   virtual ~XDirectorySearch () { }

   //@Section searching
   virtual const File* find (const string& spec, unsigned long attribs = FILE_NORMAL) {
      setSearchValue (spec);
      return Parent::find (attribs); }
   virtual const File* find (unsigned long attribs = FILE_NORMAL) {
      const File* r = Parent::find (attribs);
      return (r && !_XDSfileIsValid (nodes, r->name ())) ? next () : r;
   }
   virtual const File* next () {
      const File* r;
      do {
         r = Parent::next ();
      } while (r && !_XDSfileIsValid (nodes, r->name ()));
      return r;
   }

   //@Section in/exclusion
   void addFilesToInclude (const string& spec) { _XDSaddNode (nodes, 'i', spec); }
   void addFilesToExclude (const string& spec) { _XDSaddNode (nodes, 'x', spec); }

   void setNodes (const string& spec) { nodes = spec; }

 private:
   //@Section prohibited manager functions
   XDirectorySearch (const IDirectorySearch&);
   XDirectorySearch& operator= (const IDirectorySearch&);

   std::string nodes;
};


typedef XDirectorySearch<DirectorySearch>     ExtDirectorySearch;
typedef XDirectorySearch<RemoteDirSearch>     ExtRemoteDirSearch;
typedef XDirectorySearch<PathDirectorySearch> ExtPathDirectorySearch;


#endif
