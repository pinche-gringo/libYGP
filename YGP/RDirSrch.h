#ifndef RDIRSRCH_H
#define RDIRSRCH_H

//$Id: RDirSrch.h,v 1.9 2001/09/27 22:02:25 markus Exp $

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
#include <ATStamp.h>
#include <AttrParse.h>

#include <IDirSrch.h>


// Class to search for files in a directory over a
// network-connection. This search can be restricted to files matching
// certain name-criterias or by attributes.
//
// The name-part of the files to search supports UNIX-like wildcards;
// that are the asterisk (*) for any number of any characters, the
// question-mark for any single character and a set of characters in
// brackets (([) and (])). This set can contain a list of characters
// (like [abcde]) or a region (like [a-e]). To invert this set use a
// leading caret (^) or a leading exclamation mark (!), like
// ([^a-e]).
//
// The found (and matching) files are retrieved by objects of type
// dirEntry.
//
// Note: The class does not do any word expansion for the search-path
//       (like expanding the tilde (~) to the home-directory)!
class RemoteDirSearch : public IDirectorySearch {
 public:
   //@Section manager-functions
   RemoteDirSearch () : IDirectorySearch (), sock (-1) { }
   RemoteDirSearch (const std::string& search) throw (domain_error);
   RemoteDirSearch (const std::string& search, unsigned int port)
      throw (domain_error);
   virtual ~RemoteDirSearch ();

   //@Section initializing
   void sendTo (const std::string& server, unsigned int port) throw (domain_error);

   //@Section manipulating
   virtual void setSearchValue (const std::string& search);
   virtual std::string getDirectory () const;
   virtual std::string getFileSpec () const;

   //@Section searching
   virtual int find (dirEntry& result, unsigned long attribs = FILE_NORMAL)
      throw (std::string);
   virtual int find () throw (std::string);

   enum { FILE_NORMAL = 0, FILE_READONLY = 1, FILE_DIRECTORY = 2, FILE_HIDDEN = 4 };

   virtual bool isValid () const throw (domain_error);
   bool isValid (const std::string& dir) throw (domain_error);

   static const char SEPERATOR = ':';

 protected:
   // Variables for sending
   Socket      sock;

 private:
   //@Section prohibited manager functions
   RemoteDirSearch (const RemoteDirSearch&);
   RemoteDirSearch& operator= (const RemoteDirSearch&);

   void init (const std::string& server, unsigned int port) throw (domain_error);

   int posSeperator (const std::string& dir) const;

   bool isOK (const AByteArray& answer) const;
   void setFiledata (const char* pAnswer) throw (std::string);
   int handleServerError (const char* pAnswer) throw (std::string);

   std::string server;
   std::string files;

   // Variables for receiving
   AttributeParse attrs;
   std::string file;
   ATimestamp  time;
   unsigned long attr;
   unsigned long size;
};

#endif // RDIRSRCH_H
