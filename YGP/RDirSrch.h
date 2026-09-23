#ifndef YGP_RDIRSRCH_H
#define YGP_RDIRSRCH_H

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

#include <cstddef>
#include <cstring>
#include <ctime>
#include <string>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <YGP/ATStamp.h>
#include <YGP/AttrParse.h>
#include <YGP/Exception.h>
#include <ygp-cfg.h>

#include <YGP/IDirSrch.h>

namespace YGP {

/**Class to search for files in a directory over a
   network-connection. This search can be restricted to files matching
   certain name-criterias or by attributes.

   The name-part of the files to search supports UNIX-like wildcards;
   that are the asterisk (*) for any number of any characters, the
   question-mark for any single character and a set of characters in
   brackets (([) and (])). This set can contain a list of characters
   (like [abcde]) or a region (like [a-e]). To invert this set use a
   leading caret (^) or a leading exclamation mark (!), like
   ([^a-e]).

   The found (and matching) files are retrieved by objects of type
   File.

   Note: The class does not do any word expansion for the search-path
         (like expanding the tilde (~) to the home-directory)!
*/
class RemoteDirSearch : public IDirectorySearch {
  public:
    //@Section manager-functions
    RemoteDirSearch() : IDirectorySearch(), ctx(), sock(ctx), pending(), server(), files(), attrs(), file(), time(), attr(0), size(0) {}
    RemoteDirSearch(const std::string& search);
    RemoteDirSearch(const std::string& search, unsigned int port);
    ~RemoteDirSearch() override;

    //@Section initializing
    /// Specifies the partner (name and port) for the communication.
    void sendTo(const std::string& server, unsigned int port) { sendTo(server, std::to_string(port)); }
    void sendTo(const std::string& server, const std::string& port);

    //@Section manipulating
    void setSearchValue(const std::string& search) override;
    std::string getDirectory() const override;
    std::string getFileSpec() const override;

    /// \name Searching
    //@{
    const File* find(unsigned long attribs = FILE_NORMAL) override;
    const File* next() override;
    //@}

    bool isValid() const override;
    bool isValid(const std::string& dir);

    static const char SEPARATOR; ///< Separator between host and port

  protected:
    boost::asio::io_context ctx;        ///< Context for the I/O operations
    boost::asio::ip::tcp::socket sock; ///< Socket for the communication
    std::string pending;                ///< Data received, but not yet processed

  private:
    /// \name Section prohibited manager functions
    //@{
    RemoteDirSearch(const RemoteDirSearch&) = delete;
    RemoteDirSearch& operator=(const RemoteDirSearch&) = delete;
    //@}

    int posSeparator(const std::string& dir) const;

    bool isOK(const std::string& answer) const;
    const File* setFiledata(const char* pAnswer);
    void handleServerError(const char* pAnswer);

    std::string server;
    std::string files;

    // Variables for receiving
    AttributeParse attrs;
    std::string file;
    ATimestamp time;
    unsigned long attr;
    unsigned long size;
};

} // namespace YGP

#endif // RDIRSRCH_H
