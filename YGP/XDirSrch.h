#ifndef XDIRSRCH_H
#define XDIRSRCH_H

//$Id: XDirSrch.h,v 1.7 2003/07/03 04:49:47 markus Rel $

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
void _XDSaddNode (std::string& list, char prefix, const std::string& node);

class DirectorySearch;
class RemoteDirSearch;
class PathDirectorySearch;


/**Template to add file in- and exlusion to the *DirectorySearch classes.

   There exists some predefined (typedef'ed) classes:

     - XDirectorySearch: for DirectorySearch
     - XRemoteDirSearch: for RemoteDirSearch
     - XPathDirectorySearch: for PathDirectorySearch
*/
template <class Parent>
class XDirectorySearch : public Parent {
 public:
   //@Section manager-functions
   /// Constructor
   XDirectorySearch () : Parent () { }
   /// Constructor with specification of which files to search for
   XDirectorySearch (const std::string& spec) : Parent (spec) { }
   /// Destructor
   virtual ~XDirectorySearch () { }

   /// \name Searching
   //@{
   /// Returns the first found file matching \c spec, having the attributes \attribs
   virtual const File* find (const std::string& spec, unsigned long attribs = FILE_NORMAL) {
      setSearchValue (spec);
      return Parent::find (attribs); }
   /// Returns the first found file, having the attributes \attribs
   /// \pre The files to search for must have been previously specified
   virtual const File* find (unsigned long attribs = FILE_NORMAL) {
      const File* r = Parent::find (attribs);
      return (r && !_XDSfileIsValid (nodes, r->name ())) ? next () : r;
   }
   /// Returns the next file matching the previously specifed search criteria
   virtual const File* next () {
      const File* r;
      do {
         r = Parent::next ();
      } while (r && !_XDSfileIsValid (nodes, r->name ()));
      return r;
   }
   //@}

   /// \name In/exclusion
   //@{
   /// Adds files to include in the search. This files are added at the end
   /// of the previously specifed files (and are therefore less significant).
   ///
   /// The parameter can also contain a list of files, separated with the
   /// character to separate paths of the operating system (the colon (:) in
   /// UNIX; the semicolon (;) in DOS/WINDOZE.
   void addFilesToInclude (const std::string& spec) { _XDSaddNode (nodes, 'i', spec); }
   /// Adds files to exclude in the search. This files are added at the end
   /// of the previously specifed files (and are therefore less significant).
   ///
   /// The parameter can also contain a list of files, separated with the
   /// character to separate paths of the operating system (the colon (:) in
   /// UNIX; the semicolon (;) in DOS/WINDOZE.
   void addFilesToExclude (const std::string& spec) { _XDSaddNode (nodes, 'x', spec); }
   //@}

 protected:
   /// Sets directly the files to in- or exclude. \c Spec must be in the right
   /// format (e.g. every node must have a prefix (i or x) specifying if it
   /// should be added or removed.
   void setNodes (const std::string& spec) { nodes = spec; }

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
