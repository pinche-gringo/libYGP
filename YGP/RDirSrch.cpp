//$Id: RDirSrch.cpp,v 1.3 2001/08/08 01:44:06 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RemoteDirSearch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
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


#define DEBUG 9
#include "Trace_.h"
#include "DirEntry.h"
#include "RDirSrch.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
RemoteDirSearch::RemoteDirSearch (const std::string& host, unsigned int port)
   throw (domain_error) : IDirectorySearch (), sock (host, port) {
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
RemoteDirSearch::~RemoteDirSearch () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the first file which matches the search-criteria
//Parameters: res: Buffer where to place the result
//            attribs: Attributes of files to find
//Returns   : Status; 0: OK
//Requires  : searchDir, pEntry  already set
/*--------------------------------------------------------------------------*/
int RemoteDirSearch::find (dirEntry& res, unsigned long attribs) throw (domain_error) {
   TRACE3 ("RemoteDirSearch::find (dirEntry&, unsigned long)");

   pEntry = &res;

   std::string send ("Find:\"");
   send += host;
   send += files;
   send += "\";Attr=";
   send += "TODO";
   send += ';';

   TRACE8 ("RemoteDirSearch::find (dirEntry&, unsigned long) - Sending:\n\t"
           << send.length () << " bytes: " << send.data ());
   sock.write (send.data (), send.length ());
   return 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the next file which matches the search-criteria
//Returns   : Status; 0: OK
//Requires  : searchDir, pEntry  already set
/*--------------------------------------------------------------------------*/
int RemoteDirSearch::find () throw (domain_error) {
   std::string send ("Next");

   TRACE8 ("RemoteDirSearch::find () - Sending:\n\t"
           << send.length () << " bytes: " << send.data ());
   sock.write (send.data (), send.length ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed parameter contains a host-seperator-char
//Parameters: dir: Directory whose validity should be checked
//Returns   : True if the remote directory does exis
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::containsSeperator (const std::string& dir) const {
#if SYSTEM == UNIX
   int pos (dir.find (HOSTSEPERATOR));
#else
   // Search after drive-letter-seperator in Windoze, ...
   int pos ((dir.length < 3) ? std::string::npos
	    : (dir.find (HOSTSEPERATOR, 2) == std::string::npos
	       ? std::string::npos : dir.find (dirEntry::DIRSEPERATOR, pos)));
#endif

   return pos != std::string::npos;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the searchDir is really a direcory
//Parameters: dir: Directory whose validity should be checked
//Returns   : bool: True if the directory exists
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isValid (const std::string& dir) const {
   if (containsSeperator (dir)) {
      std::string write ("Check:\"");
      write += dir;
      write += '"';

      sock.write (write.data (), write.length ());
      char OK;
      sock.read (&OK, 1);
      return OK == '1';
   }
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the remote directory does exist
//Returns   : True if the remote directory does exis
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isValid () const {
   return isValid (host);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Splits the search-string in its host-, directory- and filepart;
//            it prepares the internal data also for a new search
//Parameters: search: Files to find
/*--------------------------------------------------------------------------*/
void RemoteDirSearch::setSearchValue (const std::string& search) {
   assert (isValid (search));

   int len (search.rfind (dirEntry::DIRSEPERATOR));
   host = search;
   host.replace (len + 1, host.length (), 0, '\0');
   files = search;
   files.replace (0, len + 1, 0, '\0');
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
