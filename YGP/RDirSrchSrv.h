#ifndef RDIRSRCHSRV_H
#define RDIRSRCHSRV_H

//$Id: RDirSrchSrv.h,v 1.16 2008/03/29 17:35:17 markus Rel $

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


#include <string>

#include <YGP/File.h>
#include <YGP/Socket.h>


namespace YGP {
class CommError;

/**Class for a server to enable remote directory searching.
   This is the server for the RemoteDirSearch-class.

   Supported commands are:

     - -> <b>Check</b>="<<tt>file</tt>>"<br>
       <- <b>RC</b>=0

     - -> <b>Find</b>="<<tt>file</tt>>";<b>Attr</b>=<<tt>attr</tt>><br>
       <- <b>RC</b>=0;<b>%File</b>="<<tt>file</tt>>";<b>Size</b>=<<tt>size</tt>><b>Time</b>=<<tt>timestamp</tt>><b>Attr</b>=<<tt>attributes</tt>>

     - -> <b>Next</b><br>
       <- <b>RC</b>=0;<b>%File</b>="<<tt>file</tt>>";<b>Size</b>=<<tt>size</tt>><b>Time</b>=<<tt>timestamp</tt>><b>Attr</b>=<<tt>attributes</tt>>

     - -> <b>End</b><br>
       <- <b>RC</b>=0

     - -> <b>Open</b>="<<tt>file</tt>>";<b>Mode</b>=<<tt>mode</tt>><br>
       <- <b>RC</b>=0;<b>ID</b>=<<tt>ID</tt>>

     - -> <b>Read</b>="<<tt>ID</tt>>";<b>Length</b>=<<tt>length</tt>><br>
       <- <b>RC</b>=0;<b>Length</b>=<<tt>length</tt>>;<b>Data</b>=<<tt>data</tt>>

     - -> <b>Write</b>=<<tt>ID</tt>>;<b>Length</b>=<<tt>length</tt>>;<b>Data</b>="<<tt>data</tt>>"<br>
       <- <b>RC</b>=0

     - -> <b>EOF</b>=<<tt>ID</tt>><br>
       <- <b>RC</b>=0

     - -> <b>Close</b>=<<tt>ID</tt>><br>
       <- <b>RC</b>=0

   Errors are reported in the following format:
     - <b>RC</b>=<<tt>status</tt>>[;<b>E</b>=<<tt>errortext</tt>>]
*/
class RemoteDirSearchSrv {
 public:
   //@Section manager-functions
   RemoteDirSearchSrv ();
   ~RemoteDirSearchSrv ();

   int performCommands (int socket) throw (YGP::CommError);

 private:
   void writeResult (Socket& socket, const File& result) const throw (YGP::CommError);
   int  writeError (Socket& socket, int error, bool desc = false) const throw (YGP::CommError);

   void handleArgError (Socket& sock, const std::string& error) const;
};

}

#endif // RDIRSRCHSRV
