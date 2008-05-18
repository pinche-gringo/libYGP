//$Id: Socket.cpp,v 1.30 2008/05/18 13:23:31 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Socket
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.30 $
//AUTHOR      : Markus Schwab
//CREATED     : 24.3.2001
//COPYRIGHT   : Copyright (C) 2001 - 2004, 2006, 2008

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


#include "ygp-cfg.h"

#include <cerrno>
#include <cstring>

#include "YGP/Check.h"
#include "YGP/Trace.h"

#include "YGP/Socket.h"
#include "YGP/Internal.h"


#if HAVE_SYS_SOCKET_H
#  include <sys/select.h>
#  include <netinet/in.h>
#  include <netdb.h>

#  include <unistd.h>
#elif HAVE_WINSOCK2_H
   // Define the macros/functions to access the socket-functions of Windows
   // (of course they are different - probably just to show that they didn't
   // steal the whole thing from BSD)
#  define close	      closesocket
   inline int write (int socket, const char* buffer, int length) {
      return send (socket, buffer, length, 0); }
   inline int read (int socket, char* buffer, int length) {
      return recv (socket, buffer, length, 0); }
#endif


#ifndef HAVE_SSIZE_T
typedef size_t  ssize_t;
#endif
#ifndef HAVE_SOCKLEN_T
typedef int socklen_t;
#endif


namespace YGP {

//----------------------------------------------------------------------------
/// Defaultconstructor; creates an socket but without any connection.
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
Socket::Socket () throw (YGP::CommError)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket ()");

   if (sock < 0)
      throwError (_("Can't create socket"), errno);
}

//----------------------------------------------------------------------------
/// Constructor; creates an socket which waits for input on \c port.
/// \param port: Port to listen at
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
Socket::Socket (unsigned int port) throw (YGP::CommError)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket (unsigned int)");

   if (sock < 0)
      throwError (_("Can't create socket"), errno);
   listenAt (port);
}

//----------------------------------------------------------------------------
/// Constructor; creates an socket which tries to write to \c server using the
/// passed \c port.
/// \param server: Name (or number) of server to send to
/// \param port: Port to write to
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
Socket::Socket (const char* server, unsigned int port) throw (YGP::CommError)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket (const char*, unsigned int)");
   Check1 (server);

   if (sock < 0)
      throwError ("Can't create socket", errno);

   writeTo (server, port);
}

//----------------------------------------------------------------------------
/// Constructor; creates an socket which tries to write to \c server using the
/// passed  \c port.
/// \param server: Name (or number) of server to send to
/// \param port: Port to write to
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
Socket::Socket (const std::string& server, unsigned int port) throw (YGP::CommError)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket (const std::string&, unsigned int)");

   if (sock < 0)
      throwError (_("Can't create socket"), errno);

   writeTo (server.c_str (), port);
}

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
Socket::~Socket () {
   TRACE9 ("Socket::~Socket ()");
   ::close (sock);
}


//----------------------------------------------------------------------------
/// Assignment-operator; duplicates a socket. The old socket is closed.
/// \param rhs: Socket to assign
/// \return \c Socket&: Reference to self
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
Socket& Socket::operator= (const Socket& rhs) throw (YGP::CommError) {
   if (&rhs != this) {
      close (sock);
      sock = socket (PF_INET, SOCK_STREAM, 0);
      if (sock < 0)
	 throwError (_("Can't create socket"), errno);
   }
   return *this;
}

//----------------------------------------------------------------------------
/// Assignmentoperator; creates an socket out of a system-socket.
/// \param socket: Socket to assign
/// \return \c Socket&: Reference to self
//----------------------------------------------------------------------------
Socket& Socket::operator= (int socket) {
   close (sock);
   sock = socket;
   return *this;
}


//----------------------------------------------------------------------------
/// Specifies the port to listen at (for incoming connections).
/// \param port: Port at which to listen
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
void Socket::listenAt (unsigned int port) const throw (YGP::CommError) {
   TRACE9 ("Socket::listenAt (unsigned int) - " << port << " (" << sock << ')');

   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port = htons (port);
   addr.sin_addr.s_addr = htonl (INADDR_ANY);

   if (::bind (sock, (struct sockaddr*)&addr, sizeof (addr)) < 0)
      throwError (_("Can't bind to port"), errno);

   if (::listen (sock, 1) < 0)
      throwError (_("Can't listen on port"), 0);
}

//----------------------------------------------------------------------------
/// Retrieves the port-number of the passed textual port-description (service
/// name or number as text).
/// \param service: Text describing port
/// \return <tt>unsigned int</tt>: Number of the passed service
/// \throw YGP::CommError in case of an invalid input
//----------------------------------------------------------------------------
unsigned int Socket::getPortOfService (const char* service) throw (YGP::CommError) {
   TRACE9 ("Socket::getPortOfService (const char*)");

   char* pTail = NULL;
   TRACE8 ("Passed service: " << service);

   errno = 0;
   int port = strtol (service, &pTail, 0);
   if (errno || (pTail && *pTail)) {
      struct servent* pServent = getservbyname (service, "tcp");

      if (pServent)
	 port = ntohs (pServent->s_port);
      else {
         std::string error (_("Port '%1' is neither numeric (decimal, octal or "
                              "hexadecimal) nor a service"));
         error.replace (error.find ("%1"), 2, service);
	 throwError (error, 0);
      }
   }

   return port;
}

