//$Id: ANumeric.cpp,v 1.10 2000/04/02 16:14:32 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ANumeric
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
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


#include <assert.h>
#include <locale.h>

#ifndef HAVE_LIBGMP
#  include <ctype.h>
#  include <stdlib.h>
#endif

#include <iomanip>
#include <iostream.h>
#ifdef UNIX
#  include <strstream.h>
#else
#  include <strstrea.h>
#endif

#include <string>

#define DEBUG 0
#include "Trace_.h"

#include "ANumeric.h"


// Prototypes
static void print (ostream& out, long outValue);


// Static (global) variables
static struct lconv* loc = NULL;


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
ANumeric::~ANumeric () {
#ifdef HAVE_LIBGMP
   mpz_clear (value);
#endif
}


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: pValue: Pointer to ASCIIZ-string containing numeric value
//Requires  : pValue valid ASCIIZ-string
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator= (const char* pValue) {
   assert (pValue);

#ifdef HAVE_LIBGMP
   if (mpz_init_set_str (value, pValue, 10))
#else
   const char* pTail = NULL;
   value = strtol (pValue, &pTail, 10);
   if (errno || !(pTail && *pTail))
#endif
      undefine ();
   else
      AttributValue::define ();

   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: other: Object to copy
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator= (const ANumeric& other) {
   if (this != &other) {
#ifdef HAVE_LIBGMP
      mpz_set (value, other.value);
#else
      value = other.value;
#endif
      AttributValue::operator= ((const AttributValue&) other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Defining an ANumeric
//Remarks   : If not already done: Initialize locale-definition
/*--------------------------------------------------------------------------*/
void ANumeric::define () {
   AttributValue::define ();
#ifdef HAVE_LIBGMP
   mpz_set_si (value, 0);
#else
   value = 0;
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string
//Returns   : String-representation of ANumeric
/*--------------------------------------------------------------------------*/
std::string ANumeric::toString () const {
   std::string str ("");

   if (isDefined ()) {
      if (!loc)
         loc = localeconv ();             // Get locale-information if not set

#ifdef HAVE_LIBGMP
      char* pString (mpz_get_str (NULL, 10, value)); assert (pString);
#else
      char* pString = new char [16];
      ostrstream ostr (pString, 16);
      ostr << value << '\0';
      
#endif
      TRACE1 ("ANumeric::toString -> value = " << pString);

      str = pString;                 // Copy unformatted string to return-value
      free (pString);

      register int len (str.length ());
      if (len > 3) {
         len = len % 3;                             // Get length of first part
         if (!len)
            len = 3;

         TRACE8 ("ANumeric::toString -> Length of first part = " << len);

         unsigned int lenSep (strlen (loc->thousands_sep));
         while (len < str.length ()) {                // Copy til end-of-string
            assert (str[len + 1]); assert (str[len + 2]);
            str.replace (len, 0, loc->thousands_sep, lenSep);
            len += 3 + lenSep;

            TRACE6 ("ANumeric::toString -> New pos = " << len);
            TRACE7 ("ANumeric::toString -> next format = " << str);
         } // end-while
      } // endif larger than 3 chars -> format
   } // endif defined
   return str;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads string-representation from stream
//Parameters: in: Stream to parse
/*--------------------------------------------------------------------------*/
void ANumeric::readFromStream (istream& in) {
   undefine ();

#ifdef HAVE_LIBGMP
   std::string help;
#else
   value = 0;
#endif
   char ch, chSep;

   in >> ch; assert (!isspace (ch));               // Skip leading whitespaces
   while (!in.eof ()) {
      if (strchr (loc->thousands_sep, ch)) {      // Skip thousand-seperators,
         chSep = ch;
         in.get (ch);
      } // endif
      else
         chSep = '0';

      if (isdigit (ch)) {                // add only digits; else terminate
#ifdef HAVE_LIBGMP
            help += ch;
#else
            AttributValue::define ();
            value *= 10;
            value += ch - '0';
#endif
         }
         else {
            if (chSep != '0')
               in.putback (chSep);
            break;
         } // end-else non-digit found

      in.get (ch);
   } // end-while !eof
   in.putback (ch);

#ifdef HAVE_LIBGMP
   if (!help.empty ()) {
      mpz_set_str (value, help.c_str (), 10);
      AttributValue::define ();
#endif
   }
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: rhs: Value to add
//Returns   : Self
//Note      : If rhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator+= (const ANumeric& rhs) {
   if (rhs.isDefined ()) {
#ifdef HAVE_LIBGMP
      mpz_add (value, value, rhs.value);
#else
      value += rhs.value;
#endif
      AttributValue::define ();
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substract a value from this
//Parameters: rhs: Value to substract
//Returns   : Self
//Note      : If rhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator-= (const ANumeric& rhs) {
   if (rhs.isDefined ()) {
#ifdef HAVE_LIBGMP
      mpz_sub (value, value, rhs.value);
#else
      value -= rhs.value;
#endif
      AttributValue::define ();
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Multiply this with a value
//Parameters: rhs: Value to multiply
//Returns   : Self
//Note      : If rhs is not defined this is not changed; if this is not defined
//            it is set to rhs
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator*= (const ANumeric& rhs) {
   if (rhs.isDefined ())
      if (isDefined ())
#ifdef HAVE_LIBGMP
         mpz_mul (value, value, rhs.value);
#else
         value *= rhs.value;
#endif
      else
         operator= (rhs);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Divides this through a value
//Parameters: rhs: Value to divide with
//Returns   : Self
//Note      : If rhs is not defined this is not changed; if this is not defined
//            it is set to 1/rhs
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator/= (const ANumeric& rhs) {
   if (rhs.isDefined ()) {
      if (!isDefined ())                   // If this is not defined, set to 1
         operator= ((const int)1);

#ifdef HAVE_LIBGMP
      mpz_tdiv_q (value, value, rhs.value);
#else
      value /= rhs.value;
#endif
   } // endif rhs defined
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares two ANumeric-values
//Parameters: other: Object to compare
//Returns   : 1 if this > other; 0 if this == other; -1 else
//Note      : Undefined values are considered as between 0 and -1
/*--------------------------------------------------------------------------*/
int ANumeric::compare (const ANumeric& other) {
   if (isDefined () && other.isDefined ())     // Both sides defined -> compare
#ifdef HAVE_LIBGMP
      return mpz_cmp (value, other.value);
#else
      return (value > other.value) ? 1 : (value < other.value) ? -1 : 0;
#endif

   int rc;
   if (isDefined ())                            // this defined: Compare with 0
#ifdef HAVE_LIBGMP
      return (mpz_cmp_ui (value, 0) < 0) ? -1 : 1;
#else
      return (value < 0) ? -1 : 1;
#endif
   else if (other.isDefined ())                      // Both not defined: Equal
      return 0;
   else
#ifdef HAVE_LIBGMP
      return (mpz_cmp_ui (other.value, 0) < 0) ? 1 : -1;
#else
      return (value < 0) ? 1 : -1;
#endif
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds two ANumeric-values
//Parameters: lhs: Left-hand-side of addition
//            rhs: Right-hand-side of addition
//Returns   : ANumeric: Result of additon
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ANumeric operator+ (const ANumeric& lhs, const ANumeric& rhs) {
   ANumeric result (lhs);
   result += rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substracts two ANumeric-values
//Parameters: lhs: Left-hand-side of substraction
//            rhs: Right-hand-side of substraction
//Returns   : ANumeric: Result of substraction
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ANumeric operator- (const ANumeric& lhs, const ANumeric& rhs) {
   ANumeric result (lhs);
   result -= rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Multiplies two ANumeric-values
//Parameters: lhs: Left-hand-side of mulitplication
//            rhs: Right-hand-side of mulitplication
//Returns   : ANumeric: Result of mulitplication
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ANumeric operator* (const ANumeric& lhs, const ANumeric& rhs) {
   ANumeric result (lhs);
   result *= rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Divides two ANumeric-values
//Parameters: lhs: Left-hand-side of division
//            rhs: Right-hand-side of division
//Returns   : ANumeric: Result of division
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ANumeric operator/ (const ANumeric& lhs, const ANumeric& rhs) {
   ANumeric result (lhs);
   result /= rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Output of ANumeric
//Parameter : out: Out-stream
//            outValue: ANumeric to print
//Returns   : Reference to out
/*--------------------------------------------------------------------------*/
ostream& operator<< (ostream& out, const ANumeric& outValue) {
   if (outValue.isDefined ())
      out << outValue.toString ().c_str ();
   return out;
}
