#ifndef IDIRSRCH_H
#define IDIRSRCH_H

//$Id: IDirSrch.h,v 1.2 2001/04/09 15:05:30 markus Exp $

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
   virtual ~IDirectorySearch () { }

   //@Section searching
   virtual int find (const std::string& search, dirEntry& result,
		     unsigned long attribs) = 0;
   virtual int find () = 0;

   virtual bool isValid () const = 0;
   virtual bool isValid (const std::string& file) const = 0;

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
