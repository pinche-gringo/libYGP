#ifndef SOCKET_H
#define SOCKET_H

//$Id: Socket.h,v 1.2 2001/04/02 21:03:14 markus Exp $

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

// Heaers for communication
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string>
#include <stdexcept>

#include <AByteArray.h>

// Internet-communication; errors are reported with exceptions
class Socket {
 public:
   Socket () throw (domain_error);
   Socket (int other) : sock (other) { }
   Socket (const Socket& other) : sock (other.sock) { }
   Socket& operator= (const Socket&) throw (domain_error);
   Socket& operator= (int socket);

   Socket (unsigned int port) throw (domain_error);
   Socket (const char* host, unsigned int port) throw (domain_error);
   Socket (const std::string& host, unsigned int port) throw (domain_error);
   virtual ~Socket ();

   void listenAt (unsigned int port) throw (domain_error);
   int waitForInput () const throw (domain_error);

   int  read (AByteArray& input) throw (domain_error);
   int  read (char* pBuffer, int lenBuffer) throw (domain_error);

   void writeTo (const char* host, unsigned int port) throw (domain_error);
   void writeTo (const std::string& host, unsigned int port) throw (domain_error) {
      writeTo (host.c_str (), port); }
   void write (const char* pBuffer, int lenBuffer) const throw (domain_error);
   void write (const AByteArray& output) const throw (domain_error) {
      write (output.data (), output.length ()); }

   // General helper-functions
   static unsigned int getPortOfService (const char* service) throw (domain_error);

   int number () const { return sock; }

 protected:
   static void throwError (const std::string& error, int errNum) throw (domain_error);

 private:
   // Prohibited manager-functions

   int sock;
};

#endif
