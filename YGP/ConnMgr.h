#ifndef CONNMGR_H
#define CONNMGR_H

//$Id: ConnMgr.h,v 1.3 2003/07/27 03:44:42 markus Rel $

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

#include <stdexcept>

#include <string>
#include <vector>

// Forward declarations
class Socket;

/**Class to handle the connections an application can have.

   There are two different modes:
      - A \b client mode: The application has a server to which it talks.
      - A \b server mode: The application has (potentially) various clients it
        talks too.

   Of course an object can switch between the states, in which case the
   previously created connections are closed and removed.
*/
class ConnectionMgr {
 public:
   ConnectionMgr ();
   virtual ~ConnectionMgr ();

   /// Mode of the connections
   typedef enum { NONE,         ///< No action has been performed to set a mode
                  CLIENT,                ///< Manager has connected to a server
                  SERVER      ///< Manager waited for connections from clients
   } modeConnect;

   /// \name Client mode
   //@{
   /// Connect to \c server on the specified \c port.
   /// \throw std::domain_error
   void connectTo (const std::string& server, unsigned int port) throw (std::domain_error) {
       connectTo (server.c_str (), port);
   }
   void connectTo (const char* server, unsigned int port) throw (std::domain_error);

   //@}

   /// \name Server mode
   //@{
   void listenAt (unsigned int port) throw (std::domain_error);
   int  getNewConnection () const;
   Socket* addConnection (int socket);

   /// Returns the clients already connected to the server
   const std::vector<Socket*>& getClients () const { return connections; }
   //@}

   /// Returns the actual mode of the connection
   modeConnect getMode () const { return mode; }
   void changeMode (modeConnect);
   void clearConnections ();
   /// Returns the Socket over which to communicate (might be NULL)
   Socket* getSocket () const { return server; }

 private:
   modeConnect mode;
   Socket*              server;
   std::vector<Socket*> connections;

   ConnectionMgr (const ConnectionMgr& other);
   const ConnectionMgr& operator= (const ConnectionMgr& other);
};

#endif
