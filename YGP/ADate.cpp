//$Id: ADate.cpp,v 1.31 2003/06/15 05:20:26 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ADate
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.31 $
//AUTHOR      : Markus Schwab
//CREATED     : 11.10.1999
//COPYRIGHT   : Anticopyright (A) 1999 - 2003

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

#include <gzo-cfg.h>

#if SYSTEM == WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include <sstream>
#include <stdexcept>

#include "Trace_.h"
#include "Internal.h"

#include "ADate.h"

//----------------------------------------------------------------------------
/// Constructor; depending on the parameter the date is either set to the first
/// of January, 1900 (now = false), or to the current day (now = true)
/// \param now: Flag which date to set (1900/1/1 (false) or now (true))
//----------------------------------------------------------------------------
ADate::ADate (bool now) : AttributValue () {
   if (now)
      operator= (time (NULL));
   else
      define ();
}

//----------------------------------------------------------------------------
/// Constructor; sets the passed date. The date is undefined, if the passed
/// parameters represent no valid day.
/// \param Day: Day for this ADate
/// \param Month: Month
/// \param Year: Year to set
/// \throw \c std::invalid_argument in case of an invalid input
//----------------------------------------------------------------------------
ADate::ADate (char Day, char Month, int Year) throw (std::invalid_argument)
   : AttributValue (true), day (Day), month (Month), year (Year) {
   int status (checkIntegrity ());
   if (status)
      throw (std::invalid_argument (status == 2 ? "Month" : "Day"));
}

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
ADate::~ADate () {
}


//----------------------------------------------------------------------------
/// Assignment-operator
/// \param other: Object to assign
/// \return \c ADate&: Reference to self
//----------------------------------------------------------------------------
ADate& ADate::operator= (const ADate& other) {
   Check1 (!checkIntegrity ()); Check1 (!other.checkIntegrity ());

   if (this != &other) {
      TRACE5 ("ADate::operator=: " << other);

      day = other.day;
      month = other.month;
      year = other.year;
      AttributValue::operator= ((const AttributValue&) other);
   }
   return *this;
}

//----------------------------------------------------------------------------
/// Assignment-operator from a const char-pointer. The date must be passed as
/// <tt>DDMMY[Y...]</tt>. If the buffer does not represent a valid day, an
/// exception is thrown.
/// \param pValue: Character array holding date to assign
/// \return \c ADate&: Reference to self
/// \throw \c std::invalid_argument if the characters don't represent a valid date
//----------------------------------------------------------------------------
ADate& ADate::operator= (const char* pValue) throw (std::invalid_argument) {
   Check1 (pValue);
   Check1 (!checkIntegrity ());

   TRACE5 ("ADate::operator= (const char*): " << pValue);

   std::istringstream help (pValue);
   readFromStream (help);
   return *this;
}

//----------------------------------------------------------------------------
/// Assignment operator from a broken down time structure.
/// \param date: Structure holding the date to assign
/// \return \c ADate&: Reference to self
//----------------------------------------------------------------------------
ADate& ADate::operator= (const struct tm& date) {
   setDefined ();
   year = date.tm_year + 1900;
   month = (unsigned char)(date.tm_mon + 1);
   setDay ((unsigned char)date.tm_mday); Check3 (!checkIntegrity ());
   return *this;
}

//----------------------------------------------------------------------------
/// Converts the date into a string, in the format <tt>DDMMYYYY[...]</tt>; each
/// entry is filled up with zeros.
/// \return \c std::string: String-representation of ADate
//----------------------------------------------------------------------------
std::string ADate::toUnformattedString () const {
   char buffer[12];

   sprintf (buffer, "%02d%02d%04d", (unsigned)day, (unsigned)month, year);
   return std::string (buffer);
}

//----------------------------------------------------------------------------
/// Converts the date into a string, in the format specified by the current
/// locale           
/// \return \c std::string: String-representation of ADate
/// \remarks Only dates valid for <tt>struct tm</tt> can be printed (e.g. dates
///     after 1900)
//----------------------------------------------------------------------------
std::string ADate::toString () const {
   return toString ("%x");
}

