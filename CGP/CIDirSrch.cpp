//$Id: CIDirSrch.cpp,v 1.2 2001/01/24 16:27:05 Markus Exp $

//PROJECT     : General/CORBA
//SUBSYSTEM   : CDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
//AUTHOR      : Markus Schwab
//CREATED     : 7.1.2001
//COPYRIGHT   : Anticopyright (A) 2001

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


#include "ATStamp.h"
#include "CIDirSrch.h"


CIDirEntry::CIDirEntry (const dirEntry& other) : dirEntry (other) {
}

char* CIDirEntry::path () {
   return const_cast <char*> (dirEntry::path ());
}

char* CIDirEntry::name () {
   return const_cast <char*> (dirEntry::name ());
}

CORBA::ULong CIDirEntry::size () {
   return dirEntry::size ();
}

char* CIDirEntry::time () {
   ATimestamp time (dirEntry::time ());
   return const_cast<char*> (time.toString ().c_str ());
}

CORBA::ULong CIDirEntry::attributes () {
   return dirEntry::attributes ();
}

CORBA::Boolean CIDirEntry::isDirectory () {
   return dirEntry::isDirectory ();
}

CORBA::Boolean CIDirEntry::isExecuteable () {
   return dirEntry::isExecuteable ();
}

CORBA::Boolean CIDirEntry::isUserExec () {
   return dirEntry::isUserExec ();
}

CORBA::Short CIDirEntry::compare (const char* file) {
   return dirEntry::compare (file);
}

CORBA::Short CIDirEntry::compareObject (CDirEntry_ptr other) {
   return dirEntry::compare (dynamic_cast<dirEntry*> (other));
}



CDirEntry_ptr CIDirectorySearch::find (const char* file, CORBA::ULong attr) {
   return DirectorySearch::find (file, entry, attr) ? &entry : NULL;
}

CDirEntry_ptr CIDirectorySearch::findnext () {
   return DirectorySearch::find () ? &entry : NULL;
}

char* CIDirectorySearch::getFile () {
   return const_cast<char*> (DirectorySearch::getFile ().c_str ());
}
