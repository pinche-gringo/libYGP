#ifndef XSTRBUF_H
#define XSTRBUF_H

// $Id: XStrBuf.h,v 1.1 1999/07/31 00:15:08 Markus Exp $

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

#include <streambuf.h>


//Description:
// Extends streambuf with a bigger putback-buffer. Additional this buffer
// has line/column-information.
struct extStreambuf : public streambuf {
   typedef streambuf parent;

   // Manager-functions
   extStreambuf ();
   extStreambuf (streambuf& source);
   virtual ~extStreambuf ();

   // Error-handling (buffer over/underflow and putback-failure)
   virtual int underflow ();
   virtual int pbackfail (int c);

 private:
   // Prohibited manager-functions
   extStreambuf (const extStreambuf&);
   const struct extStreamBuf& operator= (const extStreambuf&);

   bool checkIntegrity () const;

   void exchangeBuffer ();

   streambuf*   pSource;
   unsigned int line, column;
   bool         delBuf, puttedBack;
   char*        pBuffer;
   char*        pEndBuffer;

   static unsigned int iLenPutback;
};


#endif
