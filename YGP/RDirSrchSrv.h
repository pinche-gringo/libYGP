#ifndef RDIRSRCHSRV_H
#define RDIRSRCHSRV_H

//$Id: RDirSrchSrv.h,v 1.9 2002/05/24 06:52:49 markus Rel $

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


class AByteArray;


// Class to search for files in a certain directory
// This is the server for the RemoteDirSearch-class
//
// Supported commands are:
//   -> Check="<file>"
//   <- RC=0
//
//   -> Find="<file>;Attr=<attr>
//   <- RC=0;File="<file>";Size=<size>;Time=<timestamp>;Attr=<attributes>
//
//   -> Next
//   <- RC=0;File="<file>";Size=<size>;Time=<timestamp>;Attr=<attributes>
//
//   -> End
//   <- RC=0
//
//   -> Open="<file>";Mode=<mode>
//   <- RC=0;ID=<ID>
//
//   -> Read="<ID>;Length=<length>
//   <- RC=0;Length=<length>;Data="<data>"
//
//   -> Write=<ID>;Length=<length>;Data="<data>"
//   <- RC=0
//
//   -> EOF=<ID>
//   <- RC=0
//
//   -> Close=<ID>
//   <- RC=0
//
// Errors are reported in the following format:
//   - RC=<status>[;E=<errortext>]
class RemoteDirSearchSrv {
 public:
   //@Section manager-functions
   RemoteDirSearchSrv ();
   ~RemoteDirSearchSrv ();

   int performCommands (int socket) throw (std::domain_error);

 private:
   void writeResult (Socket& socket, const File& result) const throw (std::domain_error);
   int  writeError (Socket& socket, int error, bool desc = false) const throw (std::domain_error);

   void handleArgError (Socket& sock, const std::string& error) const;
};

#endif // RDIRSRCHSRV
