//$Id: Socket.cpp,v 1.15 2003/03/06 04:16:02 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Socket
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.15 $
//AUTHOR      : Markus Schwab
//CREATED     : 24.3.2001
//COPYRIGHT   : Anticopyright (A) 2001, 2002

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

#include <errno.h>
#include <string.h>

#include "Check.h"
#include "Trace_.h"
#include "AByteArray.h"

#include "Socket.h"
#include "Internal.h"


#if SYSTEM == UNIX
#  include <unistd.h>
#else
#  if SYSTEM == WINDOWS
      // Define the macros/functions to access the socket-function of Windows
      // (of course they are different - as if they didn't steal the whole
      // thing from BSD)
#     define close			closesocket
      inline int write (int socket, const char* buffer, int length) {
         return send (socket, buffer, length, 0); }
      inline int read (int socket, char* buffer, int length) {
         return recv (socket, buffer, length, 0); }
#  endif
#endif


#ifndef HAVE_SSIZE_T
typedef size_t  ssize_t;
#endif
#ifndef HAVE_SOCKLEN_T
typedef int socklen_t;
#endif



/*--------------------------------------------------------------------------*/
//Purpose   : Defaultconstructor; creates an socket but without any connection.
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
Socket::Socket () throw (std::domain_error)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket ()");

   if (sock < 0)
      throwError (_("Can't create socket"), errno);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; creates an socket which waits for input on port.
//Parameters: port: Port to listen at
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
Socket::Socket (unsigned int port) throw (std::domain_error)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket (unsigned int)");

   if (sock < 0)
      throwError (_("Can't create socket"), errno);
   listenAt (port);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; creates an socket which tries to write to server
//            using the passed port.
//Parameters: server: Name (or number) of server to send to
//            port: Port to write to
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
Socket::Socket (const char* server, unsigned int port) throw (std::domain_error)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket (const char*, unsigned int)");
   Check1 (server);

   if (sock < 0)
      throwError ("Can't create socket", errno);

   writeTo (server, port);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; creates an socket which tries to write to server
//            using the passed port.
//Parameters: server: Name (or number) of server to send to
//            port: Port to write to
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
Socket::Socket (const std::string& server, unsigned int port) throw (std::domain_error)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket (const std::string&, unsigned int)");

   if (sock < 0)
      throwError (_("Can't create socket"), errno);

   writeTo (server.c_str (), port);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
