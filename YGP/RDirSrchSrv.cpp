//$Id: RDirSrchSrv.cpp,v 1.7 2001/10/08 14:31:18 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RemoteDirectorySearchServer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.7 $
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


#include <errno.h>

#include <gzo-cfg.h>

#define DEBUG 9
#include "File.h"
#include "Trace_.h"
#include "Socket.h"
#include "ATStamp.h"
#include "DirSrch.h"
#include "ANumeric.h"
#include "AttrParse.h"
#include "AByteArray.h"
#include "RDirSrchSrv.h"


static const unsigned int CMD_NEXT  = 0;
static const unsigned int CMD_FIND  = 1;
static const unsigned int CMD_CHECK = 2;
static const unsigned int CMD_END   = 3;

static const unsigned int CMD_OPEN  = 4;
static const unsigned int CMD_CLOSE = 5;
static const unsigned int CMD_READ  = 6;
static const unsigned int CMD_WRITE = 7;
static const unsigned int CMD_ISEOF = 8;

static const struct { 
   const char* const  cmd;
   const unsigned int len;
} commands[] = { { "Next", 4 },   { "Find=\"", 6 },  { "Check=\"", 7 },
                 { "End", 3 },    { "Open=\"", 6 },  { "Close=", 6 },
                 { "Read=", 5 },  { "Write=", 6 },   { "EOF=", 4 } };


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
RemoteDirSearchSrv::RemoteDirSearchSrv () {
#ifndef NDEBUG
   for (int i (0); i < (sizeof (commands) / sizeof (commands[0])); ++i)
      assert (strlen (commands[i].cmd) == commands[i].len);
#endif
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
   AByteArray data;

   DirectorySearch dirSrch;
   static FILE* pFile (NULL);

   Socket sock (socket);

   do {
      sock.read (data);
      TRACE5 ("RemoteDirSearchSrv::performCommands (int) - Read: " << data.data ());

      unsigned int i (0);
      for (; i < (sizeof (commands)/sizeof (commands[0])); ++i) {
         if (!strncmp (data.data (), commands[i].cmd, commands[i].len))
            break;
      }

      data += '\0';
      switch (i) {                                    // Perform passed command
      case CMD_NEXT: {                                            // Find next
         TRACE9  ("RemoteDirSearchSrv::performCommands (int) - Find next");
         const File* file = dirSrch.next ();
         if (file)
            writeResult (sock, *file);
         else
            writeError (sock, errno ? errno : ENOENT, false);
         }
         break;

      case CMD_FIND: {                                            // Find first
         std::string   files;
         unsigned long attribs;

         std::string argument (data.data ());

         try {
            AttributeParse attrparse;
            ATTRIBUTE (attrparse, std::string, files, "Find");
            ATTRIBUTE (attrparse, unsigned long, attribs, "Attr");

            attrparse.assignValues (argument);
	 }
	 catch (std::string& error) {
	    handleArgError (sock, error);
	    break;
	 }

         TRACE9  ("RemoteDirSearchSrv::performCommands (int) - Find " << files.c_str ());

	 if (files.empty ()) {
	    sock.write ("RC=99;E=No file specified");
            break;
         }

         const File* file = dirSrch.find (files, attribs);
         if (file)
            writeResult (sock, *file);
         else
            writeError (sock, errno ? errno : ENOENT, false);
         }
         break;

      case CMD_CHECK: {                                    // Check passed data
	 data[data.length () - 2] = '\0';
         std::string argument (data.data () + commands[i].len);
         TRACE9 ("RemoteDirSearchSrv::performCommands (int) - Checking "
                 << argument.c_str ());
         sock.write (dirSrch.isValid (argument) ? "RC=0" : "RC=1");
         }
         break;

      case CMD_END:                                        // End communication
	 return 99;

      case CMD_OPEN: {
	 std::string file, mode;

         try {
            AttributeParse attrparse;
            ATTRIBUTE (attrparse, std::string, file, "Open");
            ATTRIBUTE (attrparse, std::string, mode, "Mode");

            attrparse.assignValues (data.data ());
	 }
	 catch (std::string& error) {
	    handleArgError (sock, error);
	    break;
	 }

         pFile = fopen (file.c_str (), mode.c_str ());
         if (pFile) {
	    std::string res ("RC=0;ID=1");
            sock.write (res);
	 }
         else
            writeError (sock, errno);
	 }
	 break;

      case CMD_READ: {
         unsigned int id, length;
         try {
            AttributeParse attrparse;
            ATTRIBUTE (attrparse, unsigned int, id, "Read");
            ATTRIBUTE (attrparse, unsigned int, length, "Length");

            attrparse.assignValues (data.data ());
	 }
	 catch (std::string& error) {
	    handleArgError (sock, error);
	    break;
	 }

         char data[length + 12];
         strcpy (data, "RC=0;Data=\"");

         if (!fread (data + 11, 1, length, pFile)) {
            data[length + 11] = '"';
            sock.write (data, sizeof (data));
	 }
         else
            writeError (sock, errno);
         }
	 break;

      case CMD_CLOSE: {
         unsigned int id;
         try {
            AttributeParse attrparse;
            ATTRIBUTE (attrparse, unsigned int, id, "Close");

            attrparse.assignValues (data.data ());
	 }
	 catch (std::string& error) {
	    handleArgError (sock, error);
	    break;
	 }

         if (fclose (pFile))
            writeError (sock, errno);
         else
            sock.write ("RC=0", 4);
	 }
	 break;

      case CMD_WRITE:
         sock.write ("RC=99;E=Not yet implemented");
	 break;

      case CMD_ISEOF:
         unsigned int id;
         try {
            AttributeParse attrparse;
            ATTRIBUTE (attrparse, unsigned int, id, "EOF");

            attrparse.assignValues (data.data ());
	 }
	 catch (std::string& error) {
	    handleArgError (sock, error);
	    break;
	 }

         sock.write (feof (pFile) ? "RC=0" : "RC=1");
	 break;

      default:
	 TRACE ("RemoteDirSearchSrv::performCommands (int) - Invalid command "
                << data.data ());

	 sock.write ("RC=99;E=Invalid command");
      } // end-switch
   }
   while (data.length ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sends information about the found file to the client
//Parameters: socket: Socket for communication
//            result: Found file
/*--------------------------------------------------------------------------*/
void RemoteDirSearchSrv::writeResult (Socket& socket, const File& result) const
   throw (domain_error) {
   AByteArray write ("RC=0;File=\"");
   write += result.path ();
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
//            desc: Flag if a description should be included
/*--------------------------------------------------------------------------*/
int RemoteDirSearchSrv::writeError (Socket& socket, int error, bool desc) const
   throw (domain_error) {
   AByteArray write ("RC=");
   ANumeric err (error);
   write += err.toUnformatedString ();

   if (desc) {
      write += ";E=";
      write += strerror (error);
   }
   socket.write (write);
   return error;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Informs the caller about an input-error
//Parameters: sock: Reference to socket for output
//            error: Description of error in input
/*--------------------------------------------------------------------------*/
void RemoteDirSearchSrv::handleArgError (Socket& sock, const std::string& error) const {
   std::string errText ("RC=99;E=Invalid arguments: ");
   errText += error;
   sock.write (errText.c_str (), errText.length ());
}
