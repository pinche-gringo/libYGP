#ifndef XSTRBUF_H
#define XSTRBUF_H

// $Id: XStrBuf.h,v 1.12 2001/08/27 15:31:48 markus Exp $

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

#include <assert.h>

#if SYSTEM == UNIX || defined __GNUG__
#  include <streambuf.h>
#else
#  include <streamb.h>
#endif


// Extended streambuf, specialized to parse text. Its enhanced features are
// column- and line-information and a variable-sized putback-buffer (putback
// beyond block-size possible)
// ATTENTION: A wee bit of overhandling neccessary! Although extStreambuf is
// derived from streambuf another streambuf* as member is needed (at least
// I was not able to figure out another way)
struct extStreambuf : public streambuf {
   typedef streambuf parent;

   // Manager-functions
   extStreambuf ();
   extStreambuf (streambuf& source);
   extStreambuf (streambuf* source);
   virtual ~extStreambuf ();

   // Error-handling (buffer over/underflow and putback-failure)
   virtual int overflow (int ch);
   virtual int underflow ();
   virtual int pbackfail (int c);

   // Setting of data-sink
   void setSource (streambuf* source) { assert (pSource); pSource = source; }
   void setSource (streambuf& source) { pSource = &source; }

   // Accessing values
   unsigned int getLine () const { return line; }
   unsigned int getColumn () const {
      return (base () > gptr ()) ? 0 : gptr () - base ()
#ifdef __BORLANDC__
	- 1;                        // BCC's gptr () points to next position
#else
      ;
#endif
   }

 private:
   // Prohibited manager functions
   extStreambuf (const extStreambuf&);
   const struct extStreamBuf& operator= (const extStreambuf&);

   int checkIntegrity () const;

   unsigned int line;
   int          pushbackOffset;

   streambuf* pSource;
   char*      pBuffer;
};


#endif
