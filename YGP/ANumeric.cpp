//$Id: ANumeric.cpp,v 1.55 2008/06/10 21:47:20 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : ANumeric
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.55 $
//AUTHOR      : Markus Schwab
//CREATED     : 22.7.1999
//COPYRIGHT   : Copyright (C) 1999 - 2006, 2008

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


#include <cstring>
#include <cerrno>
#include <clocale>
#include <climits>

#include <ygp-cfg.h>

#ifndef HAVE_LIBGMP
#  include <cctype>
#  include <cstdlib>

#  include <sstream>
#endif

#include <string>
#include <stdexcept>

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Internal.h"

#include "YGP/ANumeric.h"


namespace YGP {


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ANumeric::~ANumeric () {
#ifdef HAVE_LIBGMP
   mpz_clear (value);
#endif
}


//-----------------------------------------------------------------------------
/// Assignmentoperator; tries to extract an integer-value from the passed
/// value. pValue must not be a NULL-pointer, may have leading white-spaces
/// which may follow a sign (plus (+) or minus (-)) followed by numeric
/// characters and a zero-characer (\\0).
///
/// Any locale-related separaters are stripped out.
///
/// If pValue has a different format an excpetion is thrown.
/// \param  pValue Pointer to ASCIIZ-string containing numeric value to assign
/// \throw invalid_argument in case of an exception
/// \pre \c pValue a valid ASCIIZ-string
//-----------------------------------------------------------------------------
ANumeric& ANumeric::operator= (const char* pValue) throw (std::invalid_argument) {
   if (!(pValue && *pValue))
      undefine ();
   else {
      TRACE9 ("ANumeric::operator= (const char*) - Setting " << pValue);
      std::string unformatted (pValue);

      struct lconv* loc = localeconv ();             // Get locale-information
      TRACE9 ("ANumeric::operator= (const char*) - Locale-info = "
	      << std::hex << (int)*loc->grouping << std::dec << " - " << loc->thousands_sep);
      Check3 (strlen (loc->grouping) >= strlen (loc->thousands_sep));

      int len (unformatted.length () - 1);
      int index (0);
      while (loc->grouping[index])
	 ++index;
      char group (loc->grouping[--index]);
      const char* pSep = loc->thousands_sep;

      if (!group)
	 group = CHAR_MAX;

      while ((group != CHAR_MAX) && (len > group)) {     // Check if grouping nec.
	 TRACE9 ("ANumeric::operator= (const char*) - Len =  " << len
		 << "; Group = " << (int)group << "; Index = " << index);
	 len -= group;
	 if (unformatted[len] == *pSep)
	    unformatted.replace (len--, 1, 0, '\0');
	 else
	    break;
	 TRACE8 ("ANumeric::operator= (const char*) - Removed " << unformatted);

	 if (index) {                    // Decrement group-pointer if more groups
	    group = loc->grouping[--index];
	    if (pSep[1])
	       ++pSep;
	 } // endif further grouping available
      } // end-while grouping necessary

#ifdef HAVE_LIBGMP
      if (mpz_set_str (value, unformatted.c_str (), 0))
#else
      char* pTail = NULL;
      errno = 0;
      value = strtol (unformatted.c_str (), &pTail, 0);
      if (errno || ((pValue = pTail) && *pTail && !isspace (*pTail)))
#endif
	 {
	    std::string e =  (_("Not a number: %1"));
	    e.replace (e.find ("%1"), 2, pValue);
	    throw std::invalid_argument (e.c_str ());
	 }
      setDefined ();
   }
   return *this;
}

//-----------------------------------------------------------------------------
/// Assignment-operator from another ANumeric object.
/// \param other Object to copy
/// \returns ANumeric& Reference to self
//-----------------------------------------------------------------------------
ANumeric& ANumeric::operator= (const ANumeric& other) {
   TRACE9 ("ANumeric& ANumeric::operator= (const ANumeric&)");
#ifdef HAVE_LIBGMP
   mpz_set (value, other.value);
#else
   value = other.value;
#endif
   AttributValue::operator= ((const AttributValue&) other);
   return *this;
}

//-----------------------------------------------------------------------------
/// Defines the object and sets the value to 0.
//-----------------------------------------------------------------------------
void ANumeric::define () {
   setDefined ();
#ifdef HAVE_LIBGMP
   mpz_set_si (value, 0);
#else
   value = 0;
#endif
}

//-----------------------------------------------------------------------------
/// Converts the number into a string without any formatting.
/// \returns std::string String-representation of ANumeric
//-----------------------------------------------------------------------------
std::string ANumeric::toUnformattedString () const {
   std::string str ("");

   if (isDefined ()) {
#ifdef HAVE_LIBGMP
      char* pString (mpz_get_str (NULL, 10, value)); Check3 (pString);
#else
      std::ostringstream ostr;
      ostr << value << '\0';
      const char* pString = ostr.str ().c_str ();
#endif
      TRACE1 ("ANumeric::toUnformattedString -> value = " << pString);

      str = pString;                 // Copy unformatted string to return-value
#ifdef HAVE_LIBGMP
      free (pString);
#endif
   }

   return str;
}

//-----------------------------------------------------------------------------
/// Converts the number into a string, in the format specified by the current
/// locale.
/// \returns std::string String-representation of ANumeric
/// \remarks Initializes the locale-definition (if not already done)
//-----------------------------------------------------------------------------
std::string ANumeric::toString () const {
   TRACE9 ("ANumeric::toString () const");
   std::string str;

   struct lconv* loc = localeconv ();                // Get locale-information
   TRACE9 ("ANumeric::toString () const - Locale-info = " << std::hex << (long)(*loc->grouping)
           << std::dec << " - " << loc->thousands_sep);

   str = toUnformattedString ();
   TRACE5 ("ANumeric::toString () const - " << str);

   int len (str.length ());
   int index (0);
   char group (loc->grouping[index]);
   const char* pSep = loc->thousands_sep;
   if (!group)
      group = CHAR_MAX;

   while ((group != CHAR_MAX) && (len > group)) {     // Check if grouping nec.
      TRACE9 ("ANumeric::toString () const - Len =  " << len << "; Group = "
              << (int)group << "; Index = " << index);
      len -= group;
      if (str[len - 1] != '-')
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

//-----------------------------------------------------------------------------
/// Reads a numeric value from a stream. If the input is not valid, an
/// excpetion is thrown.
/// \param in Stream to parse
/// \remarks
///     - Leading whitespaces in the stream are skipped.
///     - Parsing is stopped at EOF or at any non-digit.
//-----------------------------------------------------------------------------
void ANumeric::readFromStream (std::istream& in) throw (std::invalid_argument) {
   undefine ();

   struct lconv* loc = localeconv ();                // Get locale-information

   std::string help;
   char ch, chSep;
   const char* pSep = loc->thousands_sep;

   in >> ch;                                       // Skip leading whitespaces
   while (!in.eof () && !isspace (ch)) {
      if (strchr (pSep, ch)) {      // Skip thousand-seperators,
         chSep = ch;
         in.get (ch);
      } // endif
      else
         chSep = '0';

      if (isdigit (ch))                     // add only digits; else terminate
	 help += ch;
      else {
	 if (chSep != '0')
	    in.putback (chSep);
	 break;
      } // end-else non-digit found

      in.get (ch);
   } // end-while !eof
   in.putback (ch);

   operator= (help);
}


//-----------------------------------------------------------------------------
/// Adds another number-object to this object. An undefined number is treated
/// as "0"; so only if both objects are undefined, the result is (remains)
/// undefined.
/// \param rhs Value to add
/// \returns ANumeric& Self
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Substracts another number-object from this object. An undefined number is
/// treated as "0"; so only if both objects are undefined, the result is
/// (remains) undefined.
/// \param rhs Value to substract
/// \returns ANumeric& Self
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Multiplies another number-object to this object. An undefined number is
/// treated as "1"; so only if both objects are undefined, the result is
/// (remains) undefined.
/// \param rhs Value to multiply
/// \returns ANumeric& Self
//-----------------------------------------------------------------------------
ANumeric& ANumeric::operator*= (const ANumeric& rhs) {
   if (rhs.isDefined ()) {
      if (isDefined ())
#ifdef HAVE_LIBGMP
         mpz_mul (value, value, rhs.value);
#else
         value *= rhs.value;
#endif
      else
         operator= (rhs);
   }
   return *this;
}

//-----------------------------------------------------------------------------
/// Divides this by another number-object. An undefined number is treated as
/// "1"; so only if both objects are undefined, the result is (remains)
/// undefined.
/// \param rhs Value to divide with
/// \returns ANumeric& Self
/// \remarks If rhs is not defined this is not changed; if this is not defined,
///     it is set to 1/rhs
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Returns -1 if this is less then other, 0 if they are equal or 1 if this is
/// larger. Undefined values are considered as larger than any number for this
/// comparison (so that defined values are sorted before undefined ones); if
/// both values are undefined they are considered as equal.
/// \param other Object to compare
/// \returns 1 if this > other; 0 if this == other; -1 else
//-----------------------------------------------------------------------------
int ANumeric::compare (const ANumeric& other) const {
   if (isDefined () && other.isDefined ())     // Both sides defined -> compare
#ifdef HAVE_LIBGMP
      return mpz_cmp (value, other.value);
#else
      return (value > other.value) ? 1 : (value < other.value) ? -1 : 0;
#endif

   return isDefined () ? -1 : other.isDefined () ? 1 : 0;
}


//-----------------------------------------------------------------------------
/// Adds lhs and rhs and returns the result. If one object is defined, the
/// other object is treated as "0"; so only if both objects are undefined, the
/// result is undefined.
/// \param lhs Left-hand-side of addition
/// \param rhs Right-hand-side of addition
/// \returns ANumeric Result of additon Note : Undefined values are
///     ignored
//-----------------------------------------------------------------------------
ANumeric operator+ (const ANumeric& lhs, const ANumeric& rhs) {
   ANumeric result (lhs);
   result += rhs;
   return result;
}

//-----------------------------------------------------------------------------
/// Substracts rhs from lhs and returns the result. If one object is defined,
/// the other is is treated as "0"; so only if both objects are undefined, the
/// result is undefined.
/// \param lhs Left-hand-side of substraction
/// \param rhs Right-hand-side of substraction
/// \returns ANumeric Result of substraction Note : Undefined values are
///     ignored
//-----------------------------------------------------------------------------
ANumeric operator- (const ANumeric& lhs, const ANumeric& rhs) {
   ANumeric result (lhs);
   result -= rhs;
   return result;
}

//-----------------------------------------------------------------------------
/// Multiplies rhs and lhs and returns the result. If one object is defined,
/// the other is treated as "1"; so only if both objects are undefined, the
/// result is undefined.
/// \param lhs Left-hand-side of mulitplication
/// \param rhs Right-hand-side of mulitplication
/// \returns ANumeric Result of mulitplication
//-----------------------------------------------------------------------------
ANumeric operator* (const ANumeric& lhs, const ANumeric& rhs) {
   ANumeric result (lhs);
   result *= rhs;
   return result;
}

//-----------------------------------------------------------------------------
/// Divides this by another number-object. If only one object is defined, the
/// other is treated as "1"; so only if both objects are undefined, the result
/// is undefined.
/// \param lhs Left-hand-side of division
/// \param rhs Right-hand-side of division
/// \returns ANumeric Result of division
//-----------------------------------------------------------------------------
ANumeric operator/ (const ANumeric& lhs, const ANumeric& rhs) {
   ANumeric result (lhs);
   result /= rhs;
   return result;
}

}
