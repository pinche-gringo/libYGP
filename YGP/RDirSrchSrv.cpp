//$Id: RDirSrchSrv.cpp,v 1.30 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : RemoteDirectorySearchServer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.30 $
//AUTHOR      : Markus Schwab
//CREATED     : 11.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2004, 2006, 2008

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
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif

#include <cerrno>

#include "YGP/Internal.h"

#ifndef TRACELEVEL
#  define TRACELEVEL -1
#endif
#include "YGP/File.h"
#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Socket.h"
#include "YGP/ATStamp.h"
#include "YGP/DirSrch.h"
#include "YGP/ANumeric.h"
#include "YGP/AssParse.h"
#include "YGP/Exception.h"
#include "YGP/AttrParse.h"
#include "YGP/RDirSrchSrv.h"


static const unsigned int CMD_NEXT  = 0;
static const unsigned int CMD_FIND  = 1;
static const unsigned int CMD_CHECK = 2;
static const unsigned int CMD_END   = 3;

static const unsigned int CMD_OPEN  = 4;
static const unsigned int CMD_CLOSE = 5;
static const unsigned int CMD_READ  = 6;
static const unsigned int CMD_WRITE = 7;
static const unsigned int CMD_ISEOF = 8;


// Actually also the members of this struct should be const (const char* const
// & const unsigned int), but Visual C++ wouldn't compile that. I really wonder
// if they invented C# because they couldn't implement a working C++ compiler?
/// Commands understood by the server
static const struct {
   const char* cmd;
   unsigned int len;
} commands[] = { { "Next", 4 },   { "Find=\"", 6 },  { "Check=\"", 7 },
                 { "End", 3 },    { "Open=\"", 6 },  { "Close=", 6 },
                 { "Read=", 5 },  { "Write=", 6 },   { "EOF=", 4 } };


namespace YGP {


//-----------------------------------------------------------------------------
/// Defaultconstructor
//-----------------------------------------------------------------------------
RemoteDirSearchSrv::RemoteDirSearchSrv () {
#if CHECK > 2
   for (unsigned int i (0); i < (sizeof (commands) / sizeof (commands[0])); ++i)
      Check (strlen (commands[i].cmd) == commands[i].len);
#endif
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
RemoteDirSearchSrv::~RemoteDirSearchSrv () {
}


//-----------------------------------------------------------------------------
/// Handles the commands send from the client; the respectative action is
/// performed and data is returned accordingly.
///
/// \param socket Socket for communication
/// \returns int 0 in case of end-of-communication; 99 after the END-command
/// \throw YGP::CommError In case of a communication problem
//-----------------------------------------------------------------------------
int RemoteDirSearchSrv::performCommands (int socket) throw (YGP::CommError){
   std::string data;

   DirectorySearch dirSrch;
   static FILE* pFile = NULL;

   Socket sock (socket);

   do {
      sock.read (data);
      data += '\0';
      TRACE5 ("RemoteDirSearchSrv::performCommands (int) - Read: " << data.data ());

      unsigned int i (0);
      for (; i < (sizeof (commands)/sizeof (commands[0])); ++i) {
         if (!strncmp (data.data (), commands[i].cmd, commands[i].len))
            break;
      }

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

         char* contents = new char [length];
         if ((length = fread (contents, 1, length, pFile))) {
            std::string send ("RC=0;Length=");
            ANumeric len (length);
            send += len.toUnformattedString ();
            send += AssignmentParse::makeAssignment (";Data", contents, length);
            sock.write (send.data (), send.length ());
         }
         else {
            Check3 (errno);
            writeError (sock, errno);
         }
	 delete [] contents;
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
         pFile = NULL;
         }
         break;

      case CMD_WRITE: {
         std::string err ("RC=99;E=");
         err += _("Not yet implemented");
         sock.write (err);
         }
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

      default: {
	 TRACE ("RemoteDirSearchSrv::performCommands (int) - Invalid command "
                << data.data ());

         std::string error ("RC=99;E=");
         error += _("Invalid command");
	 sock.write (error);
         }
      } // end-switch
   }
   while (data.length ());
   return 0;
}

//-----------------------------------------------------------------------------
/// Sends information about the occured error to the client
/// \param socket Socket for communication
/// \param error Errornumber
/// \param desc Flag if a description should be included
//-----------------------------------------------------------------------------
int RemoteDirSearchSrv::writeError (Socket& socket, int error, bool desc) const
   throw (YGP::CommError) {
   std::string write ("RC=");
   ANumeric err (error);
   write += err.toUnformattedString ();

   if (desc) {
      write += ";E=";
      write += strerror (error);
   }
   socket.write (write);
   return error;
}

//-----------------------------------------------------------------------------
/// Informs the caller about an input-error
/// \param sock Reference to socket for output
/// \param error Description of error in input
//-----------------------------------------------------------------------------
void RemoteDirSearchSrv::handleArgError (Socket& sock, const std::string& error) const {
   std::string errText ("RC=99;E=");
   errText += _("Invalid arguments: ");
   errText += error;
   sock.write (errText.c_str (), errText.length ());
}

//-----------------------------------------------------------------------------
/// Sends information about the found file to the client
/// \param socket Socket for communication
/// \param result Found file
//-----------------------------------------------------------------------------
void RemoteDirSearchSrv::writeResult (Socket& socket, const File& result) const
   throw (YGP::CommError) {
   std::string write ("RC=0;File=\"");
   write += result.path ();
   write += result.name ();
   write += '"';

   write += ";Size=";
   ANumeric number (result.size ());
   write += number.toUnformattedString ();

   write += ";Time=";
   ATimestamp time (result.time ());
   write += time.toUnformattedString ();

   ANumeric attr (IDirectorySearch::convertFromSysAttribs (result.attributes ()));
   write += ";Attr=";
   write += attr.toUnformattedString ();

   socket.write (write);
}

}
