// $Id: XStrBuf.cpp,v 1.30 2003/11/14 20:27:55 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : XStrBuf - Extended streambuf
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.30 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.7.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "YGP/Check.h"
#include "YGP/Trace_.h"

#include "YGP/XStrBuf.h"


namespace YGP {

static unsigned int lenBuffer = 512;


//-----------------------------------------------------------------------------
/// Defaultconstructor; Although reading from this object should work and
/// should also return some "random" data, to provide a "real" data-sink is
/// highly recommended. This method might be declared private (or at least
/// protected) in the future.
//-----------------------------------------------------------------------------
extStreambuf::extStreambuf ()
   : line (0), pushbackOffset (-1), pSource (NULL)
   , pBuffer (static_cast <char*> (malloc (lenBuffer))) {
   setbuf (pBuffer, lenBuffer);
}

//-----------------------------------------------------------------------------
/// Constructur; creates an extended streambuf which takes its input from the
/// provided source.
/// \param source: Actual datasink to use
//-----------------------------------------------------------------------------
extStreambuf::extStreambuf (std::streambuf& source)
   : line (0), pushbackOffset (-1), pSource (&source)
   , pBuffer (static_cast <char*> (malloc (lenBuffer))) {
   setbuf (pBuffer, lenBuffer);
}

//-----------------------------------------------------------------------------
/// Constructur; creates an extended streambuf which takes its input from the
/// provided source.
/// \param source: Actual datasink to use
//-----------------------------------------------------------------------------
extStreambuf::extStreambuf (std::streambuf* source)
   : line (0), pushbackOffset (-1), pSource (source)
   , pBuffer (static_cast <char*> (malloc (lenBuffer))) {
   setbuf (pBuffer, lenBuffer);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
extStreambuf::~extStreambuf () {
   free (pBuffer);  //  TODO!!     Might be done by streambuf (?) Check sources
}


//-----------------------------------------------------------------------------
/// Called if output would cause the streambuf to overrun. Because this class
/// is designed for input, this method shouldn't be called.
/// \param ch: Char to write, causing the overflow
/// \returns \c int: EOF in case of error
//-----------------------------------------------------------------------------
int extStreambuf::overflow (int) {
   Check (0);
   return EOF;
}

//-----------------------------------------------------------------------------
/// Called if the object doesn't contain data in the input-buffer. It then
/// copies characters from its data-sink (the streambuf passed while
/// constructing or defined with setSource) til the next line-feed character
/// (10, 0x0a, '\n') and sets this data as its input. This method is called
/// automatically and should NOT be used. It also might be declared protected
/// in the future.
/// \returns \c int: EOF in case of an error, else 0
/// \pre Readpointer equal or behind end-of-readbuffer
//-----------------------------------------------------------------------------
int extStreambuf::underflow () {
   if (gptr () < egptr ()) // Sanity-check; VC uses underflow to get curr char
      return *gptr ();

   TRACE7 ("extStreambuf::underflow () - Act: " << std::hex << (int)gptr ()
          << "; End: " << (int)egptr () << std::dec);
   Check1 (!checkIntegrity ());

   char* pTemp = pBuffer;
   int   ch;

   ++line;
   while ((ch = pSource->sbumpc ()) != EOF) {
      if (pTemp >= (pBuffer + lenBuffer)) {               // Buffer to small?
         Check3 (pTemp == (pBuffer + lenBuffer));          // Double its size
         pTemp = pBuffer;
         pBuffer = static_cast <char*> (malloc (lenBuffer << 1));
         memcpy (pBuffer, pTemp, lenBuffer);          // & copy old contents
         free (pTemp);
         pTemp = pBuffer + lenBuffer;
         lenBuffer <<= 1; Check3 (lenBuffer);
         setg (pBuffer, pBuffer + lenBuffer, pBuffer + lenBuffer);
      }
      *pTemp++ = (char)ch;
      TRACE8 ("New char: " << (char)ch);

      if ((char)ch == '\n')
         break;
   } // end-while !EOF

   pushbackOffset = -1 - int (pTemp - pBuffer);
   setg (pBuffer, pBuffer, pTemp);
   return (pTemp == pBuffer) ? EOF : (int (*pBuffer) & 0xff);
}

//-----------------------------------------------------------------------------
/// Manages the failure of a pushback of a character into the stream. There
/// are two reasons why this method could be called while pushing back
/// characters:
///    - A character is pushed back which doesn't fit the read character. This
///      is propably due to a programming failure and the pushback is not
///      performed. For the sake of performance this the class ignores that
///      case.
///    - A buffer underrun; caused by pushing characters back into the
///      previous line (and - depending on the implementation of streambuf -
///      maybe a few characters more). In this read-position of the
///      data-sink is changed to the end of the previous line.
/// \param c: Character to put back (not EOF)
/// \returns \c Character putted back (EOF if an error occured)
/// \remarks For the sake of performance the check if the character pushed
///     back matches the character in the stream is \b not performed (but the
///     next read-operation returns the "right" character)!
//-----------------------------------------------------------------------------
int extStreambuf::pbackfail (int c) {
   TRACE2 ("extStreambuf::pbackfail (int)");
   Check1 (!checkIntegrity ());
   Check1 (c != EOF);

   if (gptr () > eback ())        // gptr () > eback -> pushback of wrong char
      return EOF;

   TRACE5 ("extStreambuf::pbackfail (int) - Buffer underrun");

   int rc (pSource->pubseekoff (pushbackOffset, std::ios_base::cur));
   pushbackOffset = -1;
   if (rc == EOF)
      return EOF;

#if TRACELEVEL > 8
   TRACE ("extStreambuf::pbackfail (int) - Next = " << (rc = pSource->sbumpc ()));
   Check3 (rc = c);
   pSource->pubseekoff (-1, std::ios_base::cur);
#endif

   setg (NULL, NULL, NULL);
   Check3 (line != 0);
   if (c == '\n')
      --line;
   return c;
}

//-----------------------------------------------------------------------------
/// Positionates the read pointer to the passed offset (either starting from
/// the beginning or the end of the stream or from the current position).
/// \param off: Offset to change in the stream
/// \param dir: Direction to change offset to
/// \param mode: Which pointer to move (get, put)
/// \returns \c New position in the stream
//-----------------------------------------------------------------------------
std::streampos extStreambuf::seekoff (std::streamoff off,
                                      std::ios_base::seekdir dir,
                                      std::ios_base::openmode mode) {
   TRACE7 ("extStreambuf::seekoff (streamoff, _seek_dir, mode) - " << off
           << "; " << dir << '/' << mode);
   Check1 (pSource);

   // Correct offset, if positionate relative to current position as
   // pSource is already further (at end of line)
   if (dir == std::ios_base::cur)
      off -= (egptr () - gptr ());
   TRACE7 ("extStreambuf::seekoff (streamoff, _seek_dir, mode) - New value: " << off);
   setg (NULL, NULL, NULL);
   return off ? pSource->pubseekoff (off, dir, mode) : std::streampos (0);
}

//-----------------------------------------------------------------------------
/// Positionates the read pointer to the passed position.
/// \param pos: New position in the stream
/// \param mode: Which pointer to move (get, put)
/// \returns \c New position in the stream
//-----------------------------------------------------------------------------
std::streampos extStreambuf::seekpos (std::streampos pos,
                                      std::ios_base::openmode mode) {
   TRACE7 ("extStreambuf::seekpos (streampos, mode)");
   Check1 (pSource);
   setg (NULL, NULL, NULL);
   return pSource->pubseekpos (pos, mode);
}

//-----------------------------------------------------------------------------
/// Checks the integrity of the object
/// \returns \c int: Status: 0 OK; 1 \c pSource == NULL; 2 \c pBuffer == NULL
//-----------------------------------------------------------------------------
int extStreambuf::checkIntegrity () const {
   return pBuffer ? pSource ? 0 : 1 : 2;
}

}
