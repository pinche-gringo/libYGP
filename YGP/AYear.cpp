//$Id: AYear.cpp,v 1.2 2004/11/14 21:19:25 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : AYEAR
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
//AUTHOR      : Markus Schwab
//CREATED     : 7.11.2004
//COPYRIGHT   : Copyright (C) 2004

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


#include <cstdio>
#include <cerrno>
#include <cctype>
#include <ygp-cfg.h>

#if SYSTEM == WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include <stdexcept>

#include "YGP/Trace.h"
#include "YGP/Internal.h"

#include "YGP/AYear.h"


namespace YGP {

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
AYear::~AYear () {
}


//----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to assign
/// \return \c AYear&: Reference to self
//----------------------------------------------------------------------------
AYear& AYear::operator= (const AYear& other) {
   if (this != &other) {
      TRACE5 ("AYear::operator=: " << other);
      year = other.year;
      AttributValue::operator= ((const AttributValue&) other);
   }
   return *this;
}

//----------------------------------------------------------------------------
/// Assignment-operator from a const char-pointer. The year must be passed as
/// <tt>Y[Y...]</tt>. If the buffer does not represent a valid year, an
/// exception is thrown.
/// \param pValue: Character array holding year to assign
/// \return \c AYear&: Reference to self
/// \throw std::invalid_argument if the characters don't represent a valid year
//----------------------------------------------------------------------------
AYear& AYear::operator= (const char* pValue) throw (std::invalid_argument) {
   TRACE5 ("AYear::operator= (const char*): " << pValue);
   if (pValue && *pValue) {
      char* pTail = NULL;
      errno = 0;
      year = strtol (pValue, &pTail, 0);
      if (errno || (pTail && *pTail)) {
	 std::string e =  (_("Invalid value: %1"));
	 e.replace (e.find ("%1"), 2, pTail);
	 throw std::invalid_argument (e.c_str ());
      }
      setDefined ();
   }
   else
      undefine ();
   return *this;
}

//----------------------------------------------------------------------------
/// Converts the year into a string, in the format <tt>DDMMYYYY[...]</tt>; each
/// entry is filled up with zeros.
/// \return \c std::string: String-representation of AYear
//----------------------------------------------------------------------------
std::string AYear::toUnformattedString () const {
   char buffer[12] = "";

   if (isDefined ())
      sprintf (buffer, "%4d", year);
   return std::string (buffer);
}

//----------------------------------------------------------------------------
/// Reads a year as DDMMY[Y...] from a stream. If the input is not valid, an
/// exception is thrown.
/// \param in: Stream to parse
/// \throw std::invalid_argument in case of an invalid input
//----------------------------------------------------------------------------
void AYear::readFromStream (std::istream& in) throw (std::invalid_argument) {
   char ch ('\0');
   in >> ch;
   in.unget ();
   if (isdigit (ch)) {
      in >> year;
      setDefined ();
   }
   else
      undefine ();
}

//----------------------------------------------------------------------------
/// Adds another year-value to the year-object. An undefined year is treated
/// as <tt>0.0.0</tt>; so only if both objects are undefined, the result is
/// (remains) undefined. Overflows (of day or month) are corrected.
/// \param rhs: Value to add
/// \return \c Self
//----------------------------------------------------------------------------
AYear& AYear::operator+= (const AYear& rhs) {
   if (rhs.isDefined ()) {
      if (isDefined ())
         year += rhs.year;
      else
         operator= (rhs.year);
   }
   return *this;
}

//----------------------------------------------------------------------------
/// Substracts another year-value from the year-object. An undefined year
/// is treated as <tt>0.0.0</tt>; the result is (remains) undefined if both
///  objects are undefined. Underflows (of day or month) are corrected.
/// \param rhs: Value to substract
/// \return \c Self
//----------------------------------------------------------------------------
AYear& AYear::operator-= (const AYear& rhs) {
   if (rhs.isDefined ()) {
      if (isDefined ())
         year -= rhs.year;
      else
         operator= (-rhs.year);
   }
   return *this;
}

//----------------------------------------------------------------------------
/// Returns the (approximated) difference in days between two years.
/// "Younger years" (closer to the past) are considered bigger than "older
/// years" (further in the past; that means the numeric value of the year is
/// compared (e.g.: "yesterday" < "today" < "tomorrow").
///
///  If both years are undefined, those difference is "0", if only this is
///  undefined the result is -1; if other is undefined 1 is returned.
///
/// \param other: Object to compare
/// \return \c >0 if this "younger" other; 0 if this == other; <0 else
//----------------------------------------------------------------------------
long AYear::compare (const AYear& other) {
   // Both sides are defined -> return (approximated) difference
   if (isDefined ()) {
      if (other.isDefined ())
         return year - other.year;
      else
         return 1;                    // this defined, other not: Return bigger
   }
   else
      return other.isDefined () ? -1 : 0;
}

//----------------------------------------------------------------------------
/// Adds two year-values and returns the result. An undefined year is
/// treated as "0.0.0"; the result is (remains) undefined if both objects are
/// undefined. Overflows (of day or month) are corrected.
/// \param lhs: Left-hand-side of addition
/// \param rhs: Right-hand-side of addition
/// \return \c AYear: Result of additon
//----------------------------------------------------------------------------
AYear operator+ (const AYear& lhs, const AYear& rhs) {
   AYear result (lhs);
   result += rhs;
   return result;
}

//----------------------------------------------------------------------------
/// Substracts two year-values and returns the result. An undefined year is
/// treated as "0.0.0"; the result is (remains) undefined if both objects
/// are undefined. Overflows (of day or month) are corrected.
/// \param lhs: Left-hand-side of substraction
/// \param rhs: Right-hand-side of substraction
/// \return \c AYear: Result of substraction
//----------------------------------------------------------------------------
AYear operator- (const AYear& lhs, const AYear& rhs) {
   AYear result (lhs);
   result -= rhs;
   return result;
}

//----------------------------------------------------------------------------
/// Checks if the passed year is a leap-year (years which can be divided by 4;
/// except if it is also divideable by 100).
/// \param year: Year to check
/// \return \c bool: True, if leap-year
//----------------------------------------------------------------------------
bool AYear::isLeapYear (int year) {
  TRACE9 ("AYear::isLeapYear: " << year << " %4 = " << (year & 3) << " %100 = "
           << (year % 100) << " %400 = " << (year % 400));
   return (year & 3) ? false : (year % 100) ? true : !(year % 400);
}

}
