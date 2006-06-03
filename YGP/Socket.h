#ifndef SOCKET_H
#define SOCKET_H

//$Id: Socket.h,v 1.18 2006/06/03 21:32:38 markus Rel $

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


#include "ygp-cfg.h"


// Headers for communication
#if HAVE_SYS_SOCKET_H
#  include <sys/types.h>                     // Needed to compile with OpenBSD
#  include <sys/socket.h>
#elif HAVE_WINSOCK2_H
#  include <winsock2.h>
#else
#  error You need a socket implementation!
#endif

#include <string>


#include <YGP/Exception.h>


namespace YGP {

/**A socket is a generalized interprocess communication channel, which
   supports communication between unrelated processes, and even between
   processes running on different machines that communicate over a
   network. This class provides methods to connect to, read from and
   write to another socket.

   At the moment the communications are always handled using streamed
   connections in the Internet namespace.*/
class Socket {
 public:
   Socket () throw (YGP::CommError);
   Socket (int socket) : sock (socket) { }  ///< Constructor from system socket
   Socket (unsigned int port) throw (YGP::CommError);
   Socket (const char* server, unsigned int port) throw (YGP::CommError);
   Socket (const std::string& server, unsigned int port) throw (YGP::CommError);
   Socket (const Socket& other) : sock (other.sock) { }   ///< Copy constructor
   virtual ~Socket ();

   Socket& operator= (const Socket& other) throw (YGP::CommError);
   Socket& operator= (int socket);

   void listenAt (unsigned int port) const throw (YGP::CommError);
   int waitForInput () const throw (YGP::CommError);

   int  read (std::string& input) const throw (YGP::CommError);
   int  read (char* pBuffer, unsigned int lenBuffer) const throw (YGP::CommError);

   /// Specifies the \c server and \c port to write to (for outgoing connections).
   void writeTo (const std::string& server, unsigned int port) const throw (YGP::CommError) {
      writeTo (server.c_str (), port); }
   void writeTo (const char* server, unsigned int port) const throw (YGP::CommError);

   /// Writes the content of \c output to the socket (which must have been connected to an address).
   void write (const std::string& output) const throw (YGP::CommError) {
      write (output.data (), output.length ()); }
   void write (const char* pBuffer) const throw (YGP::CommError);
   void write (const char* pBuffer, unsigned int lenBuffer) const throw (YGP::CommError);

   // General helper-functions
   static unsigned int getPortOfService (const char* service) throw (YGP::CommError);

   /// Convertion from a Socket to a system socket (represented by an integer).
   int number () const { return sock; }
   /// Convertion from a Socket to a system socket (represented by an integer).
   operator int () const { return sock; }

 protected:
   static void throwError (const std::string& error, int errNum) throw (YGP::CommError);

 private:
   // Prohibited manager-functions

   int sock;
};

}

#endif
