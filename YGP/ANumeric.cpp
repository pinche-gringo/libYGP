//$Id: ANumeric.cpp,v 1.1 1999/07/31 00:15:08 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Tokenize
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.7.1999
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

#include <iostream.h>
#ifdef UNIX
#include <strstream.h>
#else
#include <strstrea.h>
#endif

#include "ANumeric.h"

static void print (ostream& out, long outValue);


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
ANumeric::~ANumeric () {
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: other: Object to copy
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator= (const ANumeric& other) {
   if (this != &other) {
      value = other.value;
      AttributValue::operator= ((const AttributValue&) other);
   }
   return *this;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Output of ANumeric
//Parameter : out: Out-stream
//            outValue: ANumeric to print
//Returns   : Reference to out
/*--------------------------------------------------------------------------*/
ostream& operator<< (ostream& out, const ANumeric& outValue) {
   if (outValue.isDefined ()) {
      char buffer[20];
      ostrstream temp (buffer, sizeof (buffer));

      print (temp, outValue);
      temp << ends;
      out << buffer;
   }
   return out;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Helper-function for formatted printing of a long-value
//Parameter : out: Out-stream
//            outValue: long to print
/*--------------------------------------------------------------------------*/
void print (ostream& out, long outValue) {
   if (outValue >= 1000) {
      print (out, outValue / 1000);
      out << '.';
      outValue %= 1000;
   }
   out << outValue;
}