//----------------------------------------------------------------------------
//// Converts the date into a string, in the specified format. The parameter
///  format can be any value accepted by the \c strftime
//            library-routine.
/// \return \c std::string: String-representation of ADate
/// \remarks Only dates valid for <tt>struct tm</tt> can be printed (e.g. dates
///     after 1900)
//----------------------------------------------------------------------------
std::string ADate::toString (const char* format) const {
   Check1 (format);
   std::string result;

   if (isDefined ()) {
      struct tm tm (toStructTM ());
#if defined (_MSC_VER) || defined (__BORLANDC__)
      char aBuffer[80];      // VC doesn't return the buffersize it would need
#else
      char aBuffer[strftime (NULL, 200, format, &tm) + 1];
#endif
      strftime (aBuffer, sizeof (aBuffer), format, &tm);
      result = aBuffer;
   }
   return result;
}

//----------------------------------------------------------------------------
/// Reads a date as DDMMY[Y...] from a stream. If the input is not valid, an
/// exception is thrown.
/// \param in: Stream to parse
/// \throw \c std::invalid_argument in case of an invalid input
//----------------------------------------------------------------------------
void ADate::readFromStream (std::istream& in) throw (std::invalid_argument) {
   if (in.eof ()) {
      undefine ();
      return;
   }
   static unsigned char ADate::*const targets[] = { &ADate::day, &ADate::month };
   day = month = 0;

   int ch;
   int i (0);
   for (; i < 4; ++i) {
      ch = in.get ();
      TRACE8 ("ADate::readFromStream (istream&): Get: " << (char)ch)

      if ((ch == EOF) || ((ch > '9') || (ch < '0')))
         break;

      this->*(targets[i >> 1]) += (ch & 0xf);
      if (!(i & 1))
	 this->*(targets[i >> 1]) *= 10;
   } // endfor

   in >> year;
   TRACE9 ("ADate::readFromStream (istream&): Read: " << (int)day << '.' << (int)month
           << '.' << year);

   if ((i < 4) || checkIntegrity ()) {
      undefine ();
      if (i) {
         std::string error (_("Position %1"));
         error.replace (error.find ("%1"), 2, 1, char (i + '0'));
         throw std::invalid_argument (error);
      }
   }
   else {
      TRACE9 ("ADate::readFromStream (istream&): Define");
      setDefined ();
   }
}

//----------------------------------------------------------------------------
/// Adds another date-value to the date-object. An undefined date is treated
/// as <tt>0.0.0</tt>; so only if both objects are undefined, the result is
/// (remains) undefined. Overflows (of day or month) are corrected.
/// \param rhs: Value to add
/// \return \c Self
//----------------------------------------------------------------------------
ADate& ADate::operator+= (const ADate& rhs) {
   Check1 (!checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (isDefined ()) {
         day += rhs.day;
         month += rhs.month;
         year += rhs.year;

         maxAdapt ();
      }
      else
         operator= (rhs);

      Check3 (!checkIntegrity ());
   }
   return *this;
}

//----------------------------------------------------------------------------
/// Substracts another date-value from the date-object. An undefined date
/// is treated as <tt>0.0.0</tt>; the result is (remains) undefined if both
///  objects are undefined. Underflows (of day or month) are corrected.
/// \param rhs: Value to substract
/// \return \c Self
//----------------------------------------------------------------------------
ADate& ADate::operator-= (const ADate& rhs) {
   Check1 (!checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (isDefined ()) {
         day -= rhs.day;
         month -= rhs.month;
         year -= rhs.year;
      }
      else {
         setDefined ();
         day = -rhs.day;
         month = -rhs.month;
         year = -rhs.year;
      } // endif this not defined

      minAdapt ();
      Check3 (!checkIntegrity ());
   }
   return *this;
}

//----------------------------------------------------------------------------
/// If this is not undefined, the passed values are added (with attention to
/// overflows). The result is returned.
/// \param Day: Day to add
/// \param Month: Month to add
/// \param Year: Year to add
/// \return \c Self
/// \remarks In counterpart to the mathematic operators (operator+() and
///    operator-()) this method does not change the object if it is undefined!
//----------------------------------------------------------------------------
ADate& ADate::add (signed char Day, signed char Month, int Year) {
   TRACE7 ("ADate::add: " << toString () << " + " << (int)Day << '.'
	   << (int)Month << '.' << Year);
   Check1 (!checkIntegrity ());

   if (isDefined ()) {
      Year += Month / 12;
      Month %= 12;
      month += Month;
      year += Year;
      maxAdapt ();

      signed char maxDay;
      while (maxDay = maxDayOf (), Day > maxDay) {
         Day -= maxDay;
         ++month;
	 maxAdapt ();
      } // end-while passed days bigger than one month
      day += Day;
      maxAdapt ();

      Check3 (!checkIntegrity ());
   }
   return *this;
}

