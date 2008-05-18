#ifndef CGP_CIDIRCOMP_H
#define CGP_CIDIRCOMP_H

//$Id: CIDirSrch.h,v 1.9 2008/05/18 13:21:27 markus Rel $

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
