// $Id: XStrBuf.cpp,v 1.21 2002/11/18 07:29:39 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : XStrBuf - Extended streambuf
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.21 $
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

#include "Check.h"
#include "Trace_.h"

#include "XStrBuf.h"

static unsigned int lenBuffer = 512;

#if SYSTEM == WINDOWS
#  define cur std::ios_base::cur
#else
#  define cur ios::cur
#endif

/*--------------------------------------------------------------------------*/
//Purpose   : (Default-)Constructur; Initializes object
/*--------------------------------------------------------------------------*/
extStreambuf::extStreambuf ()
   : line (0), pushbackOffset (-1), pSource (NULL)
   , pBuffer (static_cast <char*> (malloc (lenBuffer))) {
   setbuf (pBuffer, lenBuffer);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructur; Initializes object
//Parameter : source: Original streambuffer, which should be enhanced
/*--------------------------------------------------------------------------*/
extStreambuf::extStreambuf (streambuf& source)
   : line (0), pushbackOffset (-1), pSource (&source)
   , pBuffer (static_cast <char*> (malloc (lenBuffer))) {
   setbuf (pBuffer, lenBuffer);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructur; Initializes object
//Parameter : source: Original streambuffer, which should be enhanced
/*--------------------------------------------------------------------------*/
extStreambuf::extStreambuf (streambuf* source)
   : line (0), pushbackOffset (-1), pSource (source)
   , pBuffer (static_cast <char*> (malloc (lenBuffer))) {
   setbuf (pBuffer, lenBuffer);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor

/*--------------------------------------------------------------------------*/
extStreambuf::~extStreambuf () {
   free (pBuffer);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Overflow of buffer
//Parameters: ch: Char to write, causing the overflow
//Returns   : int: EOF in case of error
//Requires  : Readpointer equal or behind end-of-readbuffer
/*--------------------------------------------------------------------------*/
int extStreambuf::overflow (int ch) {
   Check (0);
   return EOF;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Underflow of buffer; load new data into buffer
//Returns   : int: EOF in case of error
//Requires  : Readpointer equal or behind end-of-readbuffer
/*--------------------------------------------------------------------------*/
int extStreambuf::underflow () {
   TRACE2  ("extStreambuf::underflow ()");

   if (gptr () < egptr ()) // Sanity-check; VC uses underflow to get curr char
      return *gptr ();

   Check3 (!checkIntegrity ());

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
      TRACE9 ("New char: " << (char)ch);

      if ((char)ch == '\n')
         break;
   } // end-while !EOF

   pushbackOffset = -1 - int (pTemp - pBuffer);
   setg (pBuffer, pBuffer, pTemp);
   return (pTemp == pBuffer) ? EOF : 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Pushback of last read character failed (because the beginning
//            of the actual block was reached) -> Repositionate the file-ptr
//            one char before the last one.
//Parameter : c: Character to put back (not EOF)
//Returns   : Character putted back (EOF if error)
/*--------------------------------------------------------------------------*/
int extStreambuf::pbackfail (int c) {
   TRACE2 ("extStreambuf::pbackfail (int)");

   Check3 (!checkIntegrity ());
   Check3 (c != EOF);

#if SYSTEM == WINDOWS
   if (gptr () > eback ())        // gptr () > eback -> pushback of wrong char
#else
   if (gptr () > Gbase ())        // gptr () > Gbase -> pushback of wrong char
#endif
      return EOF;

   TRACE5 ("extStreambuf::pbackfail (int) - Buffer underrun");

   int rc (pSource->pubseekoff (pushbackOffset, cur));
   pushbackOffset = -1;
   if (rc == EOF)
      return EOF;

#if TRACELEVEL > 8
   TRACE ("extStreambuf::pbackfail (int) - Next = " << (rc = pSource->sbumpc ()));
   Check3 (rc = c);
   pSource->pubseekoff (-1, cur);
#endif

   setg (NULL, NULL, NULL);
   Check3 (line != 0);
   if (c == '\n')
      --line;
   return c;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the integrity of the object
//Returns   : Status: 0 OK; 1 pSource == NULL; 2 pBuffer == NULL
/*--------------------------------------------------------------------------*/
streampos extStreambuf::seekoff (streamoff off, _seek_dir dir, int mode) {
   TRACE8 ("extStreambuf::seekoff (streamoff, _seek_dir, mode) - " << off
           << "; " << dir << '/' << mode);
   Check3 (pSource);

   // Correct offset, if positionate relative to current position as
   // pSource is already further (at end of line)
   if (dir == ios::cur)
      off -= (egptr () - gptr ());
   streampos pos (pSource->pubseekoff (off, dir, mode));
   setg (pBuffer, pBuffer + lenBuffer, pBuffer + lenBuffer);
   return pos;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the integrity of the object
//Returns   : Status: 0 OK; 1 pSource == NULL; 2 pBuffer == NULL
/*--------------------------------------------------------------------------*/
streampos extStreambuf::seekpos (streampos pos, int mode) {
   TRACE8 ("extStreambuf::seekpos (streampos, mode)");
   Check3 (pSource);
   setg (pBuffer, pBuffer, pBuffer);
   return pSource->pubseekpos (pos, mode);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the integrity of the object
//Returns   : Status: 0 OK; 1 pSource == NULL; 2 pBuffer == NULL
/*--------------------------------------------------------------------------*/
int extStreambuf::checkIntegrity () const {
   return pBuffer ? pSource ? 0 : 1 : 2;
}