Socket::~Socket () {
   TRACE9 ("Socket::~Socket ()");
   ::close (sock);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator; duplicates a socket. The old socket is closed.
//Parameters: rhs: Socket to assign
//Returns   : Socket&: Reference to self
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
Socket& Socket::operator= (const Socket& rhs) throw (std::domain_error) {
   if (&rhs != this) {
      close (sock);
      sock = socket (PF_INET, SOCK_STREAM, 0);
      if (sock < 0)
	 throwError (_("Can't create socket"), errno);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignmentoperator; creates an socket out of a system-socket.
//Parameters: rhs: Socket to assign
//Returns   : Socket&: Reference to self
/*--------------------------------------------------------------------------*/
Socket& Socket::operator= (int socket) {
   close (sock);
   sock = socket;
   return *this;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Specifies the port to listen at (for incoming connections).
//Parameters: port: Port at which to listen
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
void Socket::listenAt (unsigned int port) const throw (std::domain_error) {
   TRACE9 ("Socket::listenAt (unsigned int) - " << port << " (" << sock << ')');

   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port = htons (port);
   addr.sin_addr.s_addr = htonl (INADDR_ANY);

   if (::bind (sock, (struct sockaddr*)&addr, sizeof (addr)) < 0)
      throwError (_("Can't bind socket"), errno);

   if (::listen (sock, 1) < 0)
      throwError (_("Can't listen on socket"), 0);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the port-number of the passed textual port-description
//            (service name or number as text).
//Parameters: service: Text describing port
//Throws    : std::domain_error in case of an invalid input
/*--------------------------------------------------------------------------*/
unsigned int Socket::getPortOfService (const char* service) throw (std::domain_error) {
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
         std::string error (_("Port '%1' neither numeric (decimal, octal or "
                              "hexadecimal) nor a service"));
         error.replace (error.find ("%1"), 2, service);
	 throwError (error, 0);
      }
   }

   return port;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads all available data into the buffer specified by input from
//            the previously specified source.
//Parameters: input: AByteArray receiving the input
//Returns   : int: Number of bytes read
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
int Socket::read (AByteArray& input) const throw (std::domain_error) {
   TRACE9 ("Socket::read (AByteArray&)" << " (" << sock << ')');

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
      TRACE9 ("Socket::read (AByteArray&) - error=" << errno << "; Bytes="
              << cRead);
      throwError (_("Error reading data"), errno);
   }

   TRACE5 ("Socket::read (AByteArray&) - read: " << input.data ());
   return input.length ();
}
/*--------------------------------------------------------------------------*/
//Purpose   : Reads all available data into the buffer specified by input from
//            the previously specified source.
//Parameters: input: string receiving the input
//Returns   : int: Number of bytes read
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
int Socket::read (std::string& input) const throw (std::domain_error) {
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

/*--------------------------------------------------------------------------*/
//Purpose   : Reads up to lenBuffer bytes data into the buffer specified by
//            pBuffer from the previously specified source.
//Parameters: pBuffer: Pointer to buffer receiving the input
//            lenBuffer: Length of that buffer
//Returns   : int: Number of bytes read
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
int Socket::read (char* pBuffer, unsigned int lenBuffer) const throw (std::domain_error) {
   TRACE9 ("Socket::read (const char*, int)" << " (" << sock << ')');

   ssize_t cRead (::read (sock, pBuffer, lenBuffer));
   if (cRead == -1)
      throwError (_("Error reading data"), errno);

   TRACE5 ("Socket::read (char*, int) - read: " << pBuffer);
   return cRead;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Waits until someone wants to create a (new) connection using the
//            the previously specified port.
//
//            The new (system-)socket for this communication is returned.
//
//            TECHNICAL NOTE: If a new connection is opened to a Socket,
//            it is actually "duplicated" and returned to enable multiple
//            connections to one port (socket)".
//Returns   : int: Socket over which to communicate
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
int Socket::waitForInput () const throw (std::domain_error) {
   TRACE9 ("Socket::waitForInput (Socket&) const - (" << sock << ')');

   struct sockaddr_in client;
   socklen_t size = sizeof (client);

   int newSocket (accept (sock, (struct sockaddr*)&client, &size));
   if (newSocket < 0)
      throwError (_("Error accepting connection"), errno);

   TRACE8 ("Socket::waitForInput (Socket&) const - assigning " << newSocket);
   return newSocket;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Specifies the server and port to write to (for outgoing
//            connections).
//Parameters: server: Name (or number) of the server to connect to
//            port: Portnumber to connect to
//Requieres : server not NULL
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
void Socket::writeTo (const char* server, unsigned int port) const throw (std::domain_error) {
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

/*--------------------------------------------------------------------------*/
//Purpose   : Writes lenBuff bytes of the buffer pointed to by pBuffer to the
//            socket (which must have been connected to an address).
//Parameters: pBuffer: Buffer to write
//            lenBuffer: Length of data to write
//Requieres : pBuffer not NULL
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
void Socket::write (const char* pBuffer, unsigned int lenBuffer) const
   throw (std::domain_error) {
   TRACE5 ("Socket::write (const char*, int) const - " << pBuffer << " (" << sock << ')');
   Check1 (pBuffer);

   if (::write (sock, pBuffer, lenBuffer) < 0)
      throwError (_("Error sending data"), errno);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Writes the contents of pBuffer up to the first zero-byte to the
//            socket (which must have been connected to an address).
//Parameters: pBuffer: Buffer to write
//Requieres : pBuffer not NULL
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
void Socket::write (const char* pBuffer) const throw (std::domain_error) {
   Check1 (pBuffer);
   TRACE9 ("Socket::write (const char*) const - (" << sock << ')');

   write (pBuffer, strlen (pBuffer));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Throws a domain_error with an explaining text.
//Parameters: error: Text describing the error
//            errNum: Number of error; if !=0 an explaining text is appended
//Throws    : std::domain_error in case of a communication error
/*--------------------------------------------------------------------------*/
void Socket::throwError (const std::string& error, int errNum) throw (std::domain_error) {
   std::string str (error);
   if (errNum) {
      str += ": ";
      str += strerror (errNum);
   }

   std::domain_error e (str);
   throw (e);
}
