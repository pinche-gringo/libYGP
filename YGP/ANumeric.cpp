//$Id: ANumeric.cpp,v 1.2 1999/09/05 01:33:54 Markus Rel $

//PROJECT     : General
//SUBSYSTEM   : ANumeric
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.7.1999
//COPYRIGHT   : Anticopyright (A) 1999

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


#include <locale.h>

#include <iomanip>
#include <iostream.h>
#ifdef UNIX
#include <strstream.h>
#else
#include <strstrea.h>
#endif

#include <string>

#include "ANumeric.h"


// Prototypes
static void print (ostream& out, long outValue);


// Static (global) variables
struct lconv* loc (localeconv ());


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
//Purpose   : Converting to a string
//Returns   : String-representation of ANumeric
/*--------------------------------------------------------------------------*/
ANumeric::operator std::string () const {
   char buffer[20] = "";
   if (isDefined ()) {
      ostrstream temp (buffer, sizeof (buffer));
      print (temp, value);
   }
   return buffer;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Output of ANumeric
//Parameter : out: Out-stream
//            outValue: ANumeric to print
//Returns   : Reference to out
/*--------------------------------------------------------------------------*/
ostream& operator<< (ostream& out, const ANumeric& outValue) {
   if (outValue.isDefined ()) {
      std::string temp (outValue);
      out << temp;
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
      assert (loc);
      out << loc->thousands_sep << setw (3) << setfill ('0');
      outValue %= 1000;
   }
   out << outValue;
}

