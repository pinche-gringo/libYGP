// PROJECT     : libYGP
// SUBSYSTEM   : RemoteDirSearch
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 27.3.2001
// COPYRIGHT   : Copyright (C) 2001 - 2004, 2006 - 2008, 2026

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

#ifdef _MSC_VER
#    pragma warning(disable : 4786) // disable warning about truncating debug info
#endif

#include <cctype>

#include <boost/asio/connect.hpp>

#include "YGP/ANumeric.h"
#include "YGP/AttrParse.h"
#include "YGP/Check.h"
#include "YGP/Exception.h"
#include "YGP/Internal.h"
#include "YGP/RemoteFile.h"
#include "YGP/RemoteMsg.h"
#include "YGP/Trace.h"

#include "YGP/RDirSrch.h"

namespace YGP {

const char RemoteDirSearch::SEPARATOR = ':';

//----------------------------------------------------------------------------
/// Constructor; sets the information for which files to search.
///
/// The port-part of the srch-parameter can be both numeric or its equivalent
/// textual representation (e.g.: \c 80 or \c http).
/// \param srch String identifying server/port and path to search;
///             in a format <tt>\<server\>:\<path\>:\<port\></tt>
/// \throw boost::system::system_error Containing error message in case of an error
//----------------------------------------------------------------------------
RemoteDirSearch::RemoteDirSearch(const std::string& srch)
    : IDirectorySearch(), ctx(), sock(ctx), pending(), server(), files(), attrs(), file(), time(), attr(0), size(0) {
    TRACE9("RemoteDirSearch::RemoteDirSearch(const std::string&) - " << srch << ':');

    size_t posPort(srch.rfind(SEPARATOR));
    Check1(posPort != std::string::npos);
    sendTo(srch.substr(0, posPort), srch.substr(posPort + 1));
}

//----------------------------------------------------------------------------
/// Constructor; sets the information for which files to search.
/// \param srch String identifying server and path to search; \<server\>:\<path\>
/// \param port Port number the server is listening at
/// \throw boost::system::system_error Containing error message in case of an error
//----------------------------------------------------------------------------
RemoteDirSearch::RemoteDirSearch(const std::string& srch, unsigned int port)
    : IDirectorySearch(), ctx(), sock(ctx), pending(), server(), files(), attrs(), file(), time(), attr(0), size(0) {
    TRACE9("RemoteDirSearch::RemoteDirSearch(const std::string&, unsigned int) - " << srch << ':' << port);

    sendTo(srch, port);
}

//----------------------------------------------------------------------------
/// Destructor.
//----------------------------------------------------------------------------
RemoteDirSearch::~RemoteDirSearch() {
    clearEntry();
    try {
        RemoteMsg::send(sock, "End");
    }
    catch (...) {
    }
}

//----------------------------------------------------------------------------
/// Specifies the partner (name and port) for the communication.
/// \param search String identifying server to connect to.
/// \param port Port the server is listening at (numeric or service name).
/// \throw boost::system::system_error With an error message in case of an error
//----------------------------------------------------------------------------
void RemoteDirSearch::sendTo(const std::string& search, const std::string& port) {
    TRACE6("RemoteDirSearch::sendTo(const std::string&, const std::string&) - " << search << ':' << port);

    setSearchValue(search);

    boost::asio::connect(sock, boost::asio::ip::tcp::resolver(ctx).resolve(server, port));
    pending.clear();

    TRACE9("RemoteDirSearch::sendTo(const std::string&, const std::string&) - Setting attribs");

    ATTRIBUTE(attrs, std::string, file, "File");
    ATTRIBUTE(attrs, unsigned long, size, "Size");
    ATTRIBUTE(attrs, unsigned long, attr, "Attr");
    ATTRIBUTE(attrs, ATimestamp, time, "Time");

    TRACE9("RemoteDirSearch::sendTo(const std::string&, const std::string&) - Finish");
}

//----------------------------------------------------------------------------
/// Retrieves the first file which matches the search-criteria.
/// \param pAnswer Character-buffer holding reponse from server
/// \pre: \c pAnswer must be a valid ASCIIZ-string
/// \throw YGP::FileError In case of an error
//----------------------------------------------------------------------------
const File* RemoteDirSearch::setFiledata(const char* pAnswer) {
    TRACE9("RemoteDirSearch::setFiledata(File&, const char*) - " << pAnswer);
    Check1(pAnswer);

    clearEntry();

    try {
        attrs.assignValues(pAnswer);
    }
    catch (YGP::ParseError& e) {
        throw(YGP::FileError(e.what()));
    }

    pEntry = new RemoteFile(sock, pending);

    // Set filename
    size_t posDirEnd(file.rfind(File::DIRSEPARATOR));
    if (posDirEnd != std::string::npos) {
        pEntry->path(file.substr(0, ++posDirEnd));
        pEntry->name(file.substr(posDirEnd));
    }
    else {
        pEntry->path("");
        pEntry->name(file);
    }
    file = "";

    TRACE9("RemoteDirSearch::setFiledata(File&, const char*) - " << pEntry->path() << pEntry->name());

    // Set size
    pEntry->size(size);
    TRACE9("RemoteDirSearch::setFiledata(File&, const char*) - Size=" << size);

    // Set filetime
    pEntry->time(time.toGMTTime());
    TRACE9("RemoteDirSearch::setFiledata(File&, const char*) - Time=" << time.toString());

    // Set attributes
    pEntry->attributes(IDirectorySearch::convertToSysAttribs(attr));
    TRACE9("RemoteDirSearch::setFiledata(File&, const char*) - Attr=" << attr);
    return pEntry;
}

//----------------------------------------------------------------------------
/// Searches for first file matching the input specified by the constructor(s)
/// or the set-methods.
///
/// Only files matching also the attributes are returned!
/// \param attribs Attributes of files to find
/// \returns const File* Pointer to found file-object or \c NULL
/// \throw YGP::FileError, boost::system::system_error Containing error message in case of an error
/// \pre \c searchDir, \c pEntry already set
//----------------------------------------------------------------------------
const File* RemoteDirSearch::find(unsigned long attribs) {
    TRACE9("RemoteDirSearch::find(unsigned long)");

    std::string buffer("Find=\"");
    buffer += files;
    buffer += "\";Attr=";

    ANumeric attrs(attribs);
    buffer += attrs.toUnformattedString();

    TRACE8("RemoteDirSearch::find(unsigned long) - Sending:\n\t" << buffer.length() << " bytes: " << buffer.data());
    RemoteMsg::send(sock, buffer);
    RemoteMsg::receive(sock, pending, buffer);

    TRACE8("RemoteDirSearch::find(unsigned long) - Read:\n\t" << buffer.length() << " bytes: " << buffer.data());

    if (isOK(buffer))
        return setFiledata(buffer.data() + 5);
    else
        handleServerError(buffer.data());
    return nullptr;
}

//----------------------------------------------------------------------------
/// Returns the next matching file according to the parameters specified in
/// earlier find-calls.
/// \return const File* Pointer to found file-object or NULL
/// \pre A find must have been (successfully) performed
/// \throw YGP::FileError, boost::system::system_error Containing error message in case of an error
//----------------------------------------------------------------------------
const File* RemoteDirSearch::next() {
    std::string buffer("Next");

    TRACE8("RemoteDirSearch::next() - Sending:\n\t" << buffer.length() << " bytes: " << buffer.data());
    RemoteMsg::send(sock, buffer);
    RemoteMsg::receive(sock, pending, buffer);

    TRACE8("RemoteDirSearch::next() - Read:\n\t" << buffer.length() << " bytes: " << buffer.data());

    if (isOK(buffer))
        return setFiledata(buffer.data() + 5);
    else
        handleServerError(buffer.data());
    return nullptr;
}

//----------------------------------------------------------------------------
/// Checks out the error sent by the server; if it sends an explaining string
/// this is thrown to inform the client.
/// \param pAnswer Response from the server
/// \return \c True if the remote directory does exist
/// \throw YGP::FileError Error reported by the server
//----------------------------------------------------------------------------
void RemoteDirSearch::handleServerError(const char* pAnswer) {
    int rc;
    std::string error;

    clearEntry();

    AttributeParse attrs;
    ATTRIBUTE(attrs, int, rc, "RC");
    ATTRIBUTE(attrs, std::string, error, "E");

    try {
        attrs.assignValues(pAnswer);
    }
    catch (YGP::ParseError& e) {
        throw(YGP::FileError(e.what()));
    }

    if (!error.empty()) {
        error = _("Server returned an error: ") + error;
        throw(YGP::FileError(error));
    }
}

//----------------------------------------------------------------------------
/// Returns the position of the separator-character between server name and
/// directory to analyze.
/// \param dir Directory whose validity should be checked.
/// \return int !0 if the remote directory does exist
//----------------------------------------------------------------------------
int RemoteDirSearch::posSeparator(const std::string& dir) const {
#if SYSTEM == UNIX
    int pos(dir.find(SEPARATOR));
#else
    // Search after drive-letter-seperator in Windoze, ...
    size_t pos((dir.length() < 3) ? std::string::npos : dir.find(File::DIRSEPARATOR, (dir[1] == SEPARATOR) ? 2 : 0));
#endif

    return pos;
}

//----------------------------------------------------------------------------
/// Checks if the remote directory does exist.
/// \return True if the remote directory does exist
//----------------------------------------------------------------------------
bool RemoteDirSearch::isOK(const std::string& answer) const {
    return (answer.length() > 3) && !strncmp(answer.data(), "RC=", 3) && (answer[3] == '0');
}

//-----------------------------------------------------------------------------
/// Checks if the directory-part of the object specifies an existing directory.
/// \return bool True if the directory exists
/// \throw boost::system::system_error in case of an error during the communication
//-----------------------------------------------------------------------------
bool RemoteDirSearch::isValid() const { return const_cast<RemoteDirSearch*>(this)->isValid(files); }

//----------------------------------------------------------------------------
/// Sets the files to search for (including server and path where to search).
/// The search-string must be in the format <tt>server:[path]files</tt>.
/// \param search Files to search for
//----------------------------------------------------------------------------
void RemoteDirSearch::setSearchValue(const std::string& search) {
    TRACE9("RemoteDirSearch::setSearchValue(const std::string& srch) - " << search);

    size_t len(search.find(SEPARATOR));
    Check1(len != std::string::npos);
    files = server = search;
    server.replace(len, server.length(), 0, '\0');
    files.replace(0, len + 1, 0, '\0');

    len = files.length() - 1;
    if (files[len] == File::DIRSEPARATOR) // Remove trailing seperators
        files.replace(len, 1, 0, '\0');

    TRACE5("RemoteDirSearch::setSearchValue(const std::string& srch) - " << "Server = " << server << "; Files = " << files);
}

//----------------------------------------------------------------------------
/// Retrieves the directory-part of the files to search for (including server).
/// \return std::string Directory to search for
//----------------------------------------------------------------------------
std::string RemoteDirSearch::getDirectory() const {
    std::string ret(server);
    ret += SEPARATOR;

    size_t pos(files.rfind(File::DIRSEPARATOR));
    if (pos != std::string::npos)
        ret += files.substr(0, pos + 1);

    return ret;
}

//----------------------------------------------------------------------------
/// Retrieves the name of the files to search.
/// \return std::string Files to find
//----------------------------------------------------------------------------
std::string RemoteDirSearch::getFileSpec() const { return files.substr(files.rfind(File::DIRSEPARATOR) + 1); }

//----------------------------------------------------------------------------
/// Checks if the passed string specifies an existing directory (on the server).
/// \param dir Directory (without server-part) whose validity should be checked.
/// \return bool True if the directory exists.
/// \throw boost::system::system_error in case of an error during the communication.
//----------------------------------------------------------------------------
bool RemoteDirSearch::isValid(const std::string& dir) {
    TRACE5("RemoteDirSearch::isValid(const std::string&) - " << dir);

    std::string write("Check=\"");
    write.append(dir, 0, dir.rfind(File::DIRSEPARATOR));
    write += '"';
    TRACE8("RemoteDirSearch::isValid(const std::string&) - Cmd = " << write);

    RemoteMsg::send(sock, write);

    std::string OK;
    RemoteMsg::receive(sock, pending, OK);
    TRACE6("RemoteDirSearch::isValid(const std::string&) - Answer: " << OK.data());

    return isOK(OK);
}

} // namespace YGP
