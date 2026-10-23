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

#include <openssl/crypto.h>
#include <openssl/ssl.h>

#include "YGP/Check.h"
#include "YGP/Trace.h"

#include "YGP/ConnMgr.h"

using boost::asio::ip::tcp;

namespace YGP {

//-----------------------------------------------------------------------------
/// Default constructor
//-----------------------------------------------------------------------------
ConnectionMgr::ConnectionMgr()
    : ctx(), sslCtx(boost::asio::ssl::context::tls), key(), server(), acceptor(), connections() {
    TRACE9("ConnectionMgr::ConnectionMgr()");

    // Only TLS 1.3; the partners are authenticated by the password (not by certificates)
    SSL_CTX_set_min_proto_version(sslCtx.native_handle(), TLS1_3_VERSION);
    SSL_CTX_set_num_tickets(sslCtx.native_handle(), 0);
    SSL_CTX_set_session_cache_mode(sslCtx.native_handle(), SSL_SESS_CACHE_OFF);
    sslCtx.set_verify_mode(boost::asio::ssl::verify_none);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ConnectionMgr::~ConnectionMgr() {
    TRACE9("ConnectionMgr::~ConnectionMgr()");
    OPENSSL_cleanse(key.data(), key.size());
}

//-----------------------------------------------------------------------------
/// Sets the password the partners must know. It is used for connections
/// established afterwards.
/// \param password Password (might be empty)
//-----------------------------------------------------------------------------
void ConnectionMgr::setPassword(std::string_view password) {
    TRACE9("ConnectionMgr::setPassword(std::string_view)");
    key = Connection::deriveKey(password);
    haveKey = true;
}

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
/// Performs the TLS handshake and the authentication over the passed socket
/// \param socket Connected socket
/// \param type Role of this side of the connection
/// \returns std::unique_ptr<Connection> Established connection
/// \throws boost::system::system_error In case of an error (also if the
///     partner doesn't respond in time)
//----------------------------------------------------------------------------
std::unique_ptr<Connection> ConnectionMgr::secure(tcp::socket socket, Connection::Stream::handshake_type type) {
    if (!haveKey)
        setPassword("");

    auto connection(std::make_unique<Connection>(std::move(socket), sslCtx));
    Connection::Deadline deadline(*connection, timeout);
    try {
        connection->handshake(type, key);
    }
    catch (boost::system::system_error&) {
        if (deadline.expired())
            throw boost::system::system_error(make_error_code(ConnectionError::timeout));
        throw;
    }
    return connection;
}

//----------------------------------------------------------------------------
/// Connect to \c server on the specified \c port.
/// \param target Server to connect to
/// \param port Port the server is listening at (numeric or service name)
/// \throws boost::system::system_error In case of a connection error
//----------------------------------------------------------------------------
void ConnectionMgr::connectTo(const std::string& target, const std::string& port) {
    TRACE1("ConnectionMgr::connectTo(const std::string&, const std::string&) - " << target << ':' << port);

    tcp::socket sock(ctx);
    boost::asio::connect(sock, tcp::resolver(ctx).resolve(target, port));
    auto connection(secure(std::move(sock), Connection::Stream::client));

    changeMode(CLIENT);
    server = std::move(connection);
}

//----------------------------------------------------------------------------
/// Wait at port \c port for connections
/// \param port Port to listen at (numeric or service name)
/// \throws boost::system::system_error In case of a connection error
//----------------------------------------------------------------------------
void ConnectionMgr::listenAt(const std::string& port) {
    TRACE1("ConnectionMgr::listenAt(const std::string&) - " << port);

    if (!haveCertificate) {
        Connection::useEphemeralCertificate(sslCtx);
        haveCertificate = true;
    }

    auto endpoints(tcp::resolver(ctx).resolve(tcp::v4(), "", port, tcp::resolver::passive));
    auto listener(std::make_unique<tcp::acceptor>(ctx, endpoints.begin()->endpoint()));

    changeMode(SERVER);
    acceptor = std::move(listener);
}

//----------------------------------------------------------------------------
/// Stops listening for connections (keeping the connected clients)
//----------------------------------------------------------------------------
void ConnectionMgr::stopListening() {
    TRACE2("ConnectionMgr::stopListening()");
    acceptor.reset();
}

//----------------------------------------------------------------------------
/// Waits for a connection on the previously bound port
/// \returns std::unique_ptr<Connection> New (authenticated) connection (or
///    \c NULL in case of an error)
/// \pre listenAt() must have been called before
//----------------------------------------------------------------------------
std::unique_ptr<Connection> ConnectionMgr::getNewConnection() {
    TRACE2("ConnectionMgr::getNewConnection()");
    Check1(acceptor);
    Check1(mode == SERVER);

    try {
        return secure(acceptor->accept(), Connection::Stream::server);
    }
    catch (boost::system::system_error& error) {
        TRACE1("ConnectionMgr::getNewConnection() - Unexpected error: " << error.what());
        return nullptr;
    }
}

//----------------------------------------------------------------------------
/// Waits asynchronously for a connection on the previously bound port. The
/// handler is called while running the context (see getContext()); it is not
/// called if waiting is canceled.
/// \param handler Callback receiving the new (authenticated) connection (or
///    \c NULL in case of an error)
/// \pre listenAt() must have been called before
/// \remarks The TLS handshake and the authentication are performed
///    synchronously (but limited by the timeout) while running the context
//----------------------------------------------------------------------------
void ConnectionMgr::asyncGetNewConnection(NewConnectionHandler handler) {
    TRACE2("ConnectionMgr::asyncGetNewConnection(NewConnectionHandler)");
    Check1(acceptor);
    Check1(mode == SERVER);

    acceptor->async_accept([this, handler = std::move(handler)](const boost::system::error_code& ec, tcp::socket socket) {
        if (ec == boost::asio::error::operation_aborted)
            return;

        std::unique_ptr<Connection> connection;
        if (ec)
            TRACE1("ConnectionMgr::asyncGetNewConnection() - Unexpected error: " << ec.message());
        else
            try {
                connection = secure(std::move(socket), Connection::Stream::server);
            }
            catch (boost::system::system_error& error) {
                TRACE1("ConnectionMgr::asyncGetNewConnection() - Rejecting client: " << error.what());
            }
        handler(std::move(connection));
    });
}

//----------------------------------------------------------------------------
/// Cancels waiting for connections started with asyncGetNewConnection()
//----------------------------------------------------------------------------
void ConnectionMgr::cancelGetNewConnection() {
    TRACE2("ConnectionMgr::cancelGetNewConnection()");
    if (acceptor)
        acceptor->cancel();
}

//----------------------------------------------------------------------------
/// Adds a connection the the server connections
/// \param connection Connection (as returned by getNewConnection())
/// \returns Connection* Pointer to added connection (or \c NULL)
/// \pre
///    - listenAt() must have been called before
//----------------------------------------------------------------------------
Connection* ConnectionMgr::addConnection(std::unique_ptr<Connection> connection) {
    TRACE2("ConnectionMgr::addConnection(std::unique_ptr<Connection>)");
    Check1(mode == SERVER);

    if (!connection)
        return nullptr;

    connections.push_back(std::move(connection));
    return connections.back().get();
}

//-----------------------------------------------------------------------------
/// Disconnects one of the partners. A disconnected client keeps its entry
/// (set to NULL), so the positions of the other clients don't change.
/// \param partner Partner to disconnect
//-----------------------------------------------------------------------------
void ConnectionMgr::disconnect(const Connection* partner) {
    TRACE8("ConnectionMgr::disconnect(const Connection*)");
    Check1(mode != NONE);

    if (mode == SERVER) {
        auto i(std::find_if(connections.begin(), connections.end(), [partner](const auto& c) { return c.get() == partner; }));
        if (i == connections.end())
            return;

        i->reset();
        if (std::any_of(connections.begin(), connections.end(), [](const auto& c) { return c != nullptr; }))
            return;
    }
    server.reset();
    acceptor.reset();
    mode = NONE;
}

} // namespace YGP
