//$Id: RDirSrch.cpp,v 1.2 2001/04/09 15:06:19 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RemoteDirSearch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
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
//Parameters: search: Files to search
//            res: Buffer where to place the result
//            attribs: Attributes of files to find
//Returns   : Status; 0: OK
//Requires  : searchDir, pEntry  already set
/*--------------------------------------------------------------------------*/
int RemoteDirSearch::find (const std::string& search, dirEntry& res,
			   unsigned long attribs) throw (domain_error) {
   TRACE3 ("RemoteDirSearch::find (dirEntry&, unsigned long)");

   pEntry = &res;

   std::string send ("Find:\"");
   send += search;
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
//Purpose   : Checks if the searchDir is really a direcory
//Parameters: dir: Directory whose validity should be checked
//Returns   : bool: True if the directory exists
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isValid (const std::string& dir) const {
    std::string write ("Check:\"");
    write += dir;
    write += '"';

    sock.write (write.data (), write.length ());
    char OK;
    sock.read (&OK, 1);
    return OK == '1';
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the remote directory does exist
//Returns   : True if the remote directory does exis
/*--------------------------------------------------------------------------*/
bool RemoteDirSearch::isValid () const {
   return true;
}
