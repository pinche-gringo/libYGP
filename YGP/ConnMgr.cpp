//$Id: ConnMgr.cpp,v 1.6 2003/11/14 20:27:55 markus Exp $

//PROJECT     : Cardgames
//SUBSYSTEM   : <FILLIN>
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.07.2003
//COPYRIGHT   : Anticopyright (A) 2003

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


#include "YGP/Check.h"
#include "YGP/Trace_.h"
#include "YGP/Socket.h"

#include "YGP/ConnMgr.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
ConnectionMgr::ConnectionMgr () : mode (NONE), server (NULL) {
   TRACE9 ("ConnectionMgr::ConnectionMgr ()");
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ConnectionMgr::~ConnectionMgr () {
   TRACE9 ("ConnectionMgr::~ConnectionMgr ()");
   clearConnections ();
   delete server;
}


//-----------------------------------------------------------------------------
/// Removes the available connections
//-----------------------------------------------------------------------------
void ConnectionMgr::clearConnections () {
   TRACE6 ("ConnectionMgr::clearConnections ()");
   for (std::vector<Socket*>::iterator i (connections.begin ());
        i != connections.end (); ++i)
      delete *i;

   connections.clear ();
}

//-----------------------------------------------------------------------------
/// Changes the mode of the connection(s)
//-----------------------------------------------------------------------------
void ConnectionMgr::changeMode (modeConnect newMode) {
    if (mode != newMode) {
       TRACE3 ("ConnectionMgr::changeMode (modeConnect) - " << (int)newMode);
       clearConnections ();

       if (mode == SERVER) {
          delete server;
          server = NULL;
       }
       mode = newMode;
    }
}

//----------------------------------------------------------------------------
/// Connect to \c server on the specified \c port.
/// \param target: Server to connect to
/// \param port: Port the server is listening at
/// \throws std::domain_error: In case of a connection error
//----------------------------------------------------------------------------
void ConnectionMgr::connectTo (const char* target, unsigned int port)
    throw (std::domain_error) {
   TRACE1 ("ConnectionMgr::connectTo (const char*, unsinged int) - "
           << target << ':' << port);
   server = new Socket (target, port);
   changeMode (CLIENT);
}

//----------------------------------------------------------------------------
/// Wait at port \c port for connections
/// \param port: Port the server is listening at
/// \throws std::domain_error: In case of a connection error
//----------------------------------------------------------------------------
void ConnectionMgr::listenAt (unsigned int port) throw (std::domain_error) {
   TRACE1 ("ConnectionMgr::listenAt (unsinged int) - " << port);
   server = new Socket (port);
   changeMode (SERVER);
}

//----------------------------------------------------------------------------
/// Waits for a connection on the previously bound port
/// \pre listenAt() must have been called before
//----------------------------------------------------------------------------
int ConnectionMgr::getNewConnection () const {
   TRACE2 ("ConnectionMgr::getNewConnection ()");
   Check1 (server);
   Check1 (mode == SERVER);

   int socket (-1U);
   try {
       socket = server->waitForInput ();
   }
   catch (std::domain_error& e) {
      TRACE1 ("ConnectionMgr::getNewConnection () - Unexpected exception: "
              << e.what ());
   }
   return socket;
}

//----------------------------------------------------------------------------
/// Adds a connection the the server connections
/// \returns Socket*: Pointer to created socket (or \c NULL)
/// \pre
///    - listenAt() must have been called before
//----------------------------------------------------------------------------
Socket* ConnectionMgr::addConnection (int socket) {
   TRACE2 ("ConnectionMgr::addNewConnection (int) - " << socket);
   Check1 (server);
   Check1 (mode == SERVER);

   try {
       connections.push_back (new Socket (socket));
       return connections.back ();
   }
   catch (std::domain_error& e) {
      TRACE1 ("ConnectionMgr::addConnection (int) - Unexpected exception: "
              << e.what ());
      return NULL;
   }
}

}
