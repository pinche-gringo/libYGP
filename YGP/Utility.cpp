//$Id$

//PROJECT     : libYGP
//SUBSYSTEM   : YGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision$
//AUTHOR      : Markus Schwab
//CREATED     : 25.08.2008
//COPYRIGHT   : Copyright (C) 2008

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


#include <cstring>
#include <iostream>

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Utility.h"


namespace YGP {

static const unsigned int ID_PKZIP_LOCALHDR       (0x04034b50);
static const unsigned int ID_PKZIP_END_CDR        (0x06054B50);
static const unsigned int ID_PKZIP_CENTRALFILEHDR (0x02014b50);


//-----------------------------------------------------------------------------
/// Checks if the passed stream is a ZIP-archive having the passed file inside
/// \param stream Stream from where to read more characters
/// \param buffer Buffer to inspect
/// \param file Name of file which must be inside the ZIP-file
/// \param length Lenght of file-name
/// \returns unsigned int Offset of file in archive
//-----------------------------------------------------------------------------
unsigned int getFileOffsetInArchive (std::istream& stream, char* buffer,
				     const char* file, unsigned int lenFile) {
   Check1 (buffer); Check1 (file);

   if (get4BytesLSB (buffer) == ID_PKZIP_LOCALHDR) {
      char buffer[80];
      memset (buffer, 0, sizeof (buffer));
      stream.seekg (-22, std::ios::end);
      stream.read (buffer, 22);

      if (get4BytesLSB (buffer) == ID_PKZIP_END_CDR) {
	 // Skip to central directory record
	 unsigned int cEntries (get4BytesLSB (buffer + 10));
	 stream.seekg (get4BytesLSB (buffer + 16), std::ios::beg);
	 TRACE6 ("getFileOffsetInArchive (2x const char*, unsigned int, std::ifstream&) - Start CDR: " << get4BytesLSB (buffer + 16) << " (" << cEntries << ')');

	 // Inspect all entries
	 while (cEntries--) {
	    stream.read (buffer, 46);
	    if (get4BytesLSB (buffer) == ID_PKZIP_CENTRALFILEHDR) {
	       unsigned int lenName (get2BytesLSB (buffer + 28));
	       unsigned int lenSkip (get2BytesLSB (buffer + 30) + get2BytesLSB (buffer + 32));
	       TRACE6 ("getFileOffsetInArchive (2x const char*, unsigned int, std::ifstream&) - Len of filename: " << lenName
		       << "; Skipping: " << lenSkip);

	       // Check if the passed file has been found
	       if (lenName == lenFile) {
		  unsigned int posFile (get4BytesLSB (buffer + 42));
		  stream.read (buffer, lenFile);
		  if (!memcmp (file, buffer, lenFile))
		     return posFile;
		  lenName -= lenFile;
	       }
	       stream.seekg (lenName + lenSkip, std::ios::cur);
	    }
	    else
	       break;
	 } // end-while
      }
   }
   return 0;
}

}
