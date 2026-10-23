// PROJECT     : libYGP
// SUBSYSTEM   : YGP/Connection
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

#include <algorithm>
#include <condition_variable>
#include <memory>

#ifdef _WIN32
#    include <winsock2.h>
#    define SHUT_RDWR SD_BOTH
#else
#    include <sys/socket.h>
#endif

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

#include "YGP/Internal.h"
#include "YGP/Trace.h"

#include "YGP/Connection.h"

namespace YGP {

namespace {

/// Label to export the keying material of the TLS session with
constexpr std::string_view EXPORT_LABEL("EXPORTER-libYGP-password-proof");
/// Salt to derive the key from the password with
constexpr std::string_view SALT("libYGP connection password");
/// Number of iterations to derive the key from the password
constexpr int KDF_ITERATIONS(200'000);

// The destructor of error_category is (intentionally) not virtual; categories are never deleted polymorphically
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
/// Category of the connection errors
class ConnectionCategory : public boost::system::error_category {
  public:
    const char* name() const noexcept override { return "YGP::Connection"; }
    std::string message(int ev) const override {
        switch (static_cast<ConnectionError>(ev)) {
        case ConnectionError::authenticationFailed:
            return _("Authentication failed (wrong password?)");
        case ConnectionError::timeout:
            return _("The partner did not respond in time");
        case ConnectionError::tooMuchData:
            return _("The partner sent too much data");
        }
        return _("Unknown error");
    }
};
#pragma GCC diagnostic pop

/// Throws the passed connection error
/// \param error Error to throw
[[noreturn]] void throwError(ConnectionError error) { throw boost::system::system_error(make_error_code(error)); }

/// Throws an exception describing the last OpenSSL error
/// \param what Description of the failed action
[[noreturn]] void throwSSLError(const char* what) {
    throw boost::system::system_error(
        boost::system::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()), what);
}

/// Calculates the proof of one partner, that it knows the key
/// \param key Key derived from the password
/// \param material Keying material exported from the TLS session
/// \param role Role of the partner ('C'lient or 'S'erver)
/// \returns Connection::Key Proof
Connection::Key proof(const Connection::Key& key, const Connection::Key& material, char role) {
    std::array<unsigned char, std::tuple_size_v<Connection::Key> + 1> data;
    std::copy(material.begin(), material.end(), data.begin());
    data.back() = static_cast<unsigned char>(role);

    Connection::Key result;
    std::size_t length(0);
    if (!EVP_Q_mac(nullptr, "HMAC", nullptr, "SHA256", nullptr, key.data(), key.size(), data.data(), data.size(),
                   result.data(), result.size(), &length) ||
        (length != result.size()))
        throwSSLError("HMAC");
    return result;
}

} // namespace

//-----------------------------------------------------------------------------
/// Returns the category of the connection errors
/// \returns const boost::system::error_category& Category
//-----------------------------------------------------------------------------
const boost::system::error_category& connectionCategory() {
    static const ConnectionCategory category;
    return category;
}

//-----------------------------------------------------------------------------
/// Constructor
/// \param socket Connected socket
/// \param ctx TLS context to use for the connection
//-----------------------------------------------------------------------------
Connection::Connection(Socket socket, boost::asio::ssl::context& ctx) : stream(std::move(socket), ctx), mx() {
    TRACE9("Connection::Connection(Socket, ssl::context&)");
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
Connection::~Connection() { TRACE9("Connection::~Connection()"); }

//-----------------------------------------------------------------------------
/// Performs the TLS handshake and the mutual proof of the password. The
/// client proofs first, so the server doesn't reveal anything to clients not
/// knowing the password.
/// \param type Role of this side of the connection
/// \param key Key derived from the password (see deriveKey())
/// \throw boost::system::system_error In case of an error (with the code
///     ConnectionError::authenticationFailed, if the proof was wrong)
//-----------------------------------------------------------------------------
void Connection::handshake(Stream::handshake_type type, const Key& key) {
    TRACE3("Connection::handshake(handshake_type, const Key&) - " << ((type == Stream::client) ? "Client" : "Server"));
    std::lock_guard lock(mx);
    stream.handshake(type);

    Key material;
    if (SSL_export_keying_material(stream.native_handle(), material.data(), material.size(), EXPORT_LABEL.data(),
                                   EXPORT_LABEL.size(), nullptr, 0, 0) != 1)
        throwSSLError("SSL_export_keying_material");

    const Key own(proof(key, material, (type == Stream::client) ? 'C' : 'S'));
    const Key expected(proof(key, material, (type == Stream::client) ? 'S' : 'C'));
    Key received;
    if (type == Stream::client) {
        boost::asio::write(stream, boost::asio::buffer(own));

        // The server closes the connection, if the proof is wrong
        boost::system::error_code error;
        boost::asio::read(stream, boost::asio::buffer(received), error);
        if ((error == boost::asio::error::eof) || (error == boost::asio::ssl::error::stream_truncated))
            throwError(ConnectionError::authenticationFailed);
        else if (error)
            throw boost::system::system_error(error);

        if (CRYPTO_memcmp(received.data(), expected.data(), received.size()))
            throwError(ConnectionError::authenticationFailed);
    }
    else {
        boost::asio::read(stream, boost::asio::buffer(received));
        if (CRYPTO_memcmp(received.data(), expected.data(), received.size()))
            throwError(ConnectionError::authenticationFailed);

        boost::asio::write(stream, boost::asio::buffer(own));
    }
    TRACE5("Connection::handshake(handshake_type, const Key&) - Authenticated");
}

//-----------------------------------------------------------------------------
/// Writes (blocking) the passed data
/// \param data Data to write
/// \throw boost::system::system_error In case of an error
//-----------------------------------------------------------------------------
void Connection::write(std::string_view data) {
    std::lock_guard lock(mx);
    boost::asio::write(stream, boost::asio::buffer(data));
}

//-----------------------------------------------------------------------------
/// Reads (blocking) some data
/// \param buffer Buffer to read into
/// \param length Length of the buffer
/// \returns std::size_t Number of bytes read (at least 1)
/// \throw boost::system::system_error In case of an error (with the code
///     boost::asio::error::eof if the partner closed the connection)
/// \remarks Only to use, if no other thread is waiting for data of this
///     connection, as the mutex is held during the whole operation
//-----------------------------------------------------------------------------
std::size_t Connection::read(char* buffer, std::size_t length) {
    std::lock_guard lock(mx);
    return stream.read_some(boost::asio::buffer(buffer, length));
}

//-----------------------------------------------------------------------------
/// Reads all data available without blocking and appends it to the passed
/// buffer
/// \param buffer Buffer to append the data to
/// \param max Maximal size of the buffer; reading stops, if it is reached
/// \param error Error reading the data
/// \returns bool True, if all available data has been read; false if there
///     might be more (because the buffer has reached its maximal size)
/// \remarks As some data might be buffered by TLS, call this method also
///     before waiting for the socket to become readable
//-----------------------------------------------------------------------------
bool Connection::readAvailable(std::string& buffer, std::size_t max, boost::system::error_code& error) {
    std::lock_guard lock(mx);
    error.clear();
    getSocket().non_blocking(true, error);
    if (error)
        return true;

    std::array<char, 4096> data;
    while (buffer.size() < max) {
        const std::size_t read(stream.read_some(boost::asio::buffer(data.data(), std::min(data.size(), max - buffer.size())), error));
        buffer.append(data.data(), read);
        if (error)
            break;
    }

    const bool drained(error == boost::asio::error::would_block);
    if (drained)
        error.clear();

    boost::system::error_code ignore;
    getSocket().non_blocking(false, ignore);
    return drained || error;
}

//-----------------------------------------------------------------------------
/// Derives the key used to proof the knowledge of the password
/// \param password Password
/// \returns Key Derived key
//-----------------------------------------------------------------------------
Connection::Key Connection::deriveKey(std::string_view password) {
    Key key;
    if (!PKCS5_PBKDF2_HMAC(password.data(), static_cast<int>(password.size()),
                           reinterpret_cast<const unsigned char*>(SALT.data()), static_cast<int>(SALT.size()),
                           KDF_ITERATIONS, EVP_sha256(), static_cast<int>(key.size()), key.data()))
        throwSSLError("PKCS5_PBKDF2_HMAC");
    return key;
}

//-----------------------------------------------------------------------------
/// Creates a new (self-signed) certificate and its key and uses it for the
/// passed context. The certificate is only valid for this process; as it is
/// not verified by the partner anyway, there is no need to store it.
/// \param ctx Context to use the certificate for
/// \throw boost::system::system_error In case of an error
//-----------------------------------------------------------------------------
void Connection::useEphemeralCertificate(boost::asio::ssl::context& ctx) {
    TRACE3("Connection::useEphemeralCertificate(ssl::context&)");
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> key(EVP_PKEY_Q_keygen(nullptr, nullptr, "ED25519"), EVP_PKEY_free);
    if (!key)
        throwSSLError("EVP_PKEY_Q_keygen");

    std::unique_ptr<X509, decltype(&X509_free)> cert(X509_new(), X509_free);
    if (!cert)
        throwSSLError("X509_new");

    X509_NAME* name(X509_get_subject_name(cert.get()));
    if (!X509_set_version(cert.get(), X509_VERSION_3) || !ASN1_INTEGER_set(X509_get_serialNumber(cert.get()), 1) ||
        !X509_gmtime_adj(X509_getm_notBefore(cert.get()), -60 * 60) ||
        !X509_gmtime_adj(X509_getm_notAfter(cert.get()), 60L * 60 * 24 * 365) || !X509_set_pubkey(cert.get(), key.get()) ||
        !X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, reinterpret_cast<const unsigned char*>("libYGP"), -1, -1, 0) ||
        !X509_set_issuer_name(cert.get(), name) || !X509_sign(cert.get(), key.get(), nullptr))
        throwSSLError("Creating certificate");

    if ((SSL_CTX_use_certificate(ctx.native_handle(), cert.get()) != 1) ||
        (SSL_CTX_use_PrivateKey(ctx.native_handle(), key.get()) != 1))
        throwSSLError("Using certificate");
}

//-----------------------------------------------------------------------------
/// Constructor; starts the timer
/// \param connection Connection to abort after the timeout
/// \param timeout Time after which the connection is aborted
//-----------------------------------------------------------------------------
Connection::Deadline::Deadline(Connection& connection, std::chrono::milliseconds timeout)
    : watchdog([this, fd = connection.getSocket().native_handle(), timeout](std::stop_token stop) {
          std::mutex mx;
          std::condition_variable_any cv;
          std::unique_lock lock(mx);
          if (!cv.wait_for(lock, stop, timeout, [&stop] { return stop.stop_requested(); })) {
              TRACE1("Connection::Deadline - Timeout; aborting connection");
              timedOut = true;
              // Shutting the socket down (instead of closing it) is safe while another thread uses it
              ::shutdown(fd, SHUT_RDWR);
          }
      }) {}

//-----------------------------------------------------------------------------
/// Destructor; stops the timer
//-----------------------------------------------------------------------------
Connection::Deadline::~Deadline() = default;

} // namespace YGP
