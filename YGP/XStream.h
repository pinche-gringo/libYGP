#ifndef XSTREAM_H
#define XSTREAM_H

// $Id: XStream.h,v 1.2 1999/08/23 20:27:20 Markus Rel $

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


#include <assert.h>

#include <fstream.h>

#include <XStrBuf.h>


// Extended stream, specialized to parse text. It enhanced features are
// column- and line-information and a variable-sized putback-buffer (putback
// beyond block-size possible).
//
// Note: After creating (assigning) the istream-part of the extStream the
//       init-function must be called to set the extStreambuf, because
//       other istream-methods might change/set its streambuf.
//
// Example:
//    Xifstream xin;
//    xin.open ("Test.Dat");
//    xin.init ();
template <class T> struct extStream : public T  {
   typedef T parent;

   // Manager-functions
   extStream () : pBuffer (NULL) { }
   extStream (T& source) : pBuffer (NULL) { }
   ~extStream () { delete pBuffer; }

   void init () {
      delete pBuffer;
      pBuffer = new extStreambuf (*rdbuf ());
      ios::rdbuf (pBuffer);
   }

   // Accessing values
   unsigned int getLine () const { assert (pBuffer); return pBuffer->getLine (); }
   unsigned int getColumn () const { assert (pBuffer); return pBuffer->getColumn (); }

 private:
   // Prohibited manager functions
   extStream (const extStream&);
   const struct extStream& operator= (const extStream&);

   int checkIntegrity () const { return pBuffer; }

   extStreambuf* pBuffer;
};


typedef extStream<istream> Xistream;
typedef extStream<ifstream> Xifstream;

#endif
