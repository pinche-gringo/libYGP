#ifndef IDIRSRCH_H
#define IDIRSRCH_H

//$Id: IDirSrch.h,v 1.5 2001/09/05 15:58:58 markus Exp $

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

class dirEntry;

// Class to search for files in a certain directory over a network-connection
class IDirectorySearch {
 public:
   //@Section manager-functions
   IDirectorySearch () : pEntry (NULL) { }
   virtual ~IDirectorySearch ();

   virtual void setSearchValue (const std::string& search) = 0;
   std::string getSearchValue () const { return getDirectory () + getFileSpec (); }
   virtual std::string getDirectory () const = 0;
   virtual std::string getFileSpec () const = 0;

   //@Section searching
   virtual int find (dirEntry& result, unsigned long attribs = FILE_NORMAL) = 0;
   virtual int find () = 0;

   virtual bool isValid () const = 0;

   static unsigned long convertToSysAttribs (unsigned long attributes);
   static unsigned long convertFromSysAttribs (unsigned long attributes);

   static const int FILE_NORMAL    = 1;
   static const int FILE_READONLY  = FILE_NORMAL << 1;
   static const int FILE_DIRECTORY = FILE_READONLY << 1;
   static const int FILE_HIDDEN    = FILE_DIRECTORY << 1;

 protected:
   dirEntry* pEntry;

 private:
   //@Section prohibited manager functions
   IDirectorySearch (const IDirectorySearch&);
   IDirectorySearch& operator= (const IDirectorySearch&);
};

#endif // IDIRSRCH_H
