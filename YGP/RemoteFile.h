#ifndef YGP_REMOTEFILE_H
#define YGP_REMOTEFILE_H

// $Id: RemoteFile.h,v 1.10 2008/05/18 13:21:27 markus Rel $

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

#include <YGP/File.h>
#include <YGP/Socket.h>
#include <YGP/Exception.h>


namespace YGP {

/**Class representing a file on a remote server with simple
   management-functions.

   This class offers the same functionality as its parent although the file
   is (or can be) actually stored on a remote computer.

   This class is not really (very) useful by itself but designed to be used by
   a RemoteDirSearch-object.
*/
typedef struct RemoteFile : public File {
   /// Default constructor; creates an empty object (holding no file) with the
   /// socket for the communication
   RemoteFile (Socket& socket) : sock (socket) { }
   /// Copy constructor; with the socket for the communication
   RemoteFile (const RemoteFile& o) : File (o), sock (o.sock) { }
   virtual ~RemoteFile ();

   /// Assignment operator; with the socket for the communication
   RemoteFile& operator= (const RemoteFile& o) { File::operator= (o); sock = o.sock; return *this; }

   virtual File* clone () const;

   //@Section file-access
   virtual bool isEOF (void* file) const throw (YGP::FileError);
   virtual void* open  (const char* mode) const throw (YGP::FileError);
   virtual void close (void* file) const throw (YGP::FileError);
   virtual int  read  (void* file, char* buffer, unsigned int length) const throw (YGP::FileError);
   virtual int  write (void* file, const char* buffer, unsigned int length) const throw (YGP::FileError);

 private:
   Socket& sock;

   void handleServerMsg (const AttributeParse& attr, const char* pValue)
        const throw (YGP::FileError);
   void handleServerError (const char*) const throw (YGP::FileError);
   bool isOK (const std::string& answer) const;
} RemoteFile;

}

#endif
