#ifndef XSTRBUF_H
#define XSTRBUF_H

// $Id: XStrBuf.h,v 1.16 2002/11/18 04:39:40 markus Exp $

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
#elif SYSTEM == WINDOWS
#  include <iosfwd>
#  include <streambuf>

#  ifdef _MSC_VER
      typedef std::ios_base::seekdir _seek_dir;
#  endif
#endif

using namespace std;

// Extended streambuf, designed to parse text.
//
// It overcomes two drawbacks of the original streambuf:
//
//   - Column- and line-information (which - to be fair - wouldn't
//     make very much sense in the common streambuf anyway)
//   - A variable-sized putback-buffer. The common streambuf only
//     stores one block (and a few bytes from the last), so a pushback
//     beyond this border is not possible.
//
// Note: This buffer only works for input and needs a "real" data-sink
//       as source, so provide another streambuf-object to get its data
//       from with the constructor or with the setSource-method!
//
// ATTENTION: A wee bit of overhandling neccessary! Although extStreambuf is
// derived from streambuf another streambuf* as member is needed (at least
// I was not able to figure out another way)
class extStreambuf : public streambuf {
 public:
   // Manager-functions
   extStreambuf ();
   extStreambuf (streambuf& source);
   extStreambuf (streambuf* source);
   virtual ~extStreambuf ();

   // Error-handling (buffer over/underflow and putback-failure)
   virtual int overflow (int ch);
   virtual int underflow ();
   virtual int pbackfail (int c);

   // Position handling
   virtual streampos seekoff(streamoff, _seek_dir, int mode=ios::in|ios::out);
   virtual streampos seekpos(streampos pos, int mode = ios::in|ios::out);

   // Setting of data-sink
   void setSource (streambuf* source) { assert (source); pSource = source; }
   void setSource (streambuf& source) { pSource = &source; }

   // Accessing values
   unsigned int getLine () const { return line; }
   unsigned int getColumn () const {
      return (eback () > gptr ()) ? 0 : gptr () - eback ()
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
