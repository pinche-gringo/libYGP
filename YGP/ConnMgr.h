#ifndef YGP_CONNMGR_H
#define YGP_CONNMGR_H

//$Id: ConnMgr.h,v 1.8 2008/05/18 13:21:27 markus Rel $

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
#include <vector>

#include <YGP/Exception.h>


namespace YGP {

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
   /// \throw YGP::CommError
   void connectTo (const std::string& server, unsigned int port) throw (YGP::CommError) {
       connectTo (server.c_str (), port);
   }
   void connectTo (const char* server, unsigned int port) throw (YGP::CommError);

   //@}

   /// \name Server mode
   //@{
   void listenAt (unsigned int port) throw (YGP::CommError);
   int  getNewConnection () const;
   Socket* addConnection (int socket);

   /// Returns the clients already connected to the server
   const std::vector<Socket*>& getClients () const { return connections; }
   //@}

   /// Returns the actual mode of the connection
   modeConnect getMode () const { return mode; }
   void changeMode (modeConnect);
   void disconnect (const Socket* partner);
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

}

#endif
