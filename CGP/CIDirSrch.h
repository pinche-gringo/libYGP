#ifndef CIDIRCOMP_H
#define CIDIRCOMP_H

//$Id: CIDirSrch.h,v 1.6 2002/07/15 21:03:01 markus Rel $

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

#include <File.h>

#include <DirSrch.h>
#include <CDirSrch.h>

class CIFile : public CFile_skel {
 public:
   CIFile (const File& other);

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
   const File* pFile;
};


class CIDirectorySearch : public CDirectorySearch_skel {
 public:
   CIDirectorySearch () : srch (*new DirectorySearch) { }
   CIDirectorySearch (IDirectorySearch* srchobj) : srch (*srchobj ) { }
   ~CIDirectorySearch () { delete &srch; }

   void setSearchValue (const char* file);

   ::CFile_ptr find (CORBA::ULong attr);
   ::CFile_ptr next ();

   CORBA::Boolean isValid ();

   char* getSearchValue ();
   char* getDirectory ();
   char* getFileSpec();

   virtual void exit ();

   static CORBA::Char getSplitChar () { return File::DIRSEPARATOR; }

 private:
   IDirectorySearch& srch;
};


#endif // CIDIRCOMP
