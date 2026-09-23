// PROJECT     : libYGP
// SUBSYSTEM   : YGP/ConnectionManager
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 23.07.2003
// COPYRIGHT   : Copyright (C) 2003, 2004, 2006, 2008, 2026

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

#include <boost/asio/connect.hpp>

#include "YGP/Check.h"
#include "YGP/Trace.h"

#include "YGP/ConnMgr.h"

using boost::asio::ip::tcp;

namespace YGP {

//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
ConnectionMgr::ConnectionMgr() : ctx(), server(), acceptor(), connections() { TRACE9("ConnectionMgr::ConnectionMgr()"); }

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ConnectionMgr::~ConnectionMgr() { TRACE9("ConnectionMgr::~ConnectionMgr()"); }

//-----------------------------------------------------------------------------
/// Removes the available connections
//-----------------------------------------------------------------------------
void ConnectionMgr::clearConnections() {
    TRACE6("ConnectionMgr::clearConnections()");
    connections.clear();
}

//-----------------------------------------------------------------------------
/// Changes the mode of the connection(s)
//-----------------------------------------------------------------------------
void ConnectionMgr::changeMode(modeConnect newMode) {
    if (mode != newMode) {
        TRACE3("ConnectionMgr::changeMode(modeConnect) - " << (int)newMode);
        clearConnections();
        server.reset();
        acceptor.reset();
        mode = newMode;
    }
}

//----------------------------------------------------------------------------
/// Connect to \c server on the specified \c port.
/// \param target Server to connect to
/// \param port Port the server is listening at (numeric or service name)
/// \throws boost::system::system_error In case of a connection error
//----------------------------------------------------------------------------
void ConnectionMgr::connectTo(const std::string& target, const std::string& port) {
    TRACE1("ConnectionMgr::connectTo(const std::string&, const std::string&) - " << target << ':' << port);

    auto sock(std::make_unique<tcp::socket>(ctx));
    boost::asio::connect(*sock, tcp::resolver(ctx).resolve(target, port));

    changeMode(CLIENT);
    server = std::move(sock);
}

//----------------------------------------------------------------------------
/// Wait at port \c port for connections
/// \param port Port to listen at (numeric or service name)
/// \throws boost::system::system_error In case of a connection error
//----------------------------------------------------------------------------
void ConnectionMgr::listenAt(const std::string& port) {
    TRACE1("ConnectionMgr::listenAt(const std::string&) - " << port);

    auto endpoints(tcp::resolver(ctx).resolve(tcp::v4(), "", port, tcp::resolver::passive));
    auto listener(std::make_unique<tcp::acceptor>(ctx, endpoints.begin()->endpoint()));

    changeMode(SERVER);
    acceptor = std::move(listener);
}

//----------------------------------------------------------------------------
/// Waits for a connection on the previously bound port
/// \returns std::unique_ptr<tcp::socket> Socket of the new connection (or
///    \c NULL in case of an error)
/// \pre listenAt() must have been called before
//----------------------------------------------------------------------------
std::unique_ptr<tcp::socket> ConnectionMgr::getNewConnection() const {
    TRACE2("ConnectionMgr::getNewConnection()");
    Check1(acceptor);
    Check1(mode == SERVER);

    boost::system::error_code ec;
    auto socket(std::make_unique<tcp::socket>(acceptor->accept(ec)));
    if (ec) {
        TRACE1("ConnectionMgr::getNewConnection() - Unexpected error: " << ec.message());
        socket.reset();
    }
    return socket;
}

//----------------------------------------------------------------------------
/// Adds a connection the the server connections
/// \param socket Socket of the connection (as returned by getNewConnection())
/// \returns tcp::socket* Pointer to added socket (or \c NULL)
/// \pre
///    - listenAt() must have been called before
//----------------------------------------------------------------------------
tcp::socket* ConnectionMgr::addConnection(std::unique_ptr<tcp::socket> socket) {
    TRACE2("ConnectionMgr::addConnection(std::unique_ptr<tcp::socket>)");
    Check1(acceptor);
    Check1(mode == SERVER);

    if (!socket)
        return nullptr;

    connections.push_back(std::move(socket));
    return connections.back().get();
}

//-----------------------------------------------------------------------------
/// Disconnects one of the partners
/// \param partner Partner to disconnect
//-----------------------------------------------------------------------------
void ConnectionMgr::disconnect(const tcp::socket* partner) {
    TRACE8("ConnectionMgr::disconnect(const tcp::socket*)");
    Check1(mode != NONE);

    if (mode == SERVER) {
        auto i(std::find_if(connections.begin(), connections.end(), [partner](const auto& c) { return c.get() == partner; }));
        if (i == connections.end())
            return;

        connections.erase(i);
        if (connections.size())
            return;
    }
    server.reset();
    acceptor.reset();
    mode = NONE;
}

} // namespace YGP
