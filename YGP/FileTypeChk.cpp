//$Id: FileTypeChk.cpp,v 1.5 2008/06/11 17:53:40 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : YGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.07.2006
//COPYRIGHT   : Copyright (C) 2006 - 2008

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


#include <ygp-cfg.h>

#include <cctype>
#include <cstring>
#include <fstream>
#include <algorithm>

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Utility.h"

#include "FileTypeChk.h"


namespace YGP {

static const char ID_PDF[]        = "%PDF";
static const char ID_RTF[]        = "{\\rtf";
static const char ID_ABIWORD[]    = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE abiword PUBLIC \"-//ABISOURCE//DTD AWML";
static const char ID_ID3[]        = "ID3";
static const char ID_GIF87[]      = "GIF87a";
static const char ID_GIF89[]      = "GIF89a";
static const char ID_OGG[]        = "OggS";
static const char ID_JPEG[]       = "\xFF\xD8\xFF\xE0\x00\x10JFIF";
static const char ID_PNG[]        = "\x89PNG\x0D\x0A\x1A\x0A";
static const char ID_HTML[]       = "<!DOCTYPE HTML";
static const char ID_MSOFFICE[]   = "\xD0\xCF\x11\xE0\xA1\xB1\x1A\xE1";
static const char ID_STAROFFICE[] = "SfxDocumentInfo";

static const unsigned int ID_MP3                  (0xE0FF);


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
FileTypeChecker::~FileTypeChecker () {
}


//-----------------------------------------------------------------------------
/// Defaultconstructor
//-----------------------------------------------------------------------------
   FileTypeCheckerByExtension::FileTypeCheckerByExtension () : FileTypeChecker (), types () {
   // Create table of file-types
   types.insert (types.end (), std::pair<std::string, unsigned int> ("abw", ABIWORD));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("doc", MSOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("docx", OOXML));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("gif", GIF));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("htm", HTML));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("html", HTML));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("jpeg", JPEG));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("jpg", JPEG));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("mp3", MP3));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("odg", OPENOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("odp", OPENOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("ods", OPENOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("odt", OPENOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("ogg", OGG));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("pdf", PDF));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("php", HTML));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("png", PNG));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("ppt", MSOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("rtf", RTF));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sda", STAROFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sdc", STAROFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sdd", STAROFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sdw", STAROFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sht", HTML));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("shtm", HTML));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("shtml", HTML));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sxc", OPENOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sxd", OPENOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sxi", OPENOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sxm", OPENOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("sxw", OPENOFFICE));
   types.insert (types.end (), std::pair<std::string, unsigned int> ("xls", MSOFFICE));
   TRACE9 ("FileTypeCheckerByExtension::FileTypeCheckerByExtension () - Known types " << types.size ());
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
FileTypeCheckerByExtension::~FileTypeCheckerByExtension () {
}


//-----------------------------------------------------------------------------
/// Returns the type of the file (e.g. MS Word document) according to
/// passed file-name
/// \param file File to inspect
/// \returns unsigned int Type of file
//-----------------------------------------------------------------------------
unsigned int FileTypeCheckerByExtension::getType (const char* file) const {
   Check1 (file);
   TRACE1 ("FileTypeCheckerByExtension::getType (const char*) const - Checking " << file);

   const char* extension (strrchr (file, '.'));
   return extension ? getType4Extension (extension + 1) : (unsigned int)UNKNOWN;
}

//-----------------------------------------------------------------------------
/// Returns the type of the file (e.g. MS Word document) according to
/// passed extension
/// \param file File to inspect
/// \returns unsigned int Type of file
//-----------------------------------------------------------------------------
unsigned int FileTypeCheckerByExtension::getType4Extension (const char* extension) const {
   Check1 (extension);
   TRACE3 ("FileTypeCheckerByExtension::getType4Extension (const char*) const - " << extension);

   std::map<std::string, unsigned int>::const_iterator i (types.find (extension));
   return i != types.end () ? i->second : (unsigned int)UNKNOWN;
}

