//$Id: RDirSrch.cpp,v 1.5 2001/08/21 23:50:20 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RemoteDirSearch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
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


#define DEBUG 0
#include "Trace_.h"
#include "ATStamp.h"
#include "ANumeric.h"
#include "AByteArray.h"

#include "DirEntry.h"
#include "RDirSrch.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: srch: String identifying host, directory to search and port
//                  The string must be in the format:
//                      <host>:<path>:<port>
/*--------------------------------------------------------------------------*/
RemoteDirSearch::RemoteDirSearch (const std::string& srch) throw (domain_error)
   : IDirectorySearch (), sock () {
   TRACE9 ("RemoteDirSearch::RemoteDirSearch (const std::string&) - "
           << srch.c_str () << ':');

   int posPort (srch.rfind (':'));
   std::string sPort (srch);
   sPort.replace (0, posPort, 0, '\0');

   host = srch;
   host.replace (posPort, host.length (), 0, '\0');
   setSearchValue (host);

   ANumeric port (sPort);
   sock.writeTo (host, (unsigned int)port.operator long ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: host: String identifying host, directory to search and port
//                  The string must be in the format:
//                      <host>:<path>
//            port: Portnumber the host is listening at
/*--------------------------------------------------------------------------*/
RemoteDirSearch::RemoteDirSearch (const std::string& srch, unsigned int port)
   throw (domain_error) : IDirectorySearch (), sock () {
   TRACE9 ("RemoteDirSearch::RemoteDirSearch (const std::string&, unsigned int) - "
           << srch.c_str () << ':' << port);

   setSearchValue (srch);
   sock.writeTo (host, port);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
RemoteDirSearch::~RemoteDirSearch () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the first file which matches the search-criteria
//Parameters: pAnswer: Character-buffer holding reponse from server
//Requires  : pAnswer valid ASCIIZ-string
/*--------------------------------------------------------------------------*/
void RemoteDirSearch::setFiledata (const char* pAnswer) {
   TRACE9 ("RemoteDirSearch::setFiledata (dirEntry&, const char*)");
   assert (pAnswer);
   assert (pEntry);

   // Set filename
   pAnswer += 11;
   char* pEnd (strstr (pAnswer + 1, "\";Size=")); assert (pEnd);
   char* pPathEnd (strrchr (pEnd, dirEntry::DIRSEPERATOR));
   if (pPathEnd) {
      char ch (pPathEnd[1]);
      pPathEnd[1] = '\0';
      pEntry->path (pAnswer);
      pPathEnd[1] = ch;
   }
   else {
      pEntry->path ("");
      pPathEnd = (char*)pAnswer - 1;
   }

   *pEnd = '\0';
   pEntry->name (pPathEnd + 1);
   TRACE9 ("RemoteDirSearch::setFiledata (dirEntry&, const char*) - "
           << pEntry->path () << pEntry->name ());

   // Set filesize
   pAnswer = pEnd + 7;
   pEnd = strstr (pAnswer, ";Time="); assert (pEnd);
   *pEnd = '\0';
   TRACE9 ("RemoteDirSearch::setFiledata (dirEntry&, const char*) - Size="
	   << pAnswer);
   ANumeric size (pAnswer); assert (size.isDefined ());
   pEntry->size (size);

   // Set filetime
   pAnswer = pEnd + 6;
   ATimestamp time (pAnswer);
   TRACE9 ("RemoteDirSearch::setFiledata (dirEntry&, const char*) - Time="
	   << time.toString ());
   pEntry->time (time.toSysTime ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the first file which matches the search-criteria
//Parameters: res: Buffer where to place the result
//            attribs: Attributes of files to find
//Returns   : Status; 0: OK
//Requires  : searchDir, pEntry  already set
/*--------------------------------------------------------------------------*/
int RemoteDirSearch::find (dirEntry& res, unsigned long attribs) throw (domain_error) {
   TRACE9 ("RemoteDirSearch::find (dirEntry&, unsigned long)");

   pEntry = &res;

   AByteArray buffer ("Find:\"");
   buffer += files;
   buffer += "\";Attr=";
   
   ANumeric attrs (attribs);
   buffer += attrs.toString ();
   buffer += '\0';

   TRACE9 ("RemoteDirSearch::find (dirEntry&, unsigned long) - Sending:\n\t"
           << buffer.length () << " bytes: " << buffer.data ());
   sock.write (buffer);

   sock.read (buffer);
   buffer += '\0';
   TRACE9 ("RemoteDirSearch::find (dirEntry&, unsigned long) - Read:\n\t"
           << buffer.length () << " bytes: " << buffer.data ());

   if (isOK (buffer)) {
      setFiledata (buffer.data ());
      return 0;
   }
   else {
      return 1;
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the next file which matches the search-criteria
//Returns   : Status; 0: OK
//Requires  : searchDir, pEntry  already set
/*--------------------------------------------------------------------------*/
int RemoteDirSearch::find () throw (domain_error) {
   AByteArray buffer ("Next");

   TRACE8 ("RemoteDirSearch::find () - Sending:\n\t"
           << buffer.length () << " bytes: " << buffer.data ());
   sock.write (buffer);
   TRACE9 ("RemoteDirSearch::find () - Sended");

   sock.read (buffer);
   buffer += '\0';
   TRACE8 ("RemoteDirSearch::find () - Read:\n\t"
           << buffer.length () << " bytes: " << buffer.data ());

   if (isOK (buffer)) {
      setFiledata (buffer.data ());
      return 0;
   }
   else {
      return 1;
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the position of the seperator-character between host-
//            name and directory to analyze
//Parameters: dir: Directory whose validity should be checked
//Returns   : True if the remote directory does exis
/*--------------------------------------------------------------------------*/
int RemoteDirSearch::posSeperator (const std::string& dir) const {
#if SYSTEM == UNIX
   int pos (dir.find (HOSTSEPERATOR));
#else
   // Search after drive-letter-seperator in Windoze, ...
   int pos ((dir.length < 3) ? std::string::npos
	    : (dir.find (HOSTSEPERATOR, 2) == std::string::npos
	       ? std::string::npos : dir.find (dirEntry::DIRSEPERATOR, pos)));
#endif

   return pos;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the searchDir is really a direcory
//Parameters: dir: Directory (without host-part) whose validity should be checked
//Returns   : bool: True if the directory exists
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isValid (const std::string& dir) const {
   TRACE5 ("RemoteDirSearch::isValid (const std::string&) - " << dir.c_str ());

   std::string write ("Check:\"");
   write.append (dir, 0, dir.rfind (dirEntry::DIRSEPERATOR));
   write += '"';
   TRACE8 ("RemoteDirSearch::isValid (const std::string&) - Cmd = "
           << write.c_str ());

   sock.write (write.data (), write.length ());

   AByteArray OK (10);
   sock.read (OK);
   TRACE9 ("RemoteDirSearch::isValid (const std::string&) - Answer: "
           << OK.data ());

   return isOK (OK);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the remote directory does exist
//Returns   : True if the remote directory does exis
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isValid () const {
   return isValid (files);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the remote directory does exist
//Returns   : True if the remote directory does exis
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isOK (const AByteArray& answer) const {
   assert (answer.length () > 3);
   return !strncmp (answer.data (), "RC=", 3) && (answer[3] == '0');
}

/*--------------------------------------------------------------------------*/
//Purpose   : Splits the search-string in its host-, directory- and filepart;
//            it prepares the internal data also for a new search
//Parameters: search: Files to find
/*--------------------------------------------------------------------------*/
void RemoteDirSearch::setSearchValue (const std::string& search) {
   TRACE9 ("RemoteDirSearch::setSearchValue (const std::string& srch) - "
           << search.c_str ());

   int len (search.find (HOSTSEPERATOR));
   host = search;
   host.replace (len, host.length (), 0, '\0');
   files = search;
   files.replace (0, len + 1, 0, '\0');
   TRACE8 ("RemoteDirSearch::setSearchValue (const std::string& srch) - "
           << "Host = " << host.c_str () << "; Files = " << files.c_str ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the directory-part of search-string (including host)
//Returns   : std::string: Directory to search
/*--------------------------------------------------------------------------*/
const std::string& RemoteDirSearch::getDirectory () const {
    return host;
 }

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the directory-part of search-string (including host)
//Returns   : std::string: Files to find
/*--------------------------------------------------------------------------*/
const std::string& RemoteDirSearch::getFileSpec () const {
    return files;
 }
