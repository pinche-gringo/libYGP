#ifndef XSTRBUF_H
#define XSTRBUF_H

// $Id: XStrBuf.h,v 1.4 1999/09/11 00:58:42 Markus Rel $

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


// Only streambuf is needed, but BCC has no own streambuf.h-file; so use iostream.h
// as general basis
#ifdef WINDOWS
#  include <iostream.h>
#else
#  include <streambuf.h>
#endif


// Extended streambuf, specialized to parse text. It enhanced features are
// column- and line-information and a variable-sized putback-buffer (putback
// beyond block-size possible)
// ATTENTION: A wee bit of overhandling neccessary! Although extStreambuf is
// derived from streambuf another streambuf* as member is needed (at least
// I was not able to figure out another way)
struct extStreambuf : public streambuf {
   typedef streambuf parent;

   // Manager-functions
   extStreambuf (streambuf& source);
   virtual ~extStreambuf ();

   // Error-handling (buffer over/underflow and putback-failure)
   virtual int underflow ();
   virtual int pbackfail (int c);

   // Accessing values
   unsigned int getLine () const { return line; }
   unsigned int getColumn () const {
#ifdef WINDOWS
      return gptr () - base () - 1;    // BCC's gptr () points to next position
#else
  return gptr () - base ();
#endif
   }

 private:
   // Prohibited manager functions
   extStreambuf ();
   extStreambuf (const extStreambuf&);
   const struct extStreamBuf& operator= (const extStreambuf&);

   int checkIntegrity () const;

   unsigned int line;
   int          pushbackOffset;

   streambuf* pSource;
   char*      pBuffer;
};


#endif
