#ifndef REMOTEFILE_H
#define REMOTEFILE_H

// $Id: RemoteFile.h,v 1.3 2003/02/13 07:15:34 markus Rel $

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

#include <File.h>
#include <Socket.h>

class AByteArray;
class AttributeParse;


typedef struct RemoteFile : public File {
   RemoteFile (Socket& socket) : File (), sock (socket) { }
   RemoteFile (const RemoteFile& o) : File (o), sock (o.sock) { }
   virtual ~RemoteFile ();

   RemoteFile& operator= (const RemoteFile& o) { File::operator= (o); sock = o.sock; return *this; }

   virtual File* clone () const;

   //@Section file-access
   virtual bool isEOF (void* file) const throw (std::string);
   virtual void* open  (const char* mode) const throw (std::string);
   virtual void close (void* file) const throw (std::string);
   virtual int  read  (void* file, char* buffer, unsigned int length) const throw (std::string);
   virtual int  write (void* file, const char* buffer, unsigned int length) const throw (std::string);

 private:
   Socket& sock;

   void handleServerMsg (const AttributeParse& attr, const char* pValue)
        const throw (std::string);
   void handleServerError (const char*) const throw (std::string);
   bool isOK (const AByteArray& answer) const;
} RemoteFile;

#endif
