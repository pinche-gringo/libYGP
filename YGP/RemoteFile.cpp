// $Id: RemoteFile.cpp,v 1.23 2006/06/02 02:35:15 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : RemoteFile
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.23 $
//AUTHOR      : Markus Schwab
//CREATED     : 2.10.2001
//COPYRIGHT   : Copyright (C) 2001 - 2004, 2006

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

#include <ygp-cfg.h>

#ifndef TRACELEVEL
#  define TRACELEVEL -1
#endif
#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Internal.h"
#include "YGP/ANumeric.h"
#include "YGP/AttrParse.h"
#include "YGP/RemoteFile.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
RemoteFile::~RemoteFile () {
}


//-----------------------------------------------------------------------------
/// Duplicates (clones) the object and returns a pointer to the newly created
/// object.
/// \returns \c File*: Pointer to newly created clone
//-----------------------------------------------------------------------------
File* RemoteFile::clone () const {
   return new RemoteFile (*this);
}

//-----------------------------------------------------------------------------
/// Opends the file in the specified mode. The mode parameter can have the
/// same values as the ANSI-C fopen-function.
/// \param mode: Mode for open the file (analogue to libc's fopen)
/// \returns \c void*: Pointer to a handle for the opened file.
/// \throw string: In case of an error a textual description
//-----------------------------------------------------------------------------
void* RemoteFile::open (const char* mode) const throw (std::string) {
   std::string file (path ()); file += name ();
   TRACE5 ("RemoteFile::open (const char*) const - " << file);
   Check3 (mode);

   std::string buffer ("Open=\"");
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

//-----------------------------------------------------------------------------
/// Closes a (previously opened) file
/// \param file: Handle of opened file
/// \throw string: In case of an error a textual description
//-----------------------------------------------------------------------------
void RemoteFile::close (void* file) const throw (std::string) {
   TRACE5 ("RemoteFile::close (void*) const - " << path () << name ());
   Check3 (file);

   std::string buffer ("Close=");
   ANumeric id ((unsigned int)file);
   buffer += id.toUnformattedString ();

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

//-----------------------------------------------------------------------------
/// Reads the (next) specified number of characters from the (previously
/// opened) file (or less, if the end of the file has been reached) into
/// buffer. and returns the number of actually read bytes. If an error
/// occurres, an exception is thrown
/// \param file: Handle of openeded file
/// \param buffer: Buffer for data
/// \param length: Maximal length of buffer
/// \returns \c int: Number of read bytes
/// \throw string: In case of an error a textual description
//-----------------------------------------------------------------------------
int RemoteFile::read (void* file, char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("RemoteFile::read (void*, char*, unsigned int) const - "
           << path () << name ());

   Check3 (file);
   Check3 (buffer);
   Check3 (length);

   std::string text ("Read=");
   ANumeric id ((unsigned int)file);
   text += id.toUnformattedString ();

   id = length;
   text += ";Length=";
   text += id.toUnformattedString ();

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
      unsigned int lenReceived;
      ATTRIBUTE (attrs, unsigned int, lenReceived, "Length");
      handleServerMsg (attrs, text.data () + 5);

      if (lenReceived <= length) {
	 AssignmentParse ass (text.substr (text.find (';')));
	 memcpy (buffer, ass.getNextNode ().data (), lenReceived);
      }
      else
	 TRACE ("RemoteFile::read (void*, char*, unsigned int) const"
		"\n - Invalid length received: " << lenReceived << "; expected " << length);
      return length;
   }
   else
      handleServerError (text.data ());
   return 0;
}

//-----------------------------------------------------------------------------
/// Checks if further data is available for reading
/// \param file: Handle of openeded file
//-----------------------------------------------------------------------------
bool RemoteFile::isEOF (void* file) const throw (std::string) {
   TRACE5 ("RemoteFile::isEOF (void*) const - " << path () << name ());
   Check3 (file);

   std::string buffer ("EOF=");
   ANumeric id ((unsigned int)file);
   buffer += id.toUnformattedString ();

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

//-----------------------------------------------------------------------------
/// Checks if response from the server is
/// \returns \c True if the server sent an RC=0 response
//-----------------------------------------------------------------------------
bool RemoteFile::isOK (const std::string& answer) const {
   return (answer.length () == 4) && !strcmp (answer.data (), "RC=0");
}

//-----------------------------------------------------------------------------
/// Checks out the error send by the server; if it sends an explaining string
/// this is thrown to inform the client name and directory to analyze
/// \param pAnswer: Response from the server
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Assigns the values from the server-response to (local) variables
/// \param attrs: Attributes to manipulate
/// \param pAnswer: Data send from server
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Writes the specified number of characters to the (previously opened) file
/// and returns the number of actually written bytes. If an error occurres, an
/// exception is thrown.
/// \param file: Handle of openeded file
/// \param buffer: Buffer of data
/// \param length: Length of buffer (= bytes to write)
/// \returns \c int: Number of written bytes
/// \throw string: In case of an error a textual description
//-----------------------------------------------------------------------------
int RemoteFile::write (void* file, const char* buffer, unsigned int length) const throw (std::string) {
   TRACE5 ("RemoteFile::write (void*, char*, unsigned int) const - "
           << path () << name ());
   Check3 (file);
   Check3 (buffer);
   Check3 (length);

   std::string text ("Write=");
   ANumeric id ((unsigned int)file);
   text += id.toUnformattedString ();

   id = length;
   text += ";Length=";
   text += id.toUnformattedString ();

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

}
