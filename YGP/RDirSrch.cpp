//$Id: RDirSrch.cpp,v 1.12 2001/10/09 17:20:05 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RemoteDirSearch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.12 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.3.2001
//COPYRIGHT   : Anticopyright (A) 2001

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


#include <ctype.h>

#define DEBUG 0
#include "Trace_.h"

#include "ANumeric.h"
#include "AttrParse.h"
#include "AByteArray.h"
#include "RemoteFile.h"

#include "RDirSrch.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: srch: String identifying server, directory to search and port
//                  The string must be in the format:
//                      <server>:<path>:<port>
/*--------------------------------------------------------------------------*/
RemoteDirSearch::RemoteDirSearch (const std::string& srch) throw (domain_error)
   : IDirectorySearch (), sock () {
   TRACE9 ("RemoteDirSearch::RemoteDirSearch (const std::string&) - "
           << srch << ':');

   int posPort (srch.rfind (SEPERATOR));
   std::string sPort (srch);
   sPort.replace (0, posPort, 0, '\0');

   server = srch;
   server.replace (posPort, server.length (), 0, '\0');

   init (server, Socket::getPortOfService (sPort.c_str ()));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: server: String identifying server, directory to search and port
//                    The string must be in the format:
//                       <server>:<path>
//            port: Portnumber the server is listening at
/*--------------------------------------------------------------------------*/
RemoteDirSearch::RemoteDirSearch (const std::string& srch, unsigned int port)
   throw (domain_error) : IDirectorySearch (), sock () {
   TRACE9 ("RemoteDirSearch::RemoteDirSearch (const std::string&, unsigned int) - "
           << srch << ':' << port);

   init (srch, port);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
RemoteDirSearch::~RemoteDirSearch () {
   clearEntry ();
   sock.write ("End", 3);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Initializes the object
//Parameters: server: Name of the server to connect to
//            port: Number of port the server is 
//Requires  : pAnswer valid ASCIIZ-string
/*--------------------------------------------------------------------------*/
void RemoteDirSearch::init (const std::string& search, unsigned int port)
   throw (domain_error) {
   TRACE6 ("RemoteDirSearch::init (const std::string&, unsigned int) - "
           << search << ':' << port);

   setSearchValue (search);

   sock.writeTo (server, port);

   TRACE9 ("RemoteDirSearch::init (const std::string&, unsigned int) - Setting attribs");

   ATTRIBUTE (attrs, std::string, file, "File");
   ATTRIBUTE (attrs, unsigned long, size, "Size");
   ATTRIBUTE (attrs, unsigned long, attr, "Attr");
   ATTRIBUTE (attrs, ATimestamp, time, "Time");

   TRACE9 ("RemoteDirSearch::init (const std::string&, unsigned int) - Finish");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the first file which matches the search-criteria
//Parameters: pAnswer: Character-buffer holding reponse from server
//Requires  : pAnswer valid ASCIIZ-string
/*--------------------------------------------------------------------------*/
const File* RemoteDirSearch::setFiledata (const char* pAnswer) throw (std::string) {
   TRACE9 ("RemoteDirSearch::setFiledata (File&, const char*) - "
           << pAnswer);

   assert (pAnswer);
   clearEntry ();

   attrs.assignValues (pAnswer);

   pEntry = new RemoteFile (sock);

   // Set filename
   unsigned int posDirEnd (file.rfind (File::DIRSEPERATOR));
   if (posDirEnd != std::string::npos) {
      pEntry->path (file.substr (0, ++posDirEnd));
      pEntry->name (file.substr (posDirEnd));
   }
   else {
      pEntry->path ("");
      pEntry->name (file);
   }
   file = "";

   TRACE9 ("RemoteDirSearch::setFiledata (File&, const char*) - "
           << pEntry->path () << pEntry->name ());

   // Set size
   pEntry->size (size);
   TRACE9 ("RemoteDirSearch::setFiledata (File&, const char*) - Size=" << size);

   // Set filetime
   pEntry->time (time.toGMTTime ());
   TRACE9 ("RemoteDirSearch::setFiledata (File&, const char*) - Time="
	   << time.toString ());

   // Set attributes
   pEntry->attributes (IDirectorySearch::convertToSysAttribs (attr));
   TRACE9 ("RemoteDirSearch::setFiledata (File&, const char*) - Attr=" << attr);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the first file which matches the search-criteria
//Parameters: res: Buffer where to place the result
//            attribs: Attributes of files to find
//Returns   : const File*: Pointer to found file-obect
//Requires  : searchDir, pEntry  already set
/*--------------------------------------------------------------------------*/
const File* RemoteDirSearch::find (unsigned long attribs) throw (std::string) {
   TRACE9 ("RemoteDirSearch::find (unsigned long)");

   AByteArray buffer ("Find=\"");
   buffer += files;
   buffer += "\";Attr=";
   
   ANumeric attrs (attribs);
   buffer += attrs.toUnformatedString ();
   buffer += '\0';

   TRACE8 ("RemoteDirSearch::find (unsigned long) - Sending:\n\t"
           << buffer.length () << " bytes: " << buffer.data ());
   try {
      sock.write (buffer);
      sock.read (buffer);
   }
   catch (domain_error& error) {
      std::string err (error.what ());
      throw err;
   }
   buffer += '\0';
   TRACE8 ("RemoteDirSearch::find (unsigned long) - Read:\n\t"
           << buffer.length () << " bytes: " << buffer.data ());

   if (isOK (buffer))
      return setFiledata (buffer.data () + 5);
   else
      handleServerError (buffer.data ());
   return NULL;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the next file which matches the search-criteria
//Returns   : Status; 0: OK
//Returns   : const File*: Pointer to found file-obect
//Notes     : In case of an (unexpected) error it is thrown for the caller
/*--------------------------------------------------------------------------*/
const File* RemoteDirSearch::next () throw (std::string) {
   AByteArray buffer ("Next");

   TRACE8 ("RemoteDirSearch::next () - Sending:\n\t"
           << buffer.length () << " bytes: " << buffer.data ());
   try {
      sock.write (buffer);
      sock.read (buffer);
   }
   catch (domain_error& error) {
      std::string err (error.what ());
      throw err;
   }
   buffer += '\0';
   TRACE8 ("RemoteDirSearch::next () - Read:\n\t"
           << buffer.length () << " bytes: " << buffer.data ());

   if (isOK (buffer))
      return setFiledata (buffer.data () + 5);
   else
      handleServerError (buffer.data ());
   return NULL;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks out the error send by the server; if it sends an
//            explaining string this is thrown to inform the client
//            name and directory to analyze
//Parameters: pAnswer: Response from the server
//Returns   : True if the remote directory does exist
/*--------------------------------------------------------------------------*/
void RemoteDirSearch::handleServerError (const char* pAnswer) throw (std::string) {
   int rc;
   std::string error;

   clearEntry ();

   AttributeParse attrs;
   ATTRIBUTE (attrs, int, rc, "RC");
   ATTRIBUTE (attrs, std::string, error, "E");

   attrs.assignValues (pAnswer);

   if (!error.empty ()) {
      error = "Server returned an error: " + error;
      throw (error);
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the position of the seperator-character between server-
//            name and directory to analyze
//Parameters: dir: Directory whose validity should be checked
//Returns   : True if the remote directory does exis
/*--------------------------------------------------------------------------*/
int RemoteDirSearch::posSeperator (const std::string& dir) const {
#if SYSTEM == UNIX
   int pos (dir.find (SEPERATOR));
#else
   // Search after drive-letter-seperator in Windoze, ...
   int pos ((dir.length < 3) ? std::string::npos
	    : (dir.find (SEPERATOR, 2) == std::string::npos
	       ? std::string::npos : dir.find (File::DIRSEPERATOR, pos)));
#endif

   return pos;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the searchDir is really a direcory
//Parameters: dir: Directory (without server-part) whose validity should be
//                 checked
//Returns   : bool: True if the directory exists
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isValid (const std::string& dir) throw (domain_error) {
   TRACE5 ("RemoteDirSearch::isValid (const std::string&) - " << dir);

   std::string write ("Check=\"");
   write.append (dir, 0, dir.rfind (File::DIRSEPERATOR));
   write += '"';
   TRACE8 ("RemoteDirSearch::isValid (const std::string&) - Cmd = "
           << write);

   sock.write (write.data (), write.length ());

   AByteArray OK (10);
   sock.read (OK);
   TRACE6 ("RemoteDirSearch::isValid (const std::string&) - Answer: "
           << OK.data ());

   return isOK (OK);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the remote directory does exist
//Returns   : True if the remote directory does exis
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isValid () const throw (domain_error) {
   return const_cast<RemoteDirSearch*> (this)->isValid (files);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the remote directory does exist
//Returns   : True if the remote directory does exis
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isOK (const AByteArray& answer) const {
   return (answer.length () > 3)
           && !strncmp (answer.data (), "RC=", 3) && (answer[3] == '0');
}

/*--------------------------------------------------------------------------*/
//Purpose   : Splits the search-string in its server-, directory- and filepart;
//            it prepares the internal data also for a new search
//Parameters: search: Files to find
/*--------------------------------------------------------------------------*/
void RemoteDirSearch::setSearchValue (const std::string& search) {
   TRACE9 ("RemoteDirSearch::setSearchValue (const std::string& srch) - "
           << search);

   int len (search.find (SEPERATOR)); assert (len != std::string::npos);
   files = server = search;
   server.replace (len, server.length (), 0, '\0');
   files.replace (0, len + 1, 0, '\0');

   len = files.length () - 1;
   if (files[len] == File::DIRSEPERATOR)     // Remove trailing seperators
      files.replace (len, 1, 0, '\0');

   TRACE5 ("RemoteDirSearch::setSearchValue (const std::string& srch) - "
           << "Server = " << server << "; Files = " << files);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the directory-part of search-string (including server)
//Returns   : std::string: Directory to search
/*--------------------------------------------------------------------------*/
std::string RemoteDirSearch::getDirectory () const {
   std::string ret (server);
   ret += SEPERATOR;

   int pos (files.rfind (File::DIRSEPERATOR));
   if (pos != std::string::npos)
      ret += files.substr (0, pos + 1);

   return ret;
 }

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the directory-part of search-string (including server)
//Returns   : std::string: Files to find
/*--------------------------------------------------------------------------*/
std::string RemoteDirSearch::getFileSpec () const {
   return files.substr (files.rfind (File::DIRSEPERATOR) + 1);
}
