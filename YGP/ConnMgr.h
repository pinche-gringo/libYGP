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
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <boost/asio/io_context.hpp>
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
*/
class ConnectionMgr {
  public:
    ConnectionMgr();
    virtual ~ConnectionMgr();

    /// Mode of the connections
    using modeConnect = enum {
        NONE,   ///< No action has been performed to set a mode
        CLIENT, ///< Manager has connected to a server
        SERVER  ///< Manager waited for connections from clients
    };

    /// Default time to wait for the partner during establishing a connection
    static constexpr std::chrono::milliseconds DEFAULT_TIMEOUT{10'000};

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
    /// Wait at port \c port for connections
    /// \throw boost::system::system_error
    void listenAt(unsigned int port) { listenAt(std::to_string(port)); }
    void listenAt(const std::string& port);
    void stopListening();
    std::unique_ptr<Connection> getNewConnection();
    /// Callback receiving a new (authenticated) connection (or \c NULL in case of an error)
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
    std::unique_ptr<Connection> secure(boost::asio::ip::tcp::socket socket, Connection::Stream::handshake_type type);

    boost::asio::io_context ctx;
    boost::asio::ssl::context sslCtx;
    bool haveCertificate{false}; ///< Flag, if sslCtx has a certificate (needed for server mode)
    Connection::Key key;
    bool haveKey{false}; ///< Flag, if key has been set (it's derived from an empty password otherwise)
    std::chrono::milliseconds timeout{DEFAULT_TIMEOUT};
    modeConnect mode{NONE};
    std::unique_ptr<Connection> server;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::vector<std::unique_ptr<Connection>> connections;

    ConnectionMgr(const ConnectionMgr& other) = delete;
    const ConnectionMgr& operator=(const ConnectionMgr& other) = delete;
};

} // namespace YGP

#endif
