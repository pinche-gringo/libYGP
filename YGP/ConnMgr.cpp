//$Id: ConnMgr.cpp,v 1.14 2008/06/11 17:53:40 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : YGP/ConnectionManager
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.14 $
//AUTHOR      : Markus Schwab
//CREATED     : 23.07.2003
//COPYRIGHT   : Copyright (C) 2003, 2004, 2006, 2008

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


#include <algorithm>

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Socket.h"

#include "YGP/ConnMgr.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
ConnectionMgr::ConnectionMgr () : mode (NONE), server (NULL), connections () {
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
/// \param target Server to connect to
/// \param port Port the server is listening at
/// \throws YGP::CommError In case of a connection error
//----------------------------------------------------------------------------
void ConnectionMgr::connectTo (const char* target, unsigned int port) throw (YGP::CommError) {
   TRACE1 ("ConnectionMgr::connectTo (const char*, unsinged int) - "
           << target << ':' << port);
   server = new Socket (target, port);
   changeMode (CLIENT);
}

//----------------------------------------------------------------------------
/// Wait at port \c port for connections
/// \param port Port the server is listening at
/// \throws YGP::CommError In case of a connection error
//----------------------------------------------------------------------------
void ConnectionMgr::listenAt (unsigned int port) throw (YGP::CommError) {
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
   catch (YGP::CommError& e) {
      TRACE1 ("ConnectionMgr::getNewConnection () - Unexpected exception: "
              << e.what ());
   }
   return socket;
}

//----------------------------------------------------------------------------
/// Adds a connection the the server connections
/// \returns Socket* Pointer to created socket (or \c NULL)
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
   catch (YGP::CommError& e) {
      TRACE1 ("ConnectionMgr::addConnection (int) - Unexpected exception: " << e.what ());
      return NULL;
   }
}

//-----------------------------------------------------------------------------
/// Disconnects one of the partners
/// \param partner Partner to disconnect
//-----------------------------------------------------------------------------
void ConnectionMgr::disconnect (const Socket* partner) {
   TRACE8 ("ConnectionMgr::disconnect (const Socket*)");
   Check1 (mode == NONE);

   if (mode == SERVER) {
      std::vector<Socket*>::iterator i (find (connections.begin (), connections.end (), partner));
      if (i == connections.end ())
	 return;

      connections.erase (i);
      if (connections.size ())
	 return;
   }
   delete server;
   server = NULL;
   mode = NONE;
}


}
