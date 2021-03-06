#ifndef YGP_XSTREAM_H
#define YGP_XSTREAM_H

// $Id: XStream.h,v 1.22 2008/05/18 13:21:27 markus Rel $

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


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
      setSource (oldBuf = T::rdbuf ());
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

/// Declare Xistream as shortcut
typedef extStream<std::istream> Xistream;
/// Declare Xifstream as shortcut
typedef extStream<std::ifstream> Xifstream;

}

#endif
