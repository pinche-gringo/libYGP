// $Id: XStrBuf.cpp,v 1.3 1999/08/23 17:55:30 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : XStrBuf - Extended streambuf
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.7.1999
//COPYRIGHT   : Anticopyright (A) 1999

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


#ifndef DEBUG
#define DEBUG 0
#endif


#include <stdio.h>
#include <assert.h>

#if DEBUG > 0
#include <iomanip.h>
#include <iostream.h>
#endif

#include "XStrBuf.h"


static int lenBuffer = 512;


/*--------------------------------------------------------------------------*/
//Purpose   : Constructur; Initializes object
//Parameter : source: Original streambuffer, which should be enhanced
/*--------------------------------------------------------------------------*/
extStreambuf::extStreambuf (streambuf& source)
   : streambuf (source), line (0), pushbackOffset (-1), pSource (&source)
   , pBuffer (new char[lenBuffer]) {
   setb (pBuffer, pBuffer + lenBuffer, 1);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
extStreambuf::~extStreambuf () {
   delete [] pBuffer;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Underflow of buffer; load new data into buffer
//Returns   : int: EOF in case of error
//Requires  : Readpointer equal or behind end-of-readbuffer
/*--------------------------------------------------------------------------*/
int extStreambuf::underflow () {
#if DEBUG > 2
   cout << "Underflow!\n";
#endif

   assert (!checkIntegrity ());

   char *pTemp (pBuffer);
   int ch;

   ++line;
   pushbackOffset = -1;
   while ((ch = pSource->sbumpc ()) != EOF) {
      --pushbackOffset;
      if (pTemp == pBuffer + lenBuffer) {                 // Buffer to small?
         pBuffer = new char[lenBuffer << 1];           // Double its size and
         memcpy (pBuffer, base(), lenBuffer);            // copy old contents
         pTemp = pBuffer + lenBuffer;
         setb (pBuffer, pBuffer + lenBuffer, 1);
         lenBuffer <<= 1;
      }
      *pTemp++ = ch;

      if (ch == '\n')
	 break;
   } // end-while !EOF

   setg (pBuffer, pBuffer, pTemp);
   return ch;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Pushback of last read character failed (because the beginning
//            of the actual block was reached) -> Repositionate the file-ptr
//            one char before the last one.
//Parameter : c: Character to put back (not EOF)
//Returns   : Character putted back (EOF if error)
/*--------------------------------------------------------------------------*/
int extStreambuf::pbackfail (int c) {
#if DEBUG > 2
   cout << "Failed pushback!\n";
#endif

   assert (!checkIntegrity ());
   assert (c != EOF);

   if (gptr () > Gbase ())        // gptr () > Gbase -> pushback of wrong char
      return EOF;

#if DEBUG > 5
   cout << "Failed pushback: Buffer underrun\n";
#endif

   int rc (pSource->seekoff (pushbackOffset, ios::cur));
   pushbackOffset = -1;
   if (rc == EOF)
      return EOF;

#if DEBUG > 8
   cout << "Pushback: Next = " << (rc = pSource->sbumpc ()) << endl;
   assert (rc = c);
   pSource->seekoff (-1, ios::cur);
#endif

   setg (NULL, NULL, NULL);
   assert (line != 0);
   if (c == '\n')
      --line;
   return c;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the integrity of the object
//Returns   : Status: 0 OK
/*--------------------------------------------------------------------------*/
int extStreambuf::checkIntegrity () const {
   if (!pBuffer)
      return 1;

   if (gptr () > egptr ())
      return 2;

   if (!pSource)
      return 3;

   return 0;
}
