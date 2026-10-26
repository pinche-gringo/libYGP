#ifndef YGP_CONNECTION_H
#define YGP_CONNECTION_H

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

#include <array>
#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/system/error_code.hpp>

namespace YGP {

/// Errors specific to connections
enum class ConnectionError {
    authenticationFailed = 1, ///< The partner doesn't know the password
    timeout,                  ///< The partner didn't respond in time
    tooMuchData,              ///< The partner sent more data than allowed
    passwordMissing           ///< No password has been set
};

const boost::system::error_category& connectionCategory();

/// Creates an error code for a connection error
/// \param error Error
/// \returns boost::system::error_code Error code
inline boost::system::error_code make_error_code(ConnectionError error) {
    return {static_cast<int>(error), connectionCategory()};
}

/**Connection to a partner, secured with TLS (version 1.3 only).

   The certificates of the partners are not verified (there is no authority to
   verify them); instead both partners prove to each other that they know the
   same password. The proof is bound to the TLS session, so it can't be
   relayed by a man in the middle.

   The key proving the knowledge of the password is derived from it with
   PBKDF2, salted with a random value the client chooses for every connection
   (and the name of the application). So work spent attacking the password
   can't be precomputed or reused for other connections.

   \note Someone pretending to be the server (or sitting in between) obtains
   the proof of the client, which allows an offline attack on the password of
   this one connection. The stretching makes this expensive, but the password
   should not be trivial nevertheless.

   A TLS stream must not be used by several threads at once. Therefore all
   reading and writing methods lock the mutex of the connection. A thread
   waiting for data must not block while holding it; it should wait (e.g.
   with getSocket().async_wait()) for the socket to become readable and then
   read the available data with readAvailable().
*/
class Connection {
  public:
    using Socket = boost::asio::ip::tcp::socket;  ///< Type of the underlying socket
    using Stream = boost::asio::ssl::stream<Socket>; ///< Type of the TLS stream
    using Key = std::array<unsigned char, 32>;   ///< Key derived from the password
    using Salt = std::array<unsigned char, 16>;  ///< Random salt of the key (chosen per connection)

    Connection(Socket socket, boost::asio::ssl::context& ctx);
    ~Connection();

    void handshake(Stream::handshake_type type, std::string_view password, std::string_view application = {});

    void write(std::string_view data);
    std::size_t read(char* buffer, std::size_t length);
    bool readAvailable(std::string& buffer, std::size_t max, boost::system::error_code& error);

    /// Returns the underlying socket (e.g. to wait for data or to cancel
    /// waiting); use it not for reading or writing
    Socket& getSocket() { return stream.next_layer(); }

    void abort();

    static Key deriveKey(std::string_view password, const Salt& salt, std::string_view application = {});
    static void useEphemeralCertificate(boost::asio::ssl::context& ctx);

    /**Class aborting the connection, if it is not destroyed in a given time.

       It protects a (blocking) operation (like a handshake) against partners
       not responding; the operation fails then (with some I/O error).
    */
    class Deadline {
      public:
        Deadline(Connection& connection, std::chrono::milliseconds timeout);
        ~Deadline();

        /// Checks if the time has passed (and the connection has been aborted)
        bool expired() const { return timedOut; }

      private:
        std::atomic<bool> timedOut{false};
        std::jthread watchdog;

        Deadline(const Deadline& other) = delete;
        const Deadline& operator=(const Deadline& other) = delete;
    };

  private:
    Stream stream;
    std::mutex mx;

    Connection(const Connection& other) = delete;
    const Connection& operator=(const Connection& other) = delete;
};

} // namespace YGP

namespace boost::system {
template <> struct is_error_code_enum<YGP::ConnectionError> : std::true_type {};
} // namespace boost::system

#endif
