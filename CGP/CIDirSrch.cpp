//$Id: CIDirSrch.cpp,v 1.9 2003/11/14 20:28:34 markus Exp $

//PROJECT     : General/CORBA
//SUBSYSTEM   : CDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.9 $
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


#include <YGP/Check.h>
#include <YGP/Trace_.h>
#include <YGP/ATStamp.h>

#include <CGP/CIDirSrch.h>


CIFile::CIFile (const YGP::File& other) : pFile (&other) {
}

char* CIFile::path () {
   Check3 (pFile);
   return CORBA::string_dup (pFile->path ());
}

char* CIFile::name () {
   Check3 (pFile);
   return CORBA::string_dup (pFile->name ());
}

CORBA::ULong CIFile::size () {
   Check3 (pFile);
   return pFile->size ();
}

char* CIFile::time () {
   Check3 (pFile);
   YGP::ATimestamp time (pFile->time ());
   return CORBA::string_dup (time.toString ().c_str ());
}

CORBA::ULong CIFile::attributes () {
   Check3 (pFile);
   return YGP::IDirectorySearch::convertFromSysAttribs (pFile->attributes ());
}

CORBA::Boolean CIFile::isDirectory () {
   Check3 (pFile);
   return pFile->isDirectory ();
}

CORBA::Boolean CIFile::isExecuteable () {
   Check3 (pFile);
   return pFile->isExecuteable ();
}

CORBA::Boolean CIFile::isUserExec () {
   Check3 (pFile);
   return pFile->isUserExec ();
}

CORBA::Short CIFile::compare (const char* file) {
   Check3 (pFile);
   return pFile->compare (file);
}

CORBA::Short CIFile::compareObject (CFile_ptr other) {
   Check3 (pFile);
   return pFile->compare (other->name ());
}

void CIFile::exit () {
   CORBA::ORB_var orb (_orb ());
   CORBA::BOA_var boa (_boa ());

   boa->deactivate_impl (CORBA::ImplementationDef::_nil ());
   orb->shutdown (true);
}


void CIDirectorySearch::setSearchValue (const char* file) {
  srch.setSearchValue (file);
}

CFile_ptr CIDirectorySearch::find (CORBA::ULong attr) {
   const YGP::File* pEntry (srch.find (attr));
   return pEntry ? CFile::_duplicate (new CIFile (*pEntry)) : NULL;
}

CFile_ptr CIDirectorySearch::next () {
   const YGP::File* pEntry (srch.next ());
   return pEntry ? CFile::_duplicate (new CIFile (*pEntry)) : NULL;
}

CORBA::Boolean CIDirectorySearch::isValid () {
  return srch.isValid ();
}

char* CIDirectorySearch::getDirectory () {
   return const_cast<char*> (srch.getDirectory ().c_str ());
}

char* CIDirectorySearch::getSearchValue () {
   return CORBA::string_dup (srch.getSearchValue ().c_str ());
}

char* CIDirectorySearch::getFileSpec () {
   return CORBA::string_dup (srch.getFileSpec ().c_str ());
}

void CIDirectorySearch::exit () {
   CORBA::ORB_var orb (_orb ());
   CORBA::BOA_var boa (_boa ());

   boa->deactivate_impl (CORBA::ImplementationDef::_nil ());
   orb->shutdown (true);
}
