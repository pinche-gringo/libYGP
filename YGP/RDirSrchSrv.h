#ifndef YGP_RDIRSRCHSRV_H
#define YGP_RDIRSRCHSRV_H

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

#include <boost/asio/ip/tcp.hpp>

#include <YGP/File.h>

namespace YGP {

/**Class for a server to enable remote directory searching.
   This is the server for the RemoteDirSearch-class.

   Supported commands are:

     - -> <b>Check</b>="<<tt>file</tt>>"<br>
       <- <b>RC</b>=0

     - -> <b>Find</b>="<<tt>file</tt>>";<b>Attr</b>=<<tt>attr</tt>><br>
       <-
   <b>RC</b>=0;<b>%File</b>="<<tt>file</tt>>";<b>Size</b>=<<tt>size</tt>><b>Time</b>=<<tt>timestamp</tt>><b>Attr</b>=<<tt>attributes</tt>>

     - -> <b>Next</b><br>
       <-
   <b>RC</b>=0;<b>%File</b>="<<tt>file</tt>>";<b>Size</b>=<<tt>size</tt>><b>Time</b>=<<tt>timestamp</tt>><b>Attr</b>=<<tt>attributes</tt>>

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

   Every message (in both directions) is terminated by a NUL character.
*/
class RemoteDirSearchSrv {
  public:
    //@Section manager-functions
    RemoteDirSearchSrv();
    ~RemoteDirSearchSrv();

    int performCommands(boost::asio::ip::tcp::socket& sock);

  private:
    void writeResult(boost::asio::ip::tcp::socket& socket, const File& result) const;
    int writeError(boost::asio::ip::tcp::socket& socket, int error, bool desc = false) const;

    void handleArgError(boost::asio::ip::tcp::socket& sock, const std::string& error) const;
};

} // namespace YGP

#endif // RDIRSRCHSRV
