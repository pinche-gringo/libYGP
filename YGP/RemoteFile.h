#ifndef YGP_REMOTEFILE_H
#define YGP_REMOTEFILE_H

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

#include <YGP/Exception.h>
#include <YGP/File.h>
#include <YGP/Socket.h>

namespace YGP {

/**Class representing a file on a remote server with simple
   management-functions.

   This class offers the same functionality as its parent although the file
   is (or can be) actually stored on a remote computer.

   This class is not really (very) useful by itself but designed to be used by
   a RemoteDirSearch-object.
*/
using RemoteFile = struct RemoteFile : public File {
    /// Default constructor; creates an empty object (holding no file) with the
    /// socket for the communication
    RemoteFile(Socket& socket) : sock(socket) {}
    /// Copy constructor; with the socket for the communication
    RemoteFile(const RemoteFile& o)  = default;
    ~RemoteFile() override;

    /// Assignment operator; with the socket for the communication
    RemoteFile& operator=(const RemoteFile& o) = default;

    File* clone() const override;

    //@Section file-access
    bool isEOF(void* file) const override;
    void* open(const char* mode) const override;
    void close(void* file) const override;
    int read(void* file, char* buffer, unsigned int length) const override;
    int write(void* file, const char* buffer, unsigned int length) const override;

  private:
    Socket& sock;

    void handleServerMsg(const AttributeParse& attr, const char* pValue) const;
    void handleServerError(const char*) const;
    bool isOK(const std::string& answer) const;
};

} // namespace YGP

#endif