//----------------------------------------------------------------------------
/// Reads all available data into the buffer specified by \c input from the
/// previously specified source.
/// \param input: String receiving the input
/// \return \c int: Number of bytes read<br>
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
int Socket::read (std::string& input) const throw (YGP::CommError) {
   TRACE9 ("Socket::read (std::string&)" << " (" << sock << ')');

   char buffer[80] = "";
   ssize_t cRead;
   input = "";

   // Read from socket til either error or buffer not completely filled
   while ((cRead = ::read (sock, buffer, sizeof (buffer))) != -1) {
      input.append (buffer, cRead);
      if ((unsigned int)cRead < sizeof (buffer))
         break;
   }

   if (cRead == -1) {
      TRACE9 ("Socket::read (std::string&) - error=" << errno << "; Bytes="
              << cRead);
      throwError (_("Error reading data"), errno);
   }

   TRACE5 ("Socket::read (std::string&) - read: " << input.data ());
   return input.length ();
}

//----------------------------------------------------------------------------
/// Reads up to \c lenBuffer bytes data into the buffer specified by \c pBuffer
/// from the previously specified source.
/// \param pBuffer: Pointer to buffer receiving the input
/// \param lenBuffer: Length of that buffer
/// \return \c int: Number of bytes read<br>
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
int Socket::read (char* pBuffer, unsigned int lenBuffer) const throw (YGP::CommError) {
   TRACE9 ("Socket::read (const char*, int)" << " (" << sock << ')');

   ssize_t cRead (::read (sock, pBuffer, lenBuffer));
   if (cRead == -1)
      throwError (_("Error reading data"), errno);

   TRACE5 ("Socket::read (char*, int) - read: " << pBuffer);
   return cRead;
}

//----------------------------------------------------------------------------
/// Waits until someone wants to create a (new) connection using the previously
/// specified port.
///
/// The new (system-)socket for this communication is returned.
///
/// <b>Technical note</b>: If a new connection is opened to a Socket, it is
/// actually "duplicated" and returned to enable multiple connections to one
/// port (socket).
/// \return \c int: Socket over which to communicate<br>
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
int Socket::waitForInput () const throw (YGP::CommError) {
   TRACE9 ("Socket::waitForInput (Socket&) const - (" << sock << ')');

   struct sockaddr_in client;
   socklen_t size = sizeof (client);

   int newSocket (accept (sock, (struct sockaddr*)&client, &size));
   if (newSocket < 0)
      throwError (_("Error accepting connection"), errno);

   TRACE8 ("Socket::waitForInput (Socket&) const - assigning " << newSocket);
   return newSocket;
}

//----------------------------------------------------------------------------
/// Specifies the \c server and \c port to write to (for outgoing connections).
/// \param server: Name (or number) of the server to connect to
/// \param port: Portnumber to connect to
/// \throw YGP::CommError in case of a communication error
/// \pre \c server not NULL<br>
//----------------------------------------------------------------------------
void Socket::writeTo (const char* server, unsigned int port) const throw (YGP::CommError) {
   TRACE9 ("Socket::writeTo (const char*, unsigned int) - " << server << ':' << port);
   Check1 (server);

   struct sockaddr_in name;
   name.sin_family = AF_INET;
   name.sin_port = htons (port);

   struct hostent* hostinfo = gethostbyname (server);
   if (!hostinfo) {
      std::string error (_("Can't resolve name '%1'"));
      error.replace (error.find ("%1"), 2, server);
      throwError (error, 0);
   }

   name.sin_addr = *(struct in_addr*)hostinfo->h_addr;

   if (connect (sock, (struct sockaddr*)&name, sizeof (name)) < 0) {
      std::string error (_("Can't connect to server '%1'"));
      error.replace (error.find ("%1"), 2, server);
      throwError (error, errno);
   }
}

//----------------------------------------------------------------------------
/// Writes \c %lenBuff bytes of the buffer pointed to by \c pBuffer to the
/// socket (which must have been connected to an address).
/// \param pBuffer: Buffer to write
/// \param lenBuffer: Length of data to write
/// \throw YGP::CommError in case of a communication error
/// \pre \c pBuffer not NULL
//----------------------------------------------------------------------------
void Socket::write (const char* pBuffer, unsigned int lenBuffer) const
   throw (YGP::CommError) {
   TRACE5 ("Socket::write (const char*, int) const - " << pBuffer << " (" << sock << ')');
   Check1 (pBuffer);

   if (::write (sock, pBuffer, lenBuffer) < 0)
      throwError (_("Error sending data"), errno);
}

//----------------------------------------------------------------------------
/// Writes the contents of \c pBuffer up to the first zero-byte to the socket
/// (which must have been connected to an address).
/// \param pBuffer: Buffer to write
/// \throw YGP::CommError in case of a communication error
/// \pre \c pBuffer not NULL
//----------------------------------------------------------------------------
void Socket::write (const char* pBuffer) const throw (YGP::CommError) {
   Check1 (pBuffer);
   TRACE9 ("Socket::write (const char*) const - (" << sock << ')');

   write (pBuffer, strlen (pBuffer));
}

//----------------------------------------------------------------------------
/// Throws a domain_error with an explaining text.
/// \param error: Text describing the error
/// \param errNum: Number of error; if !=0 an explaining text is appended
/// \throw YGP::CommError in case of a communication error
//----------------------------------------------------------------------------
void Socket::throwError (const std::string& error, int errNum) throw (YGP::CommError) {
   std::string str (error);
   if (errNum) {
      str += ": ";
      str += strerror (errNum);
   }
   throw (YGP::CommError (str));
}

}

