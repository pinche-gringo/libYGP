//$Id: ANumeric.cpp,v 1.22 2002/05/24 06:52:49 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ANumeric
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.22 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.7.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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


#include <errno.h>
#include <assert.h>
#include <locale.h>
#include <limits.h>

#include <gzo-cfg.h>

#ifndef HAVE_LIBGMP
#  include <ctype.h>
#  include <stdlib.h>
#else
#  if HAVE_STRFMON
#    include <monetary.h>
#  endif
#endif

#include <iomanip>
#include <iostream.h>
#if SYSTEM == UNIX || defined __GNUG__
#  include <strstream.h>
#else
#  include <strstrea.h>
#endif

#include <string>
#include <stdexcept>

#include "Trace_.h"
#include "Internal.h"

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
ANumeric& ANumeric::operator= (const char* pValue) throw (invalid_argument) {
   assert (pValue);

#ifdef HAVE_LIBGMP
   if (mpz_init_set_str (value, pValue, 0))
#else
   char* pTail = NULL;
   errno = 0;
   value = strtol (pValue, &pTail, 0);
   if (errno || (pTail && *pTail))
#endif
      undefine ();
   else
      setDefined ();

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
   setDefined ();
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
std::string ANumeric::toUnformatedString () const {
   std::string str ("");

   if (isDefined ()) {
#ifdef HAVE_LIBGMP
      char* pString (mpz_get_str (NULL, 10, value)); assert (pString);
#else
      char* pString = new char [40];
      ostrstream ostr (pString, 16);
      ostr << value << '\0';
#endif
      TRACE1 ("ANumeric::toUnformatedString -> value = " << pString);

      str = pString;                 // Copy unformatted string to return-value
      free (pString);
   }

   return str;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string
//Returns   : String-representation of ANumeric
/*--------------------------------------------------------------------------*/
std::string ANumeric::toString () const {
   TRACE9 ("ANumeric::toString () const");
   std::string str;

   if (!loc)
      loc = localeconv ();                 // Get locale-information if not set

   TRACE9 ("ANumeric::toString () const - Locale-info = " << loc->grouping
           << " - " << loc->thousands_sep);

   str = toUnformatedString ();
   TRACE5 ("ANumeric::toString () const - " << str);

   int len (str.length ());
   int index (0);
   char group (loc->grouping[index]);
   char* pSep = loc->thousands_sep;
   if (!group)
      group = CHAR_MAX;

   while ((group != CHAR_MAX) && (len > group)) {     // Check if grouping nec.
      TRACE9 ("ANumeric::toString () const - Len =  " << len << "; Group = "
              << group << "; Index = " << index);
      len -= group;
      str.replace (len, 0, pSep, 1);
      TRACE8 ("ANumeric::toString () const - Inserted " << str);

      if (loc->grouping[index + 1]) { // Increment group-pointer if more groups
	 group = loc->grouping[++index];
         if (pSep[1])
            ++pSep;
      } // endif further grouping available
   } // end-while grouping necessary

   return str;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads string-representation from stream
//Parameters: in: Stream to parse
/*--------------------------------------------------------------------------*/
void ANumeric::readFromStream (istream& in) throw (invalid_argument) {
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
            setDefined ();
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
      setDefined ();
   }
#endif

   if (!in.eof ())
	   throw std::invalid_argument (_("No number"));
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
      setDefined ();
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
      setDefined ();
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
