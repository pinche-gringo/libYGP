#ifndef RDIRSRCH_H
#define RDIRSRCH_H

//$Id: RDirSrch.h,v 1.6 2001/08/28 20:20:04 markus Exp $

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


#include <time.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include <stdexcept>

#include <gzo-cfg.h>
#include <Socket.h>
#include <IDirSrch.h>


// Class to search for files in a certain directory over a network-connection
class RemoteDirSearch : public IDirectorySearch {
 public:
   //@Section manager-functions
   RemoteDirSearch () : IDirectorySearch (), sock (-1) { }
   RemoteDirSearch (const std::string& host) throw (domain_error);
   RemoteDirSearch (const std::string& host, unsigned int port)
      throw (domain_error);
   virtual ~RemoteDirSearch ();

   //@Section initializing
   void sendTo (const std::string& host, unsigned int port) throw (domain_error);

   //@Section manipulating
   virtual void setSearchValue (const std::string& search);
   virtual const std::string& getDirectory () const;
   virtual const std::string& getFileSpec () const;

   //@Section searching
   inline int find (dirEntry& result, unsigned long attribs = FILE_NORMAL)
      throw (std::string);
   virtual int find () throw (std::string);

   enum { FILE_NORMAL = 0, FILE_READONLY = 1, FILE_DIRECTORY = 2, FILE_HIDDEN = 4 };

   virtual bool isValid () const;
   virtual bool isValid (const std::string& dir) const;

   static const char HOSTSEPERATOR = ':';

 private:
   //@Section prohibited manager functions
   RemoteDirSearch (const RemoteDirSearch&);
   RemoteDirSearch& operator= (const RemoteDirSearch&);

   int posSeperator (const std::string& dir) const;

   bool isOK (const AByteArray& answer) const;
   void setFiledata (const char* pAnswer) throw (std::string);
   int handleServerError (const char* pAnswer) throw (std::string);

   Socket      sock;
   std::string host;
   std::string files;
};

#endif // RDIRSRCH_H