//----------------------------------------------------------------------------
/// If this is defined, the passed values are substracted (with attention to
/// underflows). The result is returned.
/// \param Day: Day to substract
/// \param Month: Month to substract
/// \param Year: Year to substract
/// \return \c Self
/// \remarks In counterpart to the mathematic operators (operator+() and
///    operator-()) this method does not change the object if it is undefined!
//----------------------------------------------------------------------------
ADate& ADate::sub (signed char Day, signed char Month, int Year) {
   TRACE7 ("ADate::sub: " << toString () << " - " << (int)Day << '.'
	   << (int)Month << '.' << Year);
   Check1 (!checkIntegrity ());

   if (isDefined ()) {
      Year += Month / 12;                      // Correct month and year first
      Month %= 12;
      month -= Month;
      year -= Year;
      minAdapt ();

      signed char maxDay;
      while (maxDay = maxDayOf (month > 1 ? month - 1 : 12, year),
	     Day > maxDay) {
         Day -= maxDay;
         --month;
	 minAdapt ();
      } // end-while passed days bigger than one month
      if (Day) {
	 day -= Day;
	 minAdapt ();
      }

      Check3 (!checkIntegrity ());
      Check3 (isDefined ());
   }
   return *this;
}

//----------------------------------------------------------------------------
/// Returns the (approximated) difference in days between two dates. 
/// "Younger dates" (closer to the past) are considered bigger than "older
/// dates" (further in the past; that means the numeric value of the date is
/// compared (e.g.: "yesterday" < "today" < "tomorrow").
///
///  If both dates are undefined, those difference is "0", if only this is
///  undefined the result is MINLONG; if other is undefined MAXLONG is
///  returned (-> undefined dates are considered as (very) old).
///
/// \param other: Object to compare
/// \return \c >0 if this "younger" other; 0 if this == other; <0 else
//----------------------------------------------------------------------------
long ADate::compare (const ADate& other) {
   Check1 (!checkIntegrity ()); Check1 (!other.checkIntegrity ());

   // Both sides are defined -> return (approximated) difference
   if (isDefined ()) {
      if (other.isDefined ()) {
         TRACE5 ("ADate::compare -> " << ((year - other.year) * 365
                                          + ((month - other.month) * 31)
                                          + day - other.day));

         return ((year - other.year) * 365 + ((month - other.month) * 31)
                 + day - other.day);
      }
      else
         return 1;                    // this defined, other not: Return bigger
   }
   else
      return other.isDefined () ? -1 : 0;
}

