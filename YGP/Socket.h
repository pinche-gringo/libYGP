#ifndef SOCKET_H
#define SOCKET_H

//$Id: Socket.h,v 1.9 2003/06/13 18:46:45 markus Rel $

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


#include "gzo-cfg.h"


// Headers for communication
#if SYSTEM == UNIX
#  include <sys/select.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netdb.h>
#else
#  if SYSTEM == WINDOWS
#     include "winsock2.h"
#  else
#     error Not yet implemented!
#  endif
#endif

#include <string>
#include <stdexcept>

/** A socket is a generalized interprocess communication channel, which
   supports communication between unrelated processes, and even between
   processes running on different machines that communicate over a
   network. This class provides methods to connect to, read from and
   write to another socket.
  
   At the moment the communications are always handled using streamed
   connections in the Internet namespace.*/
class Socket {
 public:
   Socket () throw (std::domain_error);
   Socket (int socket) : sock (socket) { }  ///< Constructor from system socket
   Socket (unsigned int port) throw (std::domain_error);
   Socket (const char* server, unsigned int port) throw (std::domain_error);
   Socket (const std::string& server, unsigned int port) throw (std::domain_error);
   Socket (const Socket& other) : sock (other.sock) { }   ///< Copy constructor
   virtual ~Socket ();

   Socket& operator= (const Socket& other) throw (std::domain_error);
   Socket& operator= (int socket);

   void listenAt (unsigned int port) const throw (std::domain_error);
   int waitForInput () const throw (std::domain_error);

   int  read (std::string& input) const throw (std::domain_error);
   int  read (char* pBuffer, unsigned int lenBuffer) const throw (std::domain_error);

   /// Specifies the \c server and \c port to write to (for outgoing connections).
   void writeTo (const std::string& server, unsigned int port) const throw (std::domain_error) {
      writeTo (server.c_str (), port); }
   void writeTo (const char* server, unsigned int port) const throw (std::domain_error);

   /// Writes the content of \c output to the socket (which must have been connected to an address).
   void write (const std::string& output) const throw (std::domain_error) {
      write (output.data (), output.length ()); }
   void write (const char* pBuffer) const throw (std::domain_error);
   void write (const char* pBuffer, unsigned int lenBuffer) const throw (std::domain_error);

   // General helper-functions
   static unsigned int getPortOfService (const char* service) throw (std::domain_error);

   /// Convertion from a Socket to a system socket (represented by an integer).
   int number () const { return sock; }
   /// Convertion from a Socket to a system socket (represented by an integer).
   operator int () const { return sock; }

 protected:
   static void throwError (const std::string& error, int errNum) throw (std::domain_error);

 private:
   // Prohibited manager-functions

   int sock;
};

#endif
