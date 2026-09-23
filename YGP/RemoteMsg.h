#ifndef YGP_REMOTEMSG_H
#define YGP_REMOTEMSG_H

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

// Internal header (not installed): Exchanges the messages between
// RemoteDirSearch/RemoteFile and RemoteDirSearchSrv.

#include <array>
#include <string>
#include <string_view>

#include <boost/asio/error.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

namespace YGP::RemoteMsg {

/// Character terminating every message
inline constexpr char END = '\0';

//----------------------------------------------------------------------------
/// Sends \c msg (followed by the terminator) over \c sock
/// \param sock Socket to send to
/// \param msg Message to send; must not contain the terminator
/// \throw boost::system::system_error in case of a communication error
//----------------------------------------------------------------------------
inline void send(boost::asio::ip::tcp::socket& sock, std::string_view msg) {
    std::array buffers{boost::asio::buffer(msg), boost::asio::buffer(&END, 1)};
    boost::asio::write(sock, buffers);
}

//----------------------------------------------------------------------------
/// Receives the next message from \c sock
/// \param sock Socket to read from
/// \param pending Data read from \c sock but not yet processed; must be kept
///    for the next call
/// \param msg Received message (without terminator)
/// \returns bool False, if the partner closed the connection
/// \throw boost::system::system_error in case of a communication error
//----------------------------------------------------------------------------
inline bool receive(boost::asio::ip::tcp::socket& sock, std::string& pending, std::string& msg) {
    boost::system::error_code ec;
    std::size_t len(boost::asio::read_until(sock, boost::asio::dynamic_buffer(pending), END, ec));
    if (ec == boost::asio::error::eof) {
        msg.clear();
        return false;
    }
    if (ec)
        throw boost::system::system_error(ec);

    msg.assign(pending, 0, len - 1);
    pending.erase(0, len);
    return true;
}

} // namespace YGP::RemoteMsg

#endif
