//$Id: CIDirSrch.cpp,v 1.4 2001/08/22 01:38:28 markus Rel $

//PROJECT     : General/CORBA
//SUBSYSTEM   : CDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
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


static unsigned long aAttributes[] = { DirectorySearch::FILE_NORMAL,
                                       DirectorySearch::FILE_READONLY,
                                       DirectorySearch::FILE_DIRECTORY,
                                       DirectorySearch::FILE_HIDDEN };


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
   unsigned long attrs (dirEntry::attributes ());
   unsigned long result (0);

   // Convert dirEntry-attributes to format of component
   for (int i (0); i < (sizeof (aAttributes) / sizeof (aAttributes[0])); ++i)
      if ((attrs & aAttributes[i]) == aAttributes[i])
         result |= (1 << i);
   return result;
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
   unsigned long attrs (0);

   // Convert attributes to format of dirEntry
   for (int i (0); i < (sizeof (aAttributes) / sizeof (aAttributes[0])); ++i)
      if (attr & i)
         attrs |= aAttributes[i];
   
   return DirectorySearch::find (file, entry, attrs) ? &entry : NULL;
}

CDirEntry_ptr CIDirectorySearch::findnext () {
   return DirectorySearch::find () ? &entry : NULL;
}

char* CIDirectorySearch::getSearchValue () {
   return const_cast<char*> (DirectorySearch::getSearchValue ().c_str ());
}
