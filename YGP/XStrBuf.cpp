// $Id: XStrBuf.cpp,v 1.1 1999/07/31 00:15:08 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : XStrBuf - Extended streambuf
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.7.1999
//COPYRIGHT   : Anticopyright (A) 1999

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#ifndef DEBUG
#define DEBUG 9
#endif


#include <stdio.h>
#include <assert.h>

#if DEBUG > 0
#include <iostream.h>
#endif

#include "XStrBuf.h"


unsigned int extStreambuf::iLenPutback = 64;


//Description: Default-constructur; Initializes object
extStreambuf::extStreambuf ()
   : line (1), column (1), puttedBack (false), pBuffer (NULL)
   , pEndBuffer (pBuffer) {
   assert (pSource);
}

//Description: Constructur; Initializes object
//Parameters : source: Original streambuffer, which should be enhanced
extStreambuf::extStreambuf (streambuf& source)
   : streambuf (source), line (1), column (1), puttedBack (false)
   , pBuffer (NULL), pEndBuffer (pBuffer) {
   assert (pSource);
}

//Description: Destructor
extStreambuf::~extStreambuf () {
   delete [] pBuffer;
}


//Description: Underflow of buffer; load new data into buffer
//Returns    : int: EOF in case of error
//Requires   : Readpointer equal or behind end-of-readbuffer
//Returns    : Status; 0 OK
int extStreambuf::underflow () {
#if DEBUG > 2
   cout << "Underflow!\n";
#endif

   assert (gptr () >= egptr ());
   assert (pEndBuffer <= pBuffer + iLenPutback);

   if (puttedBack) {     // Chars putted back -> Use this buffer as new input
#if DEBUG > 5
      cout << "Underflow! -> Use putback\n";
#endif
      exchangeBuffer ();
      pEndBuffer = pBuffer;
      puttedBack = false;
   }
   else
      return parent::underflow ();
   return 0;
}

//Description: Pushback of last read character failed. In this case the char
//             is stored in a special pusback-buffer for further reading. If
//             the putback-buffer is also overflown it is enhanced
//Parameters : c: Character to put back (not EOF)
//Returns    : Status; 0 OK
int extStreambuf::pbackfail (int c) {
#if DEBUG > 2
   cout << "Failed pushback!\n";
#endif

   assert (!checkIntegrity ());
   assert (c != EOF);

   puttedBack = true;

   if (!pBuffer) 
      pBuffer = pEndBuffer = new char [iLenPutback];
   else {
     if (pEndBuffer >= pBuffer + iLenPutback) {
       char* pHelp (new char [iLenPutback << 1]);
       memcpy (pHelp, pBuffer, iLenPutback);
       delete [] pBuffer;
       pBuffer = pHelp;
       pEndBuffer = pBuffer + iLenPutback;
       iLenPutback <<= 1;
     }

   }
   *pEndBuffer++ = (char)c;
   exchangeBuffer ();
}

//Description: Sets the readbuffer to the passed values and returns the
//             old buffer-values
void extStreambuf::exchangeBuffer () {
   assert (!checkIntegrity ());

   char* pTemp (Gbase ()); assert (pTemp);
   char* pTempEnd (eGptr ()); assert (pTempEnd);

   setg (pBuffer, pBuffer, pEndBuffer);
   pBuffer = pTemp;
   pEndBuffer = pTempEnd;
}

//Description: Checks the integrity of the object
//Returns    : Status: 0 OK
bool extStreambuf::checkIntegrity () const {
   assert (pEndBuffer >= pBuffer);
   assert (pEndBuffer <= pBuffer + iLenPutback);
   assert ((puttedBack && pBuffer && pEndBuffer)
	   || !(puttedBack || pBuffer || pEndBuffer));
   return 0;
}
