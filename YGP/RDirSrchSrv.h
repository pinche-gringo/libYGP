#ifndef RDIRSRCHSRV_H
#define RDIRSRCHSRV_H

//$Id: RDirSrchSrv.h,v 1.2 2001/08/17 13:23:35 markus Exp $

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

#include <gzo-cfg.h>


class dirEntry;
class AByteArray;


// Class to search for files in a certain directory
// This is the server for the RemoteDirSearch-class
//
// Supported commands are:
//   - Check:"<file>"
//   - Find:"<file>;Attr=<attr>
//   - Next
//
// Possible answers to those commands are
//    - RC=<status>[;<errortext>]
//    - RC=0;File="<file>";Size=<size>;Time=<timestamp>
//
// RC=0 always means, that the commands completed successfully. This is also
// true for the "Check"-command; RC=0 means in that case, that the file has
// been found!
class RemoteDirSearchSrv {
 public:
   //@Section manager-functions
   RemoteDirSearchSrv ();
   virtual ~RemoteDirSearchSrv ();

   int performCommands (int socket) throw (domain_error);

 private:
   void writeResult (Socket& socket, const dirEntry& result) const throw (domain_error);
   int  writeError (Socket& socket, int error) const throw (domain_error);

   bool isOK (const AByteArray& answer) const;
};

#endif // RDIRSRCHSRV
