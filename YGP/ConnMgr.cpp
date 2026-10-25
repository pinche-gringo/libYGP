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
#include <boost/asio/post.hpp>

#include <openssl/crypto.h>
#include <openssl/ssl.h>

#include "YGP/Check.h"
#include "YGP/Trace.h"

#include "YGP/ConnMgr.h"

using boost::asio::ip::tcp;

namespace YGP {

namespace {

/// Maximal time a partner is blocked after failed authentications
constexpr std::chrono::seconds MAX_PENALTY{60};
/// Time after which the failed authentications of a partner are forgotten
constexpr std::chrono::minutes FORGET_PENALTY{10};
/// Maximal number of partners whose failed authentications are remembered
constexpr std::size_t MAX_PENALTIES{4096};

/// Performs the TLS handshake and the authentication over the passed connection
/// \param connection Connection to secure
/// \param type Role of this side of the connection
/// \param key Key derived from the password
/// \param timeout Time to wait for the partner
/// \throws boost::system::system_error In case of an error (also if the
///     partner doesn't respond in time)
void authenticate(Connection& connection, Connection::Stream::handshake_type type, const Connection::Key& key,
                  std::chrono::milliseconds timeout) {
    Connection::Deadline deadline(connection, timeout);
    try {
        connection.handshake(type, key);
    }
    catch (boost::system::system_error&) {
        if (deadline.expired())
            throw boost::system::system_error(make_error_code(ConnectionError::timeout));
        throw;
    }
}

} // namespace

//-----------------------------------------------------------------------------
/// Constructor
/// \param application Name of the application; it is used as part of the
///     salt of the password, so all partners must use the same name
//-----------------------------------------------------------------------------
ConnectionMgr::ConnectionMgr(std::string_view application)
    : ctx(), sslCtx(boost::asio::ssl::context::tls), application(application), key(), server(), acceptor(),
      connections(), handler(), handshakes(), penalties() {
    TRACE9("ConnectionMgr::ConnectionMgr(std::string_view) - " << application);

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
    cancelGetNewConnection();
    OPENSSL_cleanse(key.data(), key.size());
}

//-----------------------------------------------------------------------------
/// Sets the password the partners must know. It is used for connections
/// established afterwards.
/// \param password Password; an empty password removes a previously set one
///     (connecting and listening is not possible then)
//-----------------------------------------------------------------------------
void ConnectionMgr::setPassword(std::string_view password) {
    TRACE9("ConnectionMgr::setPassword(std::string_view)");
    if (password.empty()) {
        OPENSSL_cleanse(key.data(), key.size());
        haveKey = false;
    }
    else {
        key = Connection::deriveKey(password, application);
        haveKey = true;
    }
}

//-----------------------------------------------------------------------------
/// Checks if a password has been set
/// \throws boost::system::system_error If no password has been set
//-----------------------------------------------------------------------------
void ConnectionMgr::checkPassword() const {
    if (!haveKey)
        throw boost::system::system_error(make_error_code(ConnectionError::passwordMissing));
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
        cancelGetNewConnection();
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
/// \throws boost::system::system_error In case of a connection error (or if
///     no password has been set)
//----------------------------------------------------------------------------
void ConnectionMgr::connectTo(const std::string& target, const std::string& port) {
    TRACE1("ConnectionMgr::connectTo(const std::string&, const std::string&) - " << target << ':' << port);
    checkPassword();

    tcp::socket sock(ctx);
    boost::asio::connect(sock, tcp::resolver(ctx).resolve(target, port));
    auto connection(std::make_unique<Connection>(std::move(sock), sslCtx));
    authenticate(*connection, Connection::Stream::client, key, timeout);

    changeMode(CLIENT);
    server = std::move(connection);
}

//----------------------------------------------------------------------------
/// Wait at port \c port for connections
/// \param port Port to listen at (numeric or service name)
/// \param address Address to listen at; if empty, at all IPv4 addresses
/// \throws boost::system::system_error In case of a connection error (or if
///     no password has been set)
//----------------------------------------------------------------------------
void ConnectionMgr::listenAt(const std::string& port, const std::string& address) {
    TRACE1("ConnectionMgr::listenAt(const std::string&, const std::string&) - " << address << ':' << port);
    checkPassword();

    if (!haveCertificate) {
        Connection::useEphemeralCertificate(sslCtx);
        haveCertificate = true;
    }

    tcp::resolver resolver(ctx);
    auto endpoints(address.empty() ? resolver.resolve(tcp::v4(), "", port, tcp::resolver::passive)
                                   : resolver.resolve(address, port, tcp::resolver::passive));
    auto listener(std::make_unique<tcp::acceptor>(ctx, endpoints.begin()->endpoint()));

    changeMode(SERVER);
    acceptor = std::move(listener);
}

//----------------------------------------------------------------------------
/// Stops listening for connections (keeping the connected clients)
//----------------------------------------------------------------------------
void ConnectionMgr::stopListening() {
    TRACE2("ConnectionMgr::stopListening()");
    cancelGetNewConnection();
    acceptor.reset();
}

//----------------------------------------------------------------------------
/// Waits for a connection on the previously bound port. Handshakes with
/// other clients still running, when one has finished, are aborted.
/// \returns std::unique_ptr<Connection> New (authenticated) connection (or
///    \c NULL, if a client was rejected or accepting failed)
/// \pre listenAt() must have been called before
/// \remarks Runs the context (see getContext()) until a handshake has finished
//----------------------------------------------------------------------------
std::unique_ptr<Connection> ConnectionMgr::getNewConnection() {
    TRACE2("ConnectionMgr::getNewConnection()");
    Check1(acceptor);
    Check1(mode == SERVER);

    std::unique_ptr<Connection> connection;
    bool done(false);
    asyncGetNewConnection([&connection, &done](std::unique_ptr<Connection> newConnection) {
        connection = std::move(newConnection);
        done = true;
    });
    while (!done)
        if (!ctx.run_one())
            ctx.restart();

    cancelGetNewConnection();
    return connection;
}

//----------------------------------------------------------------------------
/// Waits asynchronously for connections on the previously bound port, until
/// cancelGetNewConnection() is called. The handler is called (while running
/// the context; see getContext()) for every client; it is not called if
/// waiting is canceled. If accepting connections fails, waiting stops (after
/// calling the handler with \c NULL).
/// \param handler Callback receiving the new (authenticated) connection (or
///    \c NULL, if a client was rejected or accepting failed)
/// \pre listenAt() must have been called before
/// \remarks If already waiting, only the handler is replaced
//----------------------------------------------------------------------------
void ConnectionMgr::asyncGetNewConnection(NewConnectionHandler handler) {
    TRACE2("ConnectionMgr::asyncGetNewConnection(NewConnectionHandler)");
    Check1(acceptor);
    Check1(mode == SERVER);

    this->handler = std::move(handler);
    if (!accepting) {
        accepting = true;
        acceptNext(++acceptGeneration);
    }
}

//----------------------------------------------------------------------------
/// Accepts the next client
/// \param generation Generation of waiting for clients; if it has changed
///    in the meantime, the result is ignored
//----------------------------------------------------------------------------
void ConnectionMgr::acceptNext(unsigned int generation) {
    acceptor->async_accept([this, generation](const boost::system::error_code& ec, tcp::socket socket) {
        if (!accepting || (generation != acceptGeneration))
            return;

        if (ec) {
            TRACE1("ConnectionMgr::acceptNext(unsigned int) - Unexpected error: " << ec.message());
            accepting = false;
            auto callback(handler);
            callback(nullptr);
            return;
        }

        startHandshake(std::move(socket));
        if (accepting && (generation == acceptGeneration))
            acceptNext(generation);
    });
}

//----------------------------------------------------------------------------
/// Starts the handshake with a new client in its own thread; the result is
/// passed to finishHandshake() (while running the context). Clients
/// exceeding the limits are rejected immediately.
/// \param socket Socket connected with the client
//----------------------------------------------------------------------------
void ConnectionMgr::startHandshake(tcp::socket socket) {
    boost::system::error_code ec;
    const auto endpoint(socket.remote_endpoint(ec));
    if (ec) {
        TRACE1("ConnectionMgr::startHandshake(tcp::socket) - Client vanished: " << ec.message());
        return;
    }

    const auto partner(endpoint.address());
    const auto pending(static_cast<std::size_t>(std::count_if(
        handshakes.begin(), handshakes.end(), [&partner](const Handshake& h) { return h.partner == partner; })));
    std::unique_ptr<Connection> connection;
    if ((handshakes.size() < MAX_HANDSHAKES) && (pending < MAX_HANDSHAKES_PER_PARTNER) && !isBlocked(partner))
        try {
            connection = std::make_unique<Connection>(std::move(socket), sslCtx);
        }
        catch (boost::system::system_error& error) {
            TRACE1("ConnectionMgr::startHandshake(tcp::socket) - Unexpected error: " << error.what());
        }

    if (connection) {
        Handshake& handshake(handshakes.emplace_back());
        handshake.id = ++lastHandshake;
        handshake.partner = partner;
        handshake.connection = std::move(connection);
        try {
            handshake.worker = std::jthread([this, &handshake, key = key, timeout = timeout]() mutable {
                try {
                    authenticate(*handshake.connection, Connection::Stream::server, key, timeout);
                }
                catch (boost::system::system_error& error) {
                    handshake.result = error.code();
                }
                catch (std::exception&) {
                    handshake.result = make_error_code(boost::system::errc::io_error);
                }
                OPENSSL_cleanse(key.data(), key.size());
                boost::asio::post(ctx, [this, id = handshake.id] { finishHandshake(id); });
            });
            return;
        }
        catch (std::system_error& error) {
            TRACE1("ConnectionMgr::startHandshake(tcp::socket) - Can't start thread: " << error.what());
            handshakes.pop_back();
        }
    }

    TRACE1("ConnectionMgr::startHandshake(tcp::socket) - Rejecting " << partner.to_string());
    auto callback(handler);
    callback(nullptr);
}

//----------------------------------------------------------------------------
/// Finishes the handshake with a client; passes the connection (or \c NULL
/// in case of an error) to the handler
/// \param id Identification of the handshake
//----------------------------------------------------------------------------
void ConnectionMgr::finishHandshake(unsigned long id) {
    auto i(std::find_if(handshakes.begin(), handshakes.end(), [id](const Handshake& h) { return h.id == id; }));
    if (i == handshakes.end()) // Aborted in the meantime
        return;

    i->worker.join();
    auto connection(std::move(i->connection));
    const auto result(i->result);
    const auto partner(i->partner);
    handshakes.erase(i);

    if (result) {
        TRACE1("ConnectionMgr::finishHandshake(unsigned long) - Rejecting " << partner.to_string() << ": "
                                                                            << result.message());
        if (result == ConnectionError::authenticationFailed)
            penalize(partner);
        connection.reset();
    }
    else
        penalties.erase(partner);

    if (accepting) {
        auto callback(handler);
        callback(std::move(connection));
    }
}

//----------------------------------------------------------------------------
/// Checks if the passed partner is blocked (because of failed authentications)
/// \param partner Partner to check
/// \returns bool True, if connections of the partner are rejected
//----------------------------------------------------------------------------
bool ConnectionMgr::isBlocked(const boost::asio::ip::address& partner) {
    auto i(penalties.find(partner));
    return (i != penalties.end()) && (std::chrono::steady_clock::now() < i->second.until);
}

//----------------------------------------------------------------------------
/// Blocks the passed partner (because of a failed authentication); the time
/// doubles with every failure
/// \param partner Partner to block
//----------------------------------------------------------------------------
void ConnectionMgr::penalize(const boost::asio::ip::address& partner) {
    const auto now(std::chrono::steady_clock::now());
    std::erase_if(penalties, [&now](const auto& p) { return (p.second.until + FORGET_PENALTY) < now; });
    if ((penalties.size() >= MAX_PENALTIES) && !penalties.contains(partner))
        penalties.erase(penalties.begin());

    auto& penalty(penalties[partner]);
    ++penalty.failures;
    penalty.until = now + std::min(std::chrono::seconds(1LL << std::min(penalty.failures - 1, 6U)), MAX_PENALTY);
}

//----------------------------------------------------------------------------
/// Stops waiting for connections started with asyncGetNewConnection();
/// running handshakes are aborted
//----------------------------------------------------------------------------
void ConnectionMgr::cancelGetNewConnection() {
    TRACE2("ConnectionMgr::cancelGetNewConnection()");
    accepting = false;
    ++acceptGeneration;
    handler = nullptr;
    if (acceptor) {
        boost::system::error_code ignore;
        acceptor->cancel(ignore);
    }

    for (auto& handshake : handshakes)
        handshake.connection->abort();
    handshakes.clear(); // Waits for the threads to finish
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
    cancelGetNewConnection();
    server.reset();
    acceptor.reset();
    mode = NONE;
}

} // namespace YGP
