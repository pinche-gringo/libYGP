//$Id: Socket.cpp,v 1.5 2001/08/22 01:30:57 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Socket
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
//AUTHOR      : Markus Schwab
//CREATED     : 24.3.2001
//COPYRIGHT   : Anticopyright (A) 2001

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

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <assert.h>

#define DEBUG 0
#include "Trace_.h"
#include "AByteArray.h"

#include "Socket.h"

 
/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
Socket::Socket () throw (domain_error)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket ()");

   if (sock < 0)
      throwError ("Can't create socket", errno);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: port: Port to listen at
/*--------------------------------------------------------------------------*/
Socket::Socket (unsigned int port) throw (domain_error)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket (unsigned int)");

   if (sock < 0)
      throwError ("Can't create socket", errno);
   listenAt (port);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: host: Name (or number) of host to send to
//            port: Port to write to
/*--------------------------------------------------------------------------*/
Socket::Socket (const char* host, unsigned int port) throw (domain_error)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket (const char*, unsigned int)");
   assert (host);

   if (sock < 0)
      throwError ("Can't create socket", errno);

   writeTo (host, port);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: host: Name (or number) of host to send to
//            port: Port to write to
/*--------------------------------------------------------------------------*/
Socket::Socket (const std::string& host, unsigned int port) throw (domain_error)
   : sock (socket (PF_INET, SOCK_STREAM, 0)) {
   TRACE9 ("Socket::Socket (const std::string&, unsigned int)");

   if (sock < 0)
      throwError ("Can't create socket", errno);

   writeTo (host.c_str (), port);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
Socket::~Socket () {
   TRACE9 ("Socket::~Socket ()");
   ::close (sock);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: rhs: Socket to assign
//Returns   : *this: Reference to self
/*--------------------------------------------------------------------------*/
Socket& Socket::operator= (const Socket& rhs) throw (domain_error) {
   if (&rhs != this) {
      ::close (sock);
      sock = socket (PF_INET, SOCK_STREAM, 0);
      if (sock < 0)
	 throwError ("Can't create socket", errno);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: rhs: Socket to assign
//Returns   : *this: Reference to self
/*--------------------------------------------------------------------------*/
Socket& Socket::operator= (int socket) {
   close (sock);
   sock = socket;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Specifies the port to listen at (for read)
//Parameters: port: Port at which to listen
/*--------------------------------------------------------------------------*/
void Socket::listenAt (unsigned int port) throw (domain_error) {
   TRACE9 ("Socket::listenAt (unsigned int) - " << port << " (" << sock << ')');

   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port = htons (port);
   addr.sin_addr.s_addr = htonl (INADDR_ANY);

   if (::bind (sock, (struct sockaddr*)&addr, sizeof (addr)) < 0)
      throwError ("Can't bind socket", errno);

   if (::listen (sock, 1) < 0)
      throwError ("Can't listen on socket", 0);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Gets the portnumber for a service (or a text-number)
//Parameters: service: Service describing port
/*--------------------------------------------------------------------------*/
unsigned int Socket::getPortOfService (const char* service) throw (domain_error) {
   TRACE9 ("Socket::getPortOfService (const char*)");

   char* pTail = NULL;
   TRACE8 ("Passed service: " << service);

   errno = 0;
   int port = strtol (service, &pTail, 0);
   if (errno || (pTail && *pTail)) {
      struct servent* pServent = getservbyname (service, "tcp");

      if (pServent)
	 port = ntohs (pServent->s_port);
      else
	 throwError ("Port neither numeric (decimal, octal or hexadecimal)"
		     " nor a service", 0);
   }

   return port;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads from a socket
//Parameters: port: Port to listen at
//            input: AByteArray receiving the input
//Returns   : int: Number of bytes actually read
/*--------------------------------------------------------------------------*/
int Socket::read (AByteArray& input) const throw (domain_error) {
   TRACE9 ("Socket::read (AByteArray&)" << " (" << sock << ')');

   char buffer[80] = "";
   ssize_t cRead;
   input = "";

   // Read from socket til either error or buffer not completely filled
   while ((cRead = ::read (sock, buffer, sizeof (buffer))) >= 0) {
      input.append (buffer, cRead);
      if (cRead < sizeof (buffer))
         break;
   }

   if (cRead < 0) {
      TRACE9 ("Socket::read (AByteArray&) - error=" << errno << "; Bytes="
              << cRead);
      throwError ("Error reading data", errno);
   }

   TRACE5 ("Socket::read (AByteArray&) - read: " << input.data ());
   return input.length ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads from a socket
//Parameters: pBuffer: Buffer receiving the input
//            lenBuffer: Length of that buffer
//Returns   : int: Number of bytes actually read
/*--------------------------------------------------------------------------*/
int Socket::read (char* pBuffer, int lenBuffer) const throw (domain_error) {
   TRACE9 ("Socket::read (const char*, int)" << " (" << sock << ')');

   ssize_t cRead (::read (sock, pBuffer, lenBuffer));
   if (cRead < 0)
      throwError ("Error reading data", errno);

   TRACE5 ("Socket::read (char*, int) - read: " << pBuffer);
   return cRead;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Waits (and accepts) for input (from a client)
//Returns   : int: Socket over which to communicate  (with the client)
/*--------------------------------------------------------------------------*/
int Socket::waitForInput () const throw (domain_error) {
   TRACE9 ("Socket::waitForInput (Socket&) const - (" << sock << ')');

   struct sockaddr_in client;
   socklen_t size = sizeof (client);

   int newSocket (accept (sock, (struct sockaddr*)&client, &size));
   if (newSocket < 0)
      throwError ("Error accepting connection", errno);

   TRACE8 ("Socket::waitForInput (Socket&) const - assigning " << newSocket);
   return newSocket;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Connects this (socket) to an address
//Parameters: host: Name (or number) of host to connect to
//            port: Portnumber to connect to
/*--------------------------------------------------------------------------*/
void Socket::writeTo (const char* host, unsigned int port) throw (domain_error) {
   TRACE9 ("Socket::writeTo (const char*, unsigned int)");
   assert (host);

   struct sockaddr_in name;
   name.sin_family = AF_INET;
   name.sin_port = htons (port);
   
   struct hostent* hostinfo = gethostbyname (host);
   if (!hostinfo) {
      std::string error ("Can't resolve name ");
      error += host;
      throwError (error, 0);
   }

   name.sin_addr = *(struct in_addr*)hostinfo->h_addr;

   if (connect (sock, (struct sockaddr*)&name, sizeof (name)) < 0)
      throwError ("Can't connect to server", errno);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Writes a buffer to a socket
//Parameters: pBuffer: Buffer to write
//            lenBuffer: Length of data to write
/*--------------------------------------------------------------------------*/
void Socket::write (const char* pBuffer, int lenBuffer) const throw (domain_error) {
   TRACE5 ("Socket::write (const char*, int) const - " << pBuffer << " (" << sock << ')');
   assert (pBuffer);

   if (::write (sock, pBuffer, lenBuffer) < 0)
      throwError ("Error sending data", errno);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Writes a buffer to a socket
//Parameters: pBuffer: Buffer to write
/*--------------------------------------------------------------------------*/
void Socket::write (const char* pBuffer) const throw (domain_error) {
   assert (pBuffer);
   TRACE9 ("Socket::write (const char*) const - (" << sock << ')');

   write (pBuffer, strlen (pBuffer));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Throws a domain_error
//Parameters: error: Text describing the error
//            errNum: Number of error; if !=0 an explaining text is appended
//Requires  : error must be an ASCIIZ-string
/*--------------------------------------------------------------------------*/
void Socket::throwError (const std::string& error, int errNum) throw (domain_error) {
   std::string str (error);
   if (errNum) {
      str += ": ";
      str += strerror (errNum);
   }

   domain_error e (error);
   throw (e);
}
