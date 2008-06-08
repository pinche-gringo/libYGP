#ifndef CGP_CIDIRCOMP_H
#define CGP_CIDIRCOMP_H

//$Id: CIDirSrch.h,v 1.10 2008/06/08 12:32:07 markus Rel $

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

#include <CORBA.h>

#include <YGP/File.h>

#include <YGP/DirSrch.h>
#include <CGP/CDirSrch.h>


class CIFile : public CFile_skel {
 public:
   CIFile (const YGP::File& other);

   char*        path ();
   char*        name ();
   CORBA::ULong size ();
   char*        time ();
   CORBA::ULong attributes ();

   CORBA::Boolean isDirectory ();
   CORBA::Boolean isExecuteable ();
   CORBA::Boolean isUserExec ();

   CORBA::Short compare (const char* file);
   CORBA::Short compareObject (CFile_ptr other);

   virtual void exit ();

 private:
   const YGP::File* pFile;
};


class CIDirectorySearch : public CDirectorySearch_skel {
 public:
   CIDirectorySearch () : srch (*new YGP::DirectorySearch) { }
   CIDirectorySearch (YGP::IDirectorySearch* srchobj) : srch (*srchobj ) { }
   ~CIDirectorySearch () { delete &srch; }

   void setSearchValue (const char* file);

   ::CFile_ptr find (CORBA::ULong attr);
   ::CFile_ptr next ();

   CORBA::Boolean isValid ();

   char* getSearchValue ();
   char* getDirectory ();
   char* getFileSpec();

   virtual void exit ();

   static CORBA::Char getSplitChar () { return YGP::File::DIRSEPARATOR; }

 private:
   YGP::IDirectorySearch& srch;
};

#endif // CIDIRCOMP
