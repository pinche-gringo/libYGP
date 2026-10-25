// PROJECT     : libYGP
// SUBSYSTEM   : Test/ConnMgr
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 23.10.2026
// COPYRIGHT   : Copyright (C) 2026

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
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <boost/asio/connect.hpp>

#include <YGP/ConnMgr.h>

#include "Test.h"

using namespace std::chrono_literals;

namespace {

const char* PORT("47113");

/// Accepts (in another thread) the next connection of the server
std::future<std::unique_ptr<YGP::Connection>> accept(YGP::ConnectionMgr& server) {
    return std::async(std::launch::async, [&server] { return server.getNewConnection(); });
}

/// Connects a socket (not sending anything) to the server
void connectSilent(boost::asio::ip::tcp::socket& socket) {
    boost::asio::connect(socket, boost::asio::ip::tcp::resolver(socket.get_executor()).resolve("127.0.0.1", PORT));
}

} // namespace

int main() {
    unsigned int cErrors(0);

    std::cout << "Testing ConnectionMgr ...\n";
    try {
        YGP::ConnectionMgr server;
        server.setPassword("secret");
        server.setTimeout(500ms);
        server.listenAt(PORT);

        // Partners knowing the password can exchange data
        {
            auto pending(accept(server));
            YGP::ConnectionMgr client;
            client.setPassword("secret");
            client.connectTo("127.0.0.1", PORT);
            check(client.getMode() == YGP::ConnectionMgr::CLIENT);
            check(client.getServer());

            auto conn(pending.get());
            check(conn);
            if (conn && client.getServer()) {
                client.getServer()->write("Hello");

                std::string received;
                boost::system::error_code error;
                const auto end(std::chrono::steady_clock::now() + 2s);
                while ((received.size() < 5) && !error && (std::chrono::steady_clock::now() < end))
                    conn->readAvailable(received, 1024, error);
                check(!error);
                check(received == "Hello");

                // Reading stops at the maximal size
                client.getServer()->write("0123456789");
                received.clear();
                while ((received.size() < 4) && !error)
                    conn->readAvailable(received, 4, error);
                check(received == "0123");
            }
        }

        // A client not responding doesn't block others
        {
            boost::asio::io_context ctx;
            boost::asio::ip::tcp::socket silent(ctx);
            connectSilent(silent);

            auto pending(accept(server));
            YGP::ConnectionMgr client;
            client.setPassword("secret");
            client.connectTo("127.0.0.1", PORT);
            check(client.getServer());
            check(pending.get());
        }

        // A client not responding is rejected after the timeout
        {
            auto pending(accept(server));
            boost::asio::io_context ctx;
            boost::asio::ip::tcp::socket silent(ctx);
            connectSilent(silent);

            const auto start(std::chrono::steady_clock::now());
            check(!pending.get());
            check((std::chrono::steady_clock::now() - start) < 5s);
        }

        // A wrong password is rejected on both sides
        {
            auto pending(accept(server));
            YGP::ConnectionMgr client;
            client.setPassword("wrong");
            try {
                client.connectTo("127.0.0.1", PORT);
                check(!"Connected with wrong password");
            }
            catch (boost::system::system_error& e) {
                check(e.code() == YGP::ConnectionError::authenticationFailed);
            }
            check(client.getMode() == YGP::ConnectionMgr::NONE);
            check(!pending.get());
        }

        // After a failed authentication the partner is blocked for some time
        {
            auto pending(accept(server));
            YGP::ConnectionMgr client;
            client.setPassword("secret");
            try {
                client.connectTo("127.0.0.1", PORT);
                check(!"Connected while blocked");
            }
            catch (boost::system::system_error&) {
            }
            check(!pending.get());
        }
        std::this_thread::sleep_for(1100ms);

        // The name of the application is part of the password
        {
            auto pending(accept(server));
            YGP::ConnectionMgr client("Other application");
            client.setPassword("secret");
            try {
                client.connectTo("127.0.0.1", PORT);
                check(!"Connected with other application");
            }
            catch (boost::system::system_error& e) {
                check(e.code() == YGP::ConnectionError::authenticationFailed);
            }
            check(!pending.get());
        }
    }
    catch (std::exception& e) {
        std::cerr << "Test: Error: " << e.what() << '\n';
        ++cErrors;
    }

    // Connecting or listening requires a password
    {
        YGP::ConnectionMgr mgr;
        try {
            mgr.connectTo("127.0.0.1", PORT);
            check(!"Connected without password");
        }
        catch (boost::system::system_error& e) {
            check(e.code() == YGP::ConnectionError::passwordMissing);
        }

        mgr.setPassword("secret");
        mgr.setPassword("");
        try {
            mgr.listenAt(PORT);
            check(!"Listening without password");
        }
        catch (boost::system::system_error& e) {
            check(e.code() == YGP::ConnectionError::passwordMissing);
        }
    }

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
