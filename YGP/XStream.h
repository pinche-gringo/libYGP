#ifndef XSTREAM_H
#define XSTREAM_H

// $Id: XStream.h,v 1.18 2003/11/14 20:27:55 markus Rel $

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


#include <fstream>

#if SYSTEM == WINDOWS
#  include <iosfwd>
#endif

#include <YGP/XStrBuf.h>


namespace YGP {

/** Extended stream, designed to parse text.

   It overcomes two drawbacks of the original streams:

  - Column- and line-information (which - to be fair - wouldn't
    make very much sense in the common streams anyway)
  - A variable-sized putback-buffer. The common streams only
    stores one block (and a few bytes from the last), so a pushback
    beyond this border is not possible.

   \note The extensions of the stream must be initialized with the
      init()-function (after creating/assigning)!
*/
// Example:
//    Xifstream xin;
//    xin.open ("Test.Dat");
//    xin.init ();
template <class T> struct extStream : private extStreambuf, public T {
 public:
   // Management-functions
   /// Default constrcutor; Creates a stream
   extStream () : extStreambuf (), T (), oldBuf (NULL) { }
   /// Constructor; Creates a stream with a data sink (to read from)
   extStream (const T& source) : extStreambuf (), T (source), oldBuf (NULL) { }
   /// Destructor
   ~extStream () { sios::rdbuf (oldBuf); }

   /// Initializes the extended stream
   void init () {
      setSource (oldBuf = rdbuf ());
      sios::rdbuf (this);
   }

   /// \name Accessing stream values
   //@{
   /// Returns the actual line of the stream read from
   unsigned int getLine () const { return extStreambuf::getLine (); }
   unsigned int getColumn () const { return extStreambuf::getColumn (); }
   //@}

 private:
   // Prohibited manager functions
   extStream (const extStream&);
   const struct extStream& operator= (const extStream&);

   std::streambuf* oldBuf;

   // Used this type to work around Visual C's problem with calling ios::rdbuf
   typedef std::ios sios;
};

typedef extStream<std::istream> Xistream;
typedef extStream<std::ifstream> Xifstream;

}

#endif
