//$Id: RDirSrchSrv.cpp,v 1.1 2001/08/12 15:19:09 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RemoteDirectorySearchServer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 11.8.2001
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


#include <gzo-cfg.h>

#define DEBUG 9
#include "Trace_.h"
#include "Socket.h"
#include "ATStamp.h"
#include "DirSrch.h"
#include "DirEntry.h"
#include "ANumeric.h"
#include "AByteArray.h"
#include "RDirSrchSrv.h"


static const char* const  commands[] = { "Next", "Find:\"", "Check:\"" };
static const unsigned int lengths[] =  { 4, 6, 7 };


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
RemoteDirSearchSrv::RemoteDirSearchSrv () {
   assert ((sizeof (commands) / sizeof (commands[0]))
	   == (sizeof (lengths) / sizeof (lengths[0])));
   for (int i (0); i < (sizeof (lengths) / sizeof (lengths[0])); ++i)
      assert (lengths[i] == strlen (commands[i]));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
RemoteDirSearchSrv::~RemoteDirSearchSrv () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Reads and executes commands send by a client 
//Parameters: socket: Socket for communication
//Returns   : Status (error) of last command
/*--------------------------------------------------------------------------*/
int RemoteDirSearchSrv::performCommands (int socket) throw (domain_error){
   dirEntry   result;
   AByteArray data;

   DirectorySearch dirSrch;

   Socket sock (socket);

   do {
      sock.read (data);
      TRACE5 ("RemoteDirSearchSrv::performCommands (int) - Read: " << data.data ());

      int i (0);
      for (; i < (sizeof (commands)/sizeof (commands[0])); ++i) {
         if (!strncmp (data.data (), commands[i], lengths[i]))
            break;
      }

      switch (i) {                                    // Perform passed command
      case 0:                                                      // Find next
         i = dirSrch.find ();
         if (i)
            writeError (sock, i);
         else
            writeResult (sock, result);
         break;

      case 1: {                                                   // Find first
         std::string search ("*");
         ANumeric    attribs (IDirectorySearch::FILE_NORMAL);
         i = dirSrch.find (search, result, attribs);
         if (i)
            writeError (sock, i);
         else
            writeResult (sock, result);
         }
         break;

      case 2: {                                            // Check passed data
         std::string argument (data.data () + lengths[i]);
         argument.replace (argument.length () - 1, 1, 0, '\0');
         TRACE9 ("RemoteDirSearchSrv::performCommands (int) - Checking "
                 << argument.c_str ());
         sock.write (dirSrch.isValid (argument) ? "RC=1" : "RC=0");
         }
         break;

      default:
         // Invalid command
         break;
      } // end-switch
   }
   while (data.length () > 1);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sends information about the found file to the client
//Parameters: socket: Socket for communication
//            result: Found file
/*--------------------------------------------------------------------------*/
void RemoteDirSearchSrv::writeResult (Socket& socket, const dirEntry& result) const
   throw (domain_error) {
   AByteArray write ("RC=0;File=\"");
   write += result.path ();
   write += result.name ();
   write += '"';

   write += ";Size=";
   ANumeric size (result.size ());
   write += size.toString ();

   ATimestamp time (result.time ());
   write += time.toString ();
   socket.write (write);
}

void RemoteDirSearchSrv::writeError (Socket& socket, int error) const
   throw (domain_error) {
   AByteArray write ("RC=");
   ANumeric err (error);

   write += err.toString ();
   socket.write (write);
}
