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

#include <memory>
#include <string>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace YGP {

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
    ConnectionMgr();
    virtual ~ConnectionMgr();

    /// Mode of the connections
    using modeConnect = enum {
        NONE,   ///< No action has been performed to set a mode
        CLIENT, ///< Manager has connected to a server
        SERVER  ///< Manager waited for connections from clients
    };

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
    std::unique_ptr<boost::asio::ip::tcp::socket> getNewConnection() const;
    boost::asio::ip::tcp::socket* addConnection(std::unique_ptr<boost::asio::ip::tcp::socket> socket);

    /// Returns the clients already connected to the server
    const std::vector<std::unique_ptr<boost::asio::ip::tcp::socket>>& getClients() const { return connections; }
    //@}

    /// Returns the actual mode of the connection
    modeConnect getMode() const { return mode; }
    void changeMode(modeConnect);
    void disconnect(const boost::asio::ip::tcp::socket* partner);
    void clearConnections();
    /// Returns the socket over which to communicate with the server (in
    /// client mode; might be NULL)
    boost::asio::ip::tcp::socket* getSocket() const { return server.get(); }

  private:
    boost::asio::io_context ctx;
    modeConnect mode{NONE};
    std::unique_ptr<boost::asio::ip::tcp::socket> server;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::vector<std::unique_ptr<boost::asio::ip::tcp::socket>> connections;

    ConnectionMgr(const ConnectionMgr& other) = delete;
    const ConnectionMgr& operator=(const ConnectionMgr& other) = delete;
};

} // namespace YGP

#endif