//----------------------------------------------------------------------------
/// Adds two date-values and returns the result. An undefined date is
/// treated as "0.0.0"; the result is (remains) undefined if both objects are
/// undefined. Overflows (of day or month) are corrected.
/// \param lhs: Left-hand-side of addition
/// \param rhs: Right-hand-side of addition
/// \return \c ADate: Result of additon
//----------------------------------------------------------------------------
ADate operator+ (const ADate& lhs, const ADate& rhs) {
   Check1 (!lhs.checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   ADate result (lhs);
   result += rhs;
   return result;
}

//----------------------------------------------------------------------------
/// Substracts two date-values and returns the result. An undefined date is
/// treated as "0.0.0"; the result is (remains) undefined if both objects
/// are undefined. Overflows (of day or month) are corrected.
/// \param lhs: Left-hand-side of substraction
/// \param rhs: Right-hand-side of substraction
/// \return \c ADate: Result of substraction
//----------------------------------------------------------------------------
ADate operator- (const ADate& lhs, const ADate& rhs) {
   Check1 (!lhs.checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   ADate result (lhs);
   result -= rhs;
   return result;
}

//----------------------------------------------------------------------------
/// Checks if this object represents a valid date.
/// \return \c Status; 0: OK
/// \remarks Even undefined dates must have valid values!
//----------------------------------------------------------------------------
int ADate::checkIntegrity () const {
   TRACE9 ("ADate::checkIntegrity () const - " << (int)day << '.' << (int)month
           << '.' << year);
   return isDefined () ? ((month < 1) || (month > 12)) ? 2 : (day > maxDayOf ()) : 0;
}

//----------------------------------------------------------------------------
/// Returns the maximal day for the the passed month/year-combination
/// \param month: Month to check
/// \param year: Year to check
/// \return \c int: Maximal day for the passed parameters [28 - 31]
//----------------------------------------------------------------------------
char ADate::maxDayOf (char month, int year) {
   Check1 ((month > 0) && (month < 13));

   if (month == (unsigned char)2)              // Special-handling of february
      return (unsigned char)(isLeapYear (year) ? 29 : 28);

   if (month > (unsigned char)7)    // Adapt month after july for easier calc.
      --month;
   return (unsigned char)(month & 1 ? 31 : 30);
}

//----------------------------------------------------------------------------
/// Checks if the passed year is a leap-year (years which can be divided by 4;
/// except if it is also divideable by 100).
/// \param year: Year to check
/// \return \c bool: True, if leap-year
//----------------------------------------------------------------------------
bool ADate::isLeapYear (int year) {
  TRACE9 ("ADate::isLeapYear: " << year << " %4 = " << (year & 3) << " %100 = "
           << (year % 100) << " %400 = " << (year % 400));
   return (year & 3) ? false : (year % 100) ? true : !(year % 400);
}

//----------------------------------------------------------------------------
/// Corrects the object after an (possible) underflows. If the object is
/// integer after the operation, true is returned.
/// \return \c bool: Flag, if object is integer
//----------------------------------------------------------------------------
bool ADate::minAdapt () {
   TRACE7 ("ADate::minAdapt: " << toString ());
   if (((unsigned char)(month - 1)) > 11) {
      int mon (255 - (unsigned char)(month - 1));
      year -= (mon / 12) + 1;
      month = 12 - (mon % 12);
   }

   TRACE9 ("ADate::minAdapt (month adapted (1)): " << toString ());

   if ((signed char)day > maxDayOf ())
      day = maxDayOf ();
   else
      while (((signed char)day) < 1) {               // Adapt date if underflow
         --month;
         if (month < 1) {
            month = 12;
            --year;
            TRACE9 ("ADate::minAdapt (month adapted (2)): " << toString ());
         }
         day += maxDayOf ();
         TRACE9 ("ADate::minAdapt (day adapted (1)): " << toString ());
      } // endif day invalid

   return !ADate::checkIntegrity ();      // Can only ensure proper ADate-part
}

//----------------------------------------------------------------------------
/// Corrects the object after an (possible) overflows. If the object is
/// integer after the operation, true is returned.
/// \return \c bool: Flag, if object is integer
//----------------------------------------------------------------------------
bool ADate::maxAdapt () {
   TRACE7 ("ADate::maxAdapt: " << toString ());
   if (((unsigned char)(month - 1)) > 11) {
      year += month / 12;
      month = month % 12;
   }

   TRACE9 ("ADate::maxAdapt (month dapted (1)): " << toString ());

   unsigned char maxDay (maxDayOf ());               // Adapt date if overflow
   while (day > maxDay) {
      day -= maxDay;
      ++month;
   }

   if (month > (unsigned char)12) {                 // Adapt month if overflow
      month -= (unsigned char)12;     // Assuming calc. was with correct month
      ++year;
   }
   return !ADate::checkIntegrity ();      // Can only ensure proper ADate-part
}

//----------------------------------------------------------------------------
/// Sets the day to the passed value. If the date is not valid, it is set to
/// undefined.
/// \param Day: Day to set
//----------------------------------------------------------------------------
void ADate::setDay (char Day) throw (std::invalid_argument) {
   day = Day;

   if (checkIntegrity ()) {
      day = 1;
      throw (std::invalid_argument ("ADate::setDay"));
   }
   else
      setDefined ();
}

//----------------------------------------------------------------------------
/// Sets the month to the passed value. If the date is not valid, it is set to
/// undefined.
/// \param Month: Month to set
//----------------------------------------------------------------------------
void ADate::setMonth (char Month) throw (std::invalid_argument) {
   month = Month;

   if (checkIntegrity ()) {
      throw (std::invalid_argument ("ADate::setMonth"));
      undefine ();
   }
   else
      setDefined ();
}

//----------------------------------------------------------------------------
/// Converting to a struct tm.
/// \return \c struct tm: Date in struct tm-format; the time-part is set to zeros
/// \remarks It is not checked if the date is in the right range for a
//        <tt>struct tm</tt> (after 1900 and before 2039)
//----------------------------------------------------------------------------
struct tm ADate::toStructTM () const {
   struct tm result = { 0, 0, 0, 0, 0, 0 };
   if (isDefined ()) {
      result.tm_mday = day;
      result.tm_mon = month - 1;
      result.tm_year = year - 1900;
   }
   return result;
}
