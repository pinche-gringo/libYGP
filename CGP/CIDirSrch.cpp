 //$Id: CIDirSrch.cpp,v 1.1 2001/01/07 00:48:46 Markus Exp $

//PROJECT     : General/CORBA
//SUBSYSTEM   : CDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 7.1.2000
//COPYRIGHT   : Anticopyright (A) 2000

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


#include <CORBA.h>

#include "ATStamp.h"
#include "DirSrch.h"
#include "CDirSrch.h"

class CIDirEntry : virtual public CDirEntry_skel, virtual public dirEntry {
 public:
   char* path () { return const_cast <char*> (dirEntry::path ()); }
   char* name () { return const_cast <char*> (dirEntry::name ()); }
   CORBA::ULong size () { return dirEntry::size (); }
   char* time () {
      ATimestamp time (dirEntry::time ());
      return const_cast<char*> (time.toString ().c_str ());
   }
   CORBA::ULong attributes () { return dirEntry::attributes (); }

   CORBA::Boolean isDirectory () { return dirEntry::isDirectory (); }
   CORBA::Boolean isExecuteable () { return dirEntry::isExecuteable (); }
   CORBA::Boolean isUserExec () { return dirEntry::isUserExec (); }

   CORBA::Short compare (const char* file) {
      return dirEntry::compare (file); }
   CORBA::Short compareObject (CDirEntry_ptr other) {
      return dirEntry::compare (dynamic_cast<dirEntry*> (other)); }
};


class CIDirectorySearch : virtual public CDirectorySearch,
			  virtual public DirectorySearch {
 public:
   CDirEntry_ptr find (const char* file, CORBA::ULong attr) {
      return DirectorySearch::find (file, entry, attr) ? &entry : NULL; }
   CDirEntry_ptr findnext () { return DirectorySearch::find () ? &entry : NULL; }

   char* getFile () { return const_cast<char*> (DirectorySearch::getFile ().c_str ()); }

   CORBA::Char getSplitChar () { return DirectorySearch::getSplitChar (); }

 private:
   CIDirEntry entry;
};
