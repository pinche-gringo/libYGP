//$Id: ANumeric.cpp,v 1.4 1999/10/14 22:23:32 Markus Rel $

//PROJECT     : General
//SUBSYSTEM   : ANumeric
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
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
   mpz_clear (value);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: other: Object to copy
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator= (const ANumeric& other) {
   if (this != &other) {
      mpz_set (value, other.value);
      AttributValue::operator= ((const AttributValue&) other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string
//Returns   : String-representation of ANumeric
/*--------------------------------------------------------------------------*/
std::string ANumeric::toString () const {
   std::string str ("");

   if (isDefined ()) {
      char* pString (mpz_get_str (NULL, 10, value)); assert (pString);
      register char* pHelp (pString); assert (pString);

      if (*pHelp == '-')
         str = *pHelp++;

      register int len (strlen (pHelp) % 3);    // Copy first bytes til 3-byte-
      str.replace (str.length (), 0, pHelp, len);  // boundary; split with sep.
      pHelp += len;
      len = strlen (loc->thousands_sep);
      while (*pHelp) {                                // Copy til end-of-string
         str.replace (str.length (), 0, loc->thousands_sep, len);

	 assert (pHelp[1]); assert (pHelp[2]);       // pHelp should be aligned
         str.replace (str.length (), 0, pHelp, 3);        // to 3-byte-boundary
         pHelp += 3;
     } // end-while

      free (pString);
   }
   return str;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads string-representation from stream
//Parameters: in: Stream to parse
/*--------------------------------------------------------------------------*/
void ANumeric::readFromStream (istream& in) {
   std::string help;
   char ch;

   in >> ch; assert (!isspace (ch));               // Skip leading whitespaces
   while (!in.eof ()) {
      if (!strchr (loc->thousands_sep, ch)) {   // Ignore thousand-seperators,
         if (isdigit (ch))                  // add only digits; else terminate
            help += ch;
         else
            break;
      } // endif

      in.get (ch);
   } // end-while !eof
   in.putback (ch);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: lhs: Value to add
//Returns   : Self
//Note      : If lhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator+= (const ANumeric& lhs) {
   if (lhs.isDefined ()) {
      mpz_add (value, value, lhs.value);
      AttributValue::define ();
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substract a value from this
//Parameters: lhs: Value to substract
//Returns   : Self
//Note      : If lhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator-= (const ANumeric& lhs) {
   if (lhs.isDefined ()) {
      mpz_sub (value, value, lhs.value);
      AttributValue::define ();
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Multiply this with a value
//Parameters: lhs: Value to multiply
//Returns   : Self
//Note      : If lhs is not defined this is not changed; if this is not defined
//            it is set to lhs
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator*= (const ANumeric& lhs) {
   if (lhs.isDefined ())
      if (isDefined ())
         mpz_mul (value, value, lhs.value);
      else
         operator= (lhs);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Divides this through a value
//Parameters: lhs: Value to divide with
//Returns   : Self
//Note      : If lhs is not defined this is not changed; if this is not defined
//            it is set to lhs
/*--------------------------------------------------------------------------*/
ANumeric& ANumeric::operator/= (const ANumeric& lhs) {
   if (lhs.isDefined ())
      if (isDefined ())
         mpz_tdiv_q (value, value, lhs.value);
      else
         operator= (lhs);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares two ANumeric-values
//Parameters: other: Object to compare
//Returns   : >0 if this > other; 0 if this == other; <0 else
//Note      : Undefined values are considered as between 0 and -1
/*--------------------------------------------------------------------------*/
int ANumeric::compare (const ANumeric& other) {
   if (isDefined () && other.isDefined ())     // Both sides defined -> compare
      return mpz_cmp (value, other.value);

   int rc;
   if (isDefined ())                            // this defined: Compare with 0
      rc = mpz_cmp_ui (value, 0);              // other defined: Compare with 0
   else if (other.isDefined ())                      // Both not defined: Equal
      return 0;
   else
      rc = -mpz_cmp_ui (other.value, 0);
   return rc ? rc : 1;                // If comp with 0 is equal: Return bigger
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
      out << outValue.toString ();
   return out;
}
