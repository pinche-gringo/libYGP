#ifndef XSTRBUF_H
#define XSTRBUF_H

// $Id: XStrBuf.h,v 1.24 2003/11/16 19:25:55 markus Rel $

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


#include <ygp-cfg.h>

#include <ios>
#include <streambuf>

#if SYSTEM == WINDOWS
#  include <iosfwd>
#endif

#include <YGP/Check.h>


namespace YGP {

/**Extended streambuf, designed to parse text.

   It overcomes two drawbacks of the original streambuf:

     - Column  and line information (which - to be fair - wouldn't
       make very much sense in the common streambuf anyway)
     - A variable-sized putback-buffer. The common streambuf only
       stores one block (and a few bytes from the last), so a pushback
       beyond this border is not possible.

   \note This buffer only works for input and needs a "real" data-sink
         as source, so provide another streambuf-object to get its data
         from with the constructor or with the setSource-method!

   Also note that after repositionating in the stream (with seekoff/seekpos)
   the line-information might not reflect the truth anymore (and also not
   the column, at least til the next end-of-line is reached).
*/
/* ATTENTION: A wee bit of overhandling neccessary! Although extStreambuf is
   derived from streambuf another streambuf* as member is needed (at least
   I was not able to figure out another way)
*/
class extStreambuf : public std::streambuf {
 public:
   // Manager-functions
   extStreambuf ();
   extStreambuf (std::streambuf& source);
   extStreambuf (std::streambuf* source);
   virtual ~extStreambuf ();

   /// \name Exception-handling
   //@{
   virtual int overflow (int ch);
   virtual int underflow ();
   virtual int pbackfail (int c);
   //@}

   /// \name Position handling
   //@{
   virtual std::streampos seekoff (std::streamoff, std::ios_base::seekdir,
                                   std::ios_base::openmode mode = std::ios::in|std::ios::out);
   virtual std::streampos seekpos (std::streampos pos,
                                   std::ios_base::openmode  mode = std::ios::in|std::ios::out);
   //@}

   /// \name Setting of data-sink
   //@{
   /// Sets the data sink to read from
   void setSource (std::streambuf* source) { Check1 (source); pSource = source; }
   /// Sets the data sink to read from
   void setSource (std::streambuf& source) { pSource = &source; }
   //@}

   /// \name Accessing stream values
   //@{
   /// Returns the actual line of the stream read from
   unsigned int getLine () const { return line; }
   /// Returns the actual column of the stream read from
   unsigned int getColumn () const {
      return (eback () > gptr ()) ? 0 : gptr () - eback ()
#ifdef __BORLANDC__
	- 1;                        // BCC's gptr () points to next position
#else
      ;
#endif
   }
   //@}

 private:
   // Prohibited manager functions
   extStreambuf (const extStreambuf&);
   const struct extStreamBuf& operator= (const extStreambuf&);

   int checkIntegrity () const;

   unsigned int line;
   int          pushbackOffset;

   std::streambuf* pSource;
   char*           pBuffer;
};

}

#endif
