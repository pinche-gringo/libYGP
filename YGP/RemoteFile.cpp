// $Id: RemoteFile.cpp,v 1.10 2003/03/03 05:59:28 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RemoteFile
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
//AUTHOR      : Markus Schwab
//CREATED     : 2.10.2001
//COPYRIGHT   : Anticopyright (A) 2001, 2002

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

#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif

#include <gzo-cfg.h>

#define TRACELEVEL -1
#include "Check.h"
#include "Trace_.h"
#include "Internal.h"
#include "ANumeric.h"
#include "AttrParse.h"
#include "AByteArray.h"
#include "RemoteFile.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
RemoteFile::~RemoteFile () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Duplicates (clones) the object and returns a pointer to the
//            newly created object.
//Returns   : File*: Pointer to newly created clone
/*--------------------------------------------------------------------------*/
File* RemoteFile::clone () const {
   return new RemoteFile (*this);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Opends the file in the specified mode. The mode parameter can
//            have the same values than the ANSI-C fopen-function.
//Parameters: mode: Mode for open the file (analogue to libc's fopen)
//Returns   : void*: Pointer to a handle for the opened file.
//Throws    : string: In case of an error a textual description
/*--------------------------------------------------------------------------*/
void* RemoteFile::open (const char* mode) const throw (std::string) {
   std::string file (path ()); file += name ();
   TRACE5 ("RemoteFile::open (const char*) const - " << file);
   Check3 (mode);

   AByteArray buffer ("Open=\"");
   buffer += file;
   buffer += "\";Mode=";
   buffer += mode;
   if (mode[1] != 'b')
      buffer += 'b';

   try {
      sock.write (buffer);
      sock.read (buffer);
   }
   catch (std::domain_error& error) {
      std::string err (error.what ());
      throw err;
   }
   buffer += '\0';

   if (isOK (buffer)) {
      int id;

      AttributeParse attrs;
      ATTRIBUTE (attrs, int, id, "ID");

      handleServerMsg (attrs, buffer.data () + 5);
      return (void*)id;
   }
   else
      handleServerError (buffer.data ());
   return 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Closes a (previously opened) file
//Parameters: file: Handle of opened file
//Throws    : string: In case of an error a textual description
/*--------------------------------------------------------------------------*/
void RemoteFile::close (void* file) const throw (std::string) {
   TRACE5 ("RemoteFile::close (void*) const - " << path () << name ());
   Check3 (file);

   AByteArray buffer ("Close=");
   ANumeric id ((unsigned int)file);
   buffer += id.toUnformatedString ();

   try {
      sock.write (buffer);
      sock.read (buffer);
   }
   catch (std::domain_error& error) {
      std::string err (error.what ());
      throw err;
   }
   buffer += '\0';

   if (!isOK (buffer))
      handleServerError (buffer.data ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads the (next) specified number of characters from the
//            (previously opened) file (or less, if the end of the file has
//            been reached) into buffer. and returns the number of actually
//            read bytes.
//
//            If an error occurres, an exception is thrown
//Parameters: file: Handle of openeded file
//            buffer: Buffer for data
//            length: Maximal length of buffer
//Returns   : int: Number of read bytes
//Throws    : string: In case of an error a textual description
/*--------------------------------------------------------------------------*/
int RemoteFile::read (void* file, char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("RemoteFile::read (void*, char*, unsigned int) const - "
           << path () << name ());

   Check3 (file);
   Check3 (buffer);
   Check3 (length);

   AByteArray text ("Read=");
   ANumeric id ((unsigned int)file);
   text += id.toUnformatedString ();

   id = length;
   text += ";Length=";
   text += id.toUnformatedString ();

   try {
      sock.write (text);
      sock.read (text);
   }
   catch (std::domain_error& error) {
      std::string err (error.what ());
      throw err;
   }
   text += '\0';

   if (isOK (text)) {
      AttributeParse attrs;
      ATTRIBUTE (attrs, char* const, buffer, "Data");
      ATTRIBUTE (attrs, unsigned int, length, "Length");

      handleServerMsg (attrs, text.data () + 5);
      return length;
   }
   else
      handleServerError (text.data ());
   return 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Writes the specified number of characters to the  (previously
//            opened) file and returns the number of actually written bytes.
//
//            If an error occurres, an exception is thrown.
//Parameters: file: Handle of openeded file
//            buffer: Buffer of data
//            length: Length of buffer (= bytes to write)
//Returns   : int: Number of written bytes
//Throws    : string: In case of an error a textual description
/*--------------------------------------------------------------------------*/
int RemoteFile::write (void* file, const char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("RemoteFile::write (void*, char*, unsigned int) const - "
           << path () << name ());
   Check3 (file);
   Check3 (buffer);
   Check3 (length);

   AByteArray text ("Write=");
   ANumeric id ((unsigned int)file);
   text += id.toUnformatedString ();

   id = length;
   text += ";Length=";
   text += id.toUnformatedString ();

   text += ";Data=\"";
   text.append (buffer, length);
   text += '"';

   try {
      sock.write (text);
      sock.read (text);
   }
   catch (std::domain_error& error) {
      std::string err (error.what ());
      throw err;
   }
   text += '\0';

   if (isOK (text))
      return length;
   else
      handleServerError (text.data ());
   return 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if further data is available for reading
//Parameters: file: Handle of openeded file
/*--------------------------------------------------------------------------*/
bool RemoteFile::isEOF (void* file) const throw (std::string) {
   TRACE5 ("RemoteFile::isEOF (void*) const - " << path () << name ());
   Check3 (file);

   AByteArray buffer ("EOF=");
   ANumeric id ((unsigned int)file);
   buffer += id.toUnformatedString ();
   
   try {
      sock.write (buffer);
      sock.read (buffer);
   }
   catch (std::domain_error& error) {
      std::string err (error.what ());
      throw err;
   }
   buffer += '\0';

   if (isOK (buffer))
      return true;

   handleServerError (buffer.data ());
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if response from the server is 
//Returns   : True if the server sent an RC=0 response
/*--------------------------------------------------------------------------*/
bool RemoteFile::isOK (const AByteArray& answer) const {
   return (answer.length () == 4) && !strcmp (answer.data (), "RC=0");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks out the error send by the server; if it sends an
//            explaining string this is thrown to inform the client
//            name and directory to analyze
//Parameters: pAnswer: Response from the server
/*--------------------------------------------------------------------------*/
void RemoteFile::handleServerError (const char* pAnswer) const throw (std::string) {
   int rc;
   std::string error;

   AttributeParse attrs;
   ATTRIBUTE (attrs, int, rc, "RC");
   ATTRIBUTE (attrs, std::string, error, "E");

   handleServerMsg (attrs, pAnswer);

   if (!error.empty ()) {
      error = _("Server returned an error: ") + error;
      throw (error);
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assigns the values from the server-response to (local) variables
//Parameters: attrs: Attributes to manipulate
//            pAnswer: Data send from server
/*--------------------------------------------------------------------------*/
void RemoteFile::handleServerMsg (const AttributeParse& attrs, const char* pAnswer)
   const throw (std::string) {
   Check3 (pAnswer);

   try {
      attrs.assignValues (pAnswer);
   }
   catch (std::string& error) {
      TRACE ("RemoteFile::handleServerMsg (const AttributeParse&, const char*) const"
             "\n - Error parsing attributes: " << error.c_str ());
   }
}
