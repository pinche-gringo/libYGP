//$Id: CIDirSrch.cpp,v 1.5 2002/07/08 03:31:00 markus Exp $

//PROJECT     : General/CORBA
//SUBSYSTEM   : CDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
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


#include <assert.h>

#include "ATStamp.h"
#include "CIDirSrch.h"


CIFile::CIFile (const File& other) : pFile (&other) {
}

char* CIFile::path () {
   assert (pFile);
   return const_cast <char*> (pFile->path ());
}

char* CIFile::name () {
   assert (pFile);
   return const_cast <char*> (pFile->name ());
}

CORBA::ULong CIFile::size () {
   assert (pFile);
   return pFile->size ();
}

char* CIFile::time () {
   assert (pFile);
   ATimestamp time (pFile->time ());
   return const_cast<char*> (time.toString ().c_str ());
}

CORBA::ULong CIFile::attributes () {
   assert (pFile);
   return IDirectorySearch::convertFromSysAttribs (pFile->attributes ());
}

CORBA::Boolean CIFile::isDirectory () {
   assert (pFile);
   return pFile->isDirectory ();
}

CORBA::Boolean CIFile::isExecuteable () {
   assert (pFile);
   return pFile->isExecuteable ();
}

CORBA::Boolean CIFile::isUserExec () {
   assert (pFile);
   return pFile->isUserExec ();
}

CORBA::Short CIFile::compare (const char* file) {
   assert (pFile);
   return pFile->compare (file);
}

CORBA::Short CIFile::compareObject (CFile_ptr other) {
   assert (pFile);
   return pFile->compare (other->name ());
}



CFile_ptr CIDirectorySearch::find (const char* file, CORBA::ULong attr) {
   const File* pEntry (DirectorySearch::find (file, attr));
   assert (pEntry);
   return new CIFile (*pEntry);
}

CFile_ptr CIDirectorySearch::findnext () {
   const File* pEntry (DirectorySearch::next ());
   assert (pEntry);
   return new CIFile (*pEntry);
}

char* CIDirectorySearch::getSearchValue () {
   return const_cast<char*> (DirectorySearch::getSearchValue ().c_str ());
}