//-----------------------------------------------------------------------------
/// Adds a type to the handled file-types
/// \param ext Extension, which is handled
/// \param vaule ID identifying the file-type
//-----------------------------------------------------------------------------
void FileTypeCheckerByExtension::addType (const char* ext, unsigned int value) {
   TRACE9 ("FileTypeCheckerByExtension::addType (const char*, unsigned int) - " << ext << ": " << value);
   Check2 (types.find (ext) == types.end ());
   types[ext] = value;
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
FileTypeCheckerByCaseExt::~FileTypeCheckerByCaseExt () {
}


//-----------------------------------------------------------------------------
/// Returns the type of the file (e.g. MS Word document) according to
/// passed file-name (ignoring the case)
/// \param file File to inspect
/// \returns unsigned int Type of file
//-----------------------------------------------------------------------------
unsigned int FileTypeCheckerByCaseExt::getType (const char* file) const {
   Check1 (file);
   TRACE1 ("FileTypeCheckerByCaseExt::getType (const char*) const - Checking " << file);

   const char* extension (strrchr (file, '.'));
   if (extension) {
      std::string ext (extension + 1);
      std::transform (ext.begin (), ext.end (), ext.begin (), tolower);
      return FileTypeCheckerByExtension::getType4Extension (ext.c_str ());
   }
   return UNKNOWN;
}


//-----------------------------------------------------------------------------
/// Defaultconstructor
//-----------------------------------------------------------------------------
FileTypeCheckerByContent::FileTypeCheckerByContent () : FileTypeChecker (), types () {
   // Create table of file-types
   types.push_back (ID (0, sizeof (ID_PDF) - 1, ID_PDF, PDF));
   types.push_back (ID (0, sizeof (ID_RTF) - 1, ID_RTF, RTF));
   types.push_back (ID (0, sizeof (ID_ABIWORD) - 1, ID_ABIWORD, ABIWORD));
   types.push_back (ID (0, sizeof (ID_ID3) - 1, ID_ID3, MP3));
   types.push_back (ID (0, sizeof (ID_GIF87) - 1, ID_GIF87, GIF));
   types.push_back (ID (0, sizeof (ID_GIF89) - 1, ID_GIF89, GIF));
   types.push_back (ID (0, sizeof (ID_OGG) - 1, ID_OGG, OGG));
   types.push_back (ID (0, sizeof (ID_JPEG) - 1, ID_JPEG, JPEG));
   types.push_back (ID (0, sizeof (ID_PNG) - 1, ID_PNG, PNG));
   types.push_back (ID (0, sizeof (ID_PNG) - 1, ID_PNG, PNG));
   types.push_back (ID (0, 0, "", MP3, &isMP3));
   types.push_back (ID (0, 0, "", OPENOFFICE, &isOOffice));
   types.push_back (ID (0, 0, "", STAROFFICE, &isStarOffice));
   types.push_back (ID (0, 0, "", MSOFFICE, &isMSOffice));
   types.push_back (ID (0, 0, "", HTML, &isHTML));
   types.push_back (ID (0, 0, "", OOXML, &isOfficeOpenXML));
   TRACE9 ("FileTypeCheckerByContent::FileTypeCheckerByContent () - Known types " << types.size ());
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
FileTypeCheckerByContent::~FileTypeCheckerByContent () {
}


//-----------------------------------------------------------------------------
/// Returns the class of the file (e.g. MS Office document) according
/// to passed file-name
/// \param file File to inspect
/// \returns unsigned int Class of file
//-----------------------------------------------------------------------------
unsigned int FileTypeCheckerByContent::getType (const char* file) const {
   Check1 (file);
   TRACE1 ("FileTypeCheckerByContent::getType (const char*) const - Checking " << file);

   std::ifstream stream (file);
   if (stream) {
      // Fill buffer with file
      char buffer[512];
      memset (buffer, 0, sizeof (buffer));
      stream.read (buffer, sizeof (buffer));

      // Check if first bytes identify the file
      for (std::vector<ID>::const_iterator i (types.begin ()); i != types.end (); ++i) {
	 TRACE9  ("FileTypeCheckerByContent::getType (const char*) - Type " << (i - types.begin ()));
	 unsigned int len (i->text.length () ? i->text.length () : sizeof (buffer));
	 if (((i->start + len) > stream.tellg ())
	     || (i->start != ((unsigned int)stream.tellg () - sizeof (buffer)))) {
	    TRACE3 ("FileTypeCheckerByContent::getType (const char*) - Skip to " << i->start);
	    stream.seekg (i->start, std::ios::beg);
	    stream.read (buffer, sizeof (buffer));
	 }

	 do {
	    TRACE9 ("Do call: " << (unsigned int)i->fnCheck);
	    if (i->fnCheck (buffer, i->text.data (), len, stream))
	       return i->type;

	    if (len <= sizeof (buffer))
	       break;
	    else {
	       TRACE9 ("FileTypeCheckerByContent::getType (const char*) - Next chunk");
	       stream.read (buffer, sizeof (buffer));
	       len -= sizeof (buffer);
	    }
	 } while (true); // end-do
	 stream.clear ();
      }
   }
   return UNKNOWN;
}

//-----------------------------------------------------------------------------
/// Skips over HTML-comments and white-spaces. The buffer contains afterwards
/// the first charcter not being a white-space or a HTML-comment.
/// If the buffer contains only HTML-comments or white-spaces additional data
/// is read from the passed stream
/// \param buffer Buffer to inspect (and maybe re-fill with new data from stream)
/// \param size Size of buffer
/// \param stream Stream to read from
/// \pre buffer must be at least 4 characters big
//-----------------------------------------------------------------------------
void FileTypeCheckerByContent::skipHTMLComments (char* buffer, unsigned int size,
						 std::ifstream& stream) {
   Check1 (buffer); Check1 (size > 3);

   const char* pos (buffer);
   unsigned int left (size);
   do {
      TRACE5 ("FileTypeCheckerByContent::skipHTMLComments (char*, unsigned int, std::ifstream&) - " << std::string (pos, 5));

      // Simply skip whitespaces
      if (isspace (*pos)) {
	 ++pos;
	 --left;
      }
      // Check for HTML comment (starting with <!--)
      else if ((*pos == '<') && (pos[1] == '!') && (pos[2] == '-') && (pos[3] == '-')) {
	 pos += 4;
	 bool cont (true);
	 do {
	    // Try to find the end of the comment (-->)
	    pos = static_cast<char*> (memchr (pos, '-', left));
	    if (pos) {
	       TRACE7 ("FileTypeCheckerByContent::skipHTMLComments (char*, unsigned int, std::ifstream&) - EOC: \"" << std::string (pos, (left > 4) ? 5 : left) << '"');
	       left = size - (pos - buffer);
	       if (left > 2) {
		  // Found end-of-comment: Stop parsing and copy remaining
		  // characters to the beginning of the buffer
		  if ((pos[1] == '-') && (pos[2] == '>')) {
		     cont = false;
		     memcpy (buffer, pos + 3, left - 3);
		  }
		  else {
		     // Only minus found; skip and continue
		     ++pos;
		     --left;
		     continue;
		  }
	       }
	       else
		  // Reached end of buffer; store last unchecked chars
		  memcpy (buffer, pos, left);
	    }
	    else
	       // No minus found; re-fill the buffer completely
	       left = 0;

	    TRACE9 ("FileTypeCheckerByContent::skipHTMLComments (char*, unsigned int, std::ifstream&) - Filling up: " << (size - left));
	    stream.read (buffer + left, size - left);
	    pos = buffer;
	    left += stream.gcount ();
	 } while (cont);
      } // endif HTML comment
      else {
	 if (pos != buffer) {
	    // No more whitespaces or comments: Copy data to beginning of buffer
	    memcpy (buffer, pos, left);
	    stream.read (buffer + left, size - left);
	 }
	 break;
      }
   } while (left);
}

//-----------------------------------------------------------------------------
/// Checks if the first bytes in buffer machtes the passed value
/// \param buffer Buffer to inspect
/// \param text Text to match
/// \param length Number of bytes to check
/// \param stream Stream from where to read more characters
/// \returns bool True, if the text matches
//-----------------------------------------------------------------------------
bool FileTypeCheckerByContent::matchFirstBytes (char* buffer, const char* text,
						unsigned int length, std::ifstream& stream) {
   return !memcmp (buffer, text, length);
}

//-----------------------------------------------------------------------------
/// Checks if the buffer contains HTML-code; HTML-comments are skipped. If
/// necessary additional data is read from the stream
/// \param buffer Buffer to inspect
/// \param text Text to match
/// \param length Number of bytes to check
/// \param stream Stream from where to read more characters
/// \returns bool True, if the text matches
//-----------------------------------------------------------------------------
bool FileTypeCheckerByContent::isHTML (char* buffer, const char* text,
				       unsigned int length, std::ifstream& stream) {
   // Check for HTML-document; this must be the last test, as the buffer
   // might be updated
   skipHTMLComments (buffer, length, stream);
   TRACE3 (" FileTypeCheckerByContent::isHTML (...): " << std::string (buffer, sizeof (ID_HTML) - 1));
   return !memcmp (buffer, ID_HTML, sizeof (ID_HTML) - 1);
}

//-----------------------------------------------------------------------------
/// Checks if the first two bytes in the buffer are an MP3-ID tag
/// \param buffer Buffer to inspect
/// \param text Text to match
/// \param length Number of bytes to check
/// \param stream Stream from where to read more characters
/// \returns bool True, if the text matches
//-----------------------------------------------------------------------------
bool FileTypeCheckerByContent::isMP3 (char* buffer, const char* text,
				      unsigned int length, std::ifstream& stream) {
   // Check for MP3
   return (get2BytesLSB (buffer) & ID_MP3) == ID_MP3;
}

//-----------------------------------------------------------------------------
/// Checks if the first four bytes in buffer identify a ZIP-archive. If so, check
/// if the file could be an OpenOffice-document
/// \param buffer Buffer to inspect (ignored)
/// \param text Text to match (ignored)
/// \param length Number of bytes to check (ignored)
/// \param stream Stream from where to read more characters
/// \returns bool True, if the text matches
//-----------------------------------------------------------------------------
bool FileTypeCheckerByContent::isOOffice (char* buffer, const char* text,
					  unsigned int length, std::ifstream& stream) {
   return getFileOffsetInArchive (stream, buffer, "meta.xml", 8);
}


//-----------------------------------------------------------------------------
/// Checks if the first bytes in buffer identify a MSOffice-document which also
/// identifies a StarOffice-document. The latter has another identifier at
/// position 0xc2.
/// \param buffer Buffer to inspect
/// \param text Text to match
/// \param length Number of bytes to check
/// \param stream Stream from where to read more characters
/// \returns bool True, if the text matches
//-----------------------------------------------------------------------------
bool FileTypeCheckerByContent::isStarOffice (char* buffer, const char* text,
					     unsigned int length, std::ifstream& stream) {
   // Check for StarOffice/MS-Office (StarOffice up to V5 uses a
   // MS-compatible format additional for the information, additional to
   // their own format, so they could also be parsed as MS-Office document)
   if (!memcmp (buffer, ID_MSOFFICE, sizeof (ID_MSOFFICE) - 1)) {
      char buffer[16];
      stream.seekg (0x8c2, std::ios::beg);
      stream.read (buffer, sizeof (buffer));
      return !memcmp (buffer, ID_STAROFFICE, sizeof (ID_STAROFFICE) - 1);
   }
   return false;
}

//-----------------------------------------------------------------------------
/// Checks if the first bytes in buffer identify a MSOffice-document. As StarOffice-
/// documents have the same identifier, check position 0xc2 of the stream if
/// it does \b not contain an StarOffice-identifier.
/// \param buffer Buffer to inspect
/// \param text Text to match
/// \param length Number of bytes to check
/// \param stream Stream from where to read more characters
/// \returns bool True, if the text matches
//-----------------------------------------------------------------------------
bool FileTypeCheckerByContent::isMSOffice (char* buffer, const char* text,
					   unsigned int length, std::ifstream& stream) {
   // Check for StarOffice/MS-Office (StarOffice up to V5 uses a
   // MS-compatible format additional for the information, additional to
   // their own format, so they could also be parsed as MS-Office document)
   if (!memcmp (buffer, ID_MSOFFICE, sizeof (ID_MSOFFICE) - 1)) {
      char buffer[16];
      stream.seekg (0x8c2, std::ios::beg);
      stream.read (buffer, sizeof (buffer));
      return memcmp (buffer, ID_STAROFFICE, sizeof (ID_STAROFFICE) - 1);
   }
   return false;
}

//-----------------------------------------------------------------------------
/// Checks if the passed stream is an MS-Office 2007 document
/// \param buffer Buffer to inspect (ignored)
/// \param text Text to match (ignored)
/// \param length Number of bytes to check (ignored)
/// \param stream Stream from where to read more characters
/// \returns bool True, if the text matches
//-----------------------------------------------------------------------------
bool FileTypeCheckerByContent::isOfficeOpenXML (char* buffer, const char* text,
						unsigned int length, std::ifstream& stream) {
   TRACE1 (" FileTypeCheckerByContent::isOfficeOpenXML (...)");
   return getFileOffsetInArchive (stream, buffer, "docProps/core.xml", 17);
}

}
