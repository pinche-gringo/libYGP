//$Id: RDirSrchSrv.cpp,v 1.3 2001/08/28 20:21:12 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RemoteDirectorySearchServer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
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

#define DEBUG 0
#include "Trace_.h"
#include "Socket.h"
#include "ATStamp.h"
#include "DirSrch.h"
#include "DirEntry.h"
#include "ANumeric.h"
#include "AttrParse.h"
#include "AByteArray.h"
#include "RDirSrchSrv.h"


static const char* const  commands[] = { "Next", "Find=\"", "Check=\"" };
static const unsigned int lengths[] =  { strlen (commands[0]), strlen (commands[1]),
                                         strlen (commands[2]) };


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
RemoteDirSearchSrv::RemoteDirSearchSrv () {
   assert ((sizeof (commands) / sizeof (commands[0]))
	   == (sizeof (lengths) / sizeof (lengths[0])));
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
         TRACE9  ("RemoteDirSearchSrv::performCommands (int) - Find next");
         i = dirSrch.find ();
         if (i)
            return writeError (sock, i);
         else
            writeResult (sock, result);
         break;

      case 1: {                                                   // Find first
         std::string   files;
         unsigned long attribs;

	 data += '\0';
         std::string argument (data.data ());

         try {
            AttributeParse attrparse;
            ATTRIBUTE (attrparse, std::string, files, "Find");
            ATTRIBUTE (attrparse, unsigned long, attribs, "Attr");

            attrparse.assignValues (argument);
	 }
	 catch (std::string& error) {
            sock.write ("RC=99;E=Invalid arguments");
	    return 99;
	 }

         TRACE9  ("RemoteDirSearchSrv::performCommands (int) - Find " << files.c_str ());

	 if (files.empty ())
	    sock.write ("RC=99;E=No file specified");

         i = dirSrch.find (files, result, attribs);
         if (i)
            return writeError (sock, i);
         else
            writeResult (sock, result);
         }
         break;

      case 2: {                                            // Check passed data
	 data[data.length () - 1] = '\0';
         std::string argument (data.data () + lengths[i]);
         TRACE9 ("RemoteDirSearchSrv::performCommands (int) - Checking "
                 << argument.c_str ());
         sock.write (dirSrch.isValid (argument) ? "RC=0" : "RC=1");
         }
         break;

      default:
	 data += '\0';
	 TRACE ("RemoteDirSearchSrv::performCommands (int) - Invalid command "
                << data.data ());

	 sock.write ("RC=99;E=Invalid command");
	 return 99;
      } // end-switch
   }
   while (data.length ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sends information about the found file to the client
//Parameters: socket: Socket for communication
//            result: Found file
/*--------------------------------------------------------------------------*/
void RemoteDirSearchSrv::writeResult (Socket& socket, const dirEntry& result) const
   throw (domain_error) {
   AByteArray write ("RC=0;File=\"");
   write += result.name ();
   write += '"';

   write += ";Size=";
   ANumeric number (result.size ());
   write += number.toUnformatedString ();

   write += ";Time=";
   ATimestamp time (result.time ());
   write += time.toUnformatedString ();

   ANumeric attr (IDirectorySearch::convertFromSysAttribs (result.attributes ()));
   write += ";Attr=";
   write += attr.toUnformatedString ();

   socket.write (write);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sends information about the occured error to the client
//Parameters: socket: Socket for communication
//            error: Errornumber
/*--------------------------------------------------------------------------*/
int RemoteDirSearchSrv::writeError (Socket& socket, int error) const
   throw (domain_error) {
   AByteArray write ("RC=");
   ANumeric err (error);

   write += err.toUnformatedString ();
   socket.write (write);
   return error;
}
