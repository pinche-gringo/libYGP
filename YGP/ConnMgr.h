#ifndef YGP_CONNMGR_H
#define YGP_CONNMGR_H

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

#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>

#include <YGP/Connection.h>

namespace YGP {

/**Class to handle the connections an application can have.

   There are two different modes:
      - A \b client mode: The application has a server to which it talks.
      - A \b server mode: The application has (potentially) various clients it
        talks too.

   Of course an object can switch between the states, in which case the
   previously created connections are closed and removed.

   All connections are secured with TLS; the partners must use the same
   password (see setPassword() and Connection).

   In server mode the handshakes of new clients are performed in parallel (in
   own threads), so clients not responding don't block others. To protect
   against a single host occupying the server, the number of handshakes per
   partner is limited and partners failing to authenticate are blocked for
   some (increasing) time.

   The object is not thread-safe; use it only in the thread running its
   context (see getContext()).
*/
class ConnectionMgr {
  public:
    explicit ConnectionMgr(std::string_view application = {});
    virtual ~ConnectionMgr();

    /// Mode of the connections
    using modeConnect = enum {
        NONE,   ///< No action has been performed to set a mode
        CLIENT, ///< Manager has connected to a server
        SERVER  ///< Manager waited for connections from clients
    };

    /// Default time to wait for the partner during establishing a connection
    static constexpr std::chrono::milliseconds DEFAULT_TIMEOUT{10'000};
    /// Maximal number of handshakes performed at once
    static constexpr std::size_t MAX_HANDSHAKES{16};
    /// Maximal number of handshakes performed at once with the same partner
    static constexpr std::size_t MAX_HANDSHAKES_PER_PARTNER{2};

    void setPassword(std::string_view password);
    /// Sets the time to wait for the partner while establishing a connection
    /// \param timeout Time to wait
    void setTimeout(std::chrono::milliseconds timeout) { this->timeout = timeout; }
    /// Returns the time to wait for the partner while establishing a connection
    std::chrono::milliseconds getTimeout() const { return timeout; }

    /// \name Client mode
    //@{
    /// Connect to \c server on the specified \c port.
    /// \throw boost::system::system_error
    void connectTo(const std::string& server, unsigned int port) { connectTo(server, std::to_string(port)); }
    void connectTo(const std::string& server, const std::string& port);

    //@}

    /// \name Server mode
    //@{
    /// Wait at port \c port (of the passed address, or of all IPv4 addresses)
    /// for connections
    /// \throw boost::system::system_error
    void listenAt(unsigned int port, const std::string& address = {}) { listenAt(std::to_string(port), address); }
    void listenAt(const std::string& port, const std::string& address = {});
    void stopListening();
    std::unique_ptr<Connection> getNewConnection();
    /// Callback receiving a new (authenticated) connection (or \c NULL, if a
    /// client was rejected or accepting failed)
    using NewConnectionHandler = std::function<void(std::unique_ptr<Connection>)>;
    void asyncGetNewConnection(NewConnectionHandler handler);
    void cancelGetNewConnection();
    Connection* addConnection(std::unique_ptr<Connection> connection);

    /// Returns the clients already connected to the server. Clients keep
    /// their position, so the entries of disconnected clients are NULL.
    const std::vector<std::unique_ptr<Connection>>& getClients() const { return connections; }
    //@}

    /// Returns the actual mode of the connection
    modeConnect getMode() const { return mode; }
    void changeMode(modeConnect);
    void disconnect(const Connection* partner);
    void clearConnections();
    /// Returns the connection to the server (in client mode; might be NULL)
    Connection* getServer() const { return server.get(); }
    /// Returns the context of the managed sockets (e.g. to run asynchronous
    /// operations on them)
    boost::asio::io_context& getContext() { return ctx; }

  private:
    /// Handshake with a new client, performed in its own thread
    struct Handshake {
        unsigned long id{0};                   ///< Identification of the handshake
        boost::asio::ip::address partner{};    ///< Address of the client
        std::unique_ptr<Connection> connection{}; ///< Connection to the client
        boost::system::error_code result{};    ///< Result of the handshake (set by the thread)
        std::jthread worker{};                 ///< Thread performing the handshake
    };
    /// Failed authentications of a partner
    struct Penalty {
        unsigned int failures{0};                   ///< Number of failed authentications
        std::chrono::steady_clock::time_point until{}; ///< Time until which the partner is blocked
    };

    void checkPassword() const;
    void acceptNext(unsigned int generation);
    void startHandshake(boost::asio::ip::tcp::socket socket);
    void finishHandshake(unsigned long id);
    bool isBlocked(const boost::asio::ip::address& partner);
    void penalize(const boost::asio::ip::address& partner);

    boost::asio::io_context ctx;
    boost::asio::ssl::context sslCtx;
    bool haveCertificate{false}; ///< Flag, if sslCtx has a certificate (needed for server mode)
    std::string application;     ///< Name of the application (part of the salt of the password)
    Connection::Key key;
    bool haveKey{false}; ///< Flag, if a password has been set
    std::chrono::milliseconds timeout{DEFAULT_TIMEOUT};
    modeConnect mode{NONE};
    std::unique_ptr<Connection> server;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::vector<std::unique_ptr<Connection>> connections;

    NewConnectionHandler handler;    ///< Callback for new connections
    bool accepting{false};           ///< Flag, if waiting for connections
    unsigned int acceptGeneration{0}; ///< Incremented, when waiting is stopped (to ignore pending accepts)
    unsigned long lastHandshake{0};  ///< Identification of the last started handshake
    std::list<Handshake> handshakes; ///< Running handshakes
    std::map<boost::asio::ip::address, Penalty> penalties; ///< Partners which failed to authenticate

    ConnectionMgr(const ConnectionMgr& other) = delete;
    const ConnectionMgr& operator=(const ConnectionMgr& other) = delete;
};

} // namespace YGP

#endif
