//$Id: ADate.cpp,v 1.29 2003/03/03 05:56:28 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ADate
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.29 $
//AUTHOR      : Markus Schwab
//CREATED     : 11.10.1999
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

#include <stdio.h>

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

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; depending on the parameter the date is either set
//            to the first of January, 1900 (now = false), or to the current
//             day (now = true)
//Parameters: now: Flag which time to set (1900/1/1 (false) or now (true))
/*--------------------------------------------------------------------------*/
ADate::ADate (bool now) : AttributValue () {
   if (now)
      operator= (time (NULL));
   else
      define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; sets the passed date. The date is undefined, if the
//            passed parameters represent no valid day.
//Parameters: day: Day for this ADate
//            month: Month
//            year: Year to set
/*--------------------------------------------------------------------------*/
ADate::ADate (char Day, char Month, int Year) throw (std::invalid_argument)
   : AttributValue (true), day (Day), month (Month), year (Year) {
   int status (checkIntegrity ());
   if (status)
      throw (std::invalid_argument (status == 2 ? "Month" : "Day"));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
ADate::~ADate () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: other: Object to assign
//Returns   : ADate&: Reference to self
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator from a const char-pointer. The date must be
//            passed as DDMMY[Y...]. If the buffer does not represent a
//            valid day, an exception is thrown.
//Parameters: pDate: Character array holding date to assign
//Returns   : ADate&: Reference to self
//Throws    : std::invalid_argument if the characters don't represent a valid date
/*--------------------------------------------------------------------------*/
ADate& ADate::operator= (const char* pDate) throw (std::invalid_argument) {
   Check1 (pDate);
   Check1 (!checkIntegrity ());

   TRACE5 ("ADate::operator= (const char*): " << pDate);

   // #if defined (__BORLANDC__) || defined (_MSC_VER)
   //   std::istrstream help (const_cast <char*> (pDate));
   // #else
   std::istringstream help (pDate);
   // #endif
   readFromStream (help);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the date into a string, in the format DDMMYYYY; each
//            entry is filled up with zeros.
//Returns   : std::string: String-representation of ADate
/*--------------------------------------------------------------------------*/
std::string ADate::toUnformatedString () const {
   char buffer[12];

   sprintf (buffer, "%02d%02d%04d", (unsigned)day, (unsigned)month, year);
   return std::string (buffer);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the date into a string, in the format specified by the
//            current locale
//Returns   : std::string: String-representation of ADate
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ADate::toString () const {
   return toString ("%x");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the date into a string, in the specified format. The
//            parameter format can be any value accepted by the strftime
//            library-routine.
//Returns   : std::string: String-representation of ADate
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : Reads a date as DDMMY[Y...] from a stream. If the input is
//            not valid, an exception is thrown.
//Parameters: in: Stream to parse
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : Adds another date-value to the date-object. An undefined date is
//            treated as "0.0.0"; so only if both objects are undefined, the
//            result is (remains) undefined. Overflows (of day or month) are
//            corrected.
//Parameters: rhs: Value to add
//Returns   : Self
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : Substracts another date-value from the date-object. An undefined
//            date is treated as "0.0.0"; the result is (remains) undefined if
//            both objects are undefined. Underflows (of day or month) are
//            corrected.
//Parameters: rhs: Value to substract
//Returns   : Self
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : If this is not undefined, the passed values are added (with
//            attention to overflows). The result is returned.
//Parameters: day: Day to add
//            month: Month to add
//            year: Year to add
//Returns   : Self
//Remarks   : In counterpart to the mathematic operators (+ and -) this method
//            does not change the object if it is undefined!
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : If this is defined, the passed values are substracted (with
//            attention to underflows). The result is returned.
//Parameters: day: Day to substract
//            month: Month to substract
//            year: Year to substract
//Returns   : Self
//Remarks   : In counterpart to the mathematic operators (+ and -) this method
//            does not change the object if it is undefined!
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the (approximated) difference in days between two dates.
//            "Younger dates" (closer to the past) are considered bigger
//            than "older dates" (further in the past; that means the numeric
//            value of the date is compared.
//
//            If both dates are undefined, those difference is "0", if only
//            this is undefined the result is MINLONG; if other is undefined
//            MAXLONG is returned (-> undefined dates are considered as
//            (very) old).
//Parameters: other: Object to compare
//Returns   : >0 if this "younger" other; 0 if this == other; <0 else
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : Adds two date-values and returns the result. An undefined date
//            is treated as "0.0.0"; the result is (remains) undefined if both
//            objects are undefined. Overflows (of day or month) are corrected.
//Parameters: lhs: Left-hand-side of addition
//            rhs: Right-hand-side of addition
//Returns   : ADate: Result of additon
/*--------------------------------------------------------------------------*/
ADate operator+ (const ADate& lhs, const ADate& rhs) {
   Check1 (!lhs.checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   ADate result (lhs);
   result += rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substracts two date-values and returns the result. An undefined
//            date is treated as "0.0.0"; the result is (remains) undefined
//            if both objects are undefined. Overflows (of day or month) are
//            corrected.
//Parameters: lhs: Left-hand-side of substraction
//            rhs: Right-hand-side of substraction
//Returns   : ADate: Result of substraction
/*--------------------------------------------------------------------------*/
ADate operator- (const ADate& lhs, const ADate& rhs) {
   Check1 (!lhs.checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   ADate result (lhs);
   result -= rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if this object represents a valid date.
//Returns   : Status; 0: OK
//Remarks   : Even undefined dates must have valid values!
/*--------------------------------------------------------------------------*/
int ADate::checkIntegrity () const {
   TRACE9 ("ADate::checkIntegrity () const - " << (int)day << '.' << (int)month
           << '.' << year);
   return isDefined () ? ((month < 1) || (month > 12)) ? 2 : (day > maxDayOf ()) : 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the maximal day for the the passed month/year-combination
//Parameters: month: Month to check
//            year: Year to check
//Returns   : int: Maximal day for the passed parameters [28 - 31]
/*--------------------------------------------------------------------------*/
char ADate::maxDayOf (char month, int year) {
   Check1 ((month > 0) && (month < 13));

   if (month == (unsigned char)2)              // Special-handling of february
      return (unsigned char)(isLeapYear (year) ? 29 : 28);

   if (month > (unsigned char)7)    // Adapt month after july for easier calc.
      --month;
   return (unsigned char)(month & 1 ? 31 : 30);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed year is a leap-year (years which can be
//            divided by 4; except if it is also divideable by 100)
//Parameters  year: Year to check
//Returns   : bool: True, if leap-year
/*--------------------------------------------------------------------------*/
bool ADate::isLeapYear (int year) {
  TRACE9 ("ADate::isLeapYear: " << year << " %4 = " << (year & 3) << " %100 = "
           << (year % 100) << " %400 = " << (year % 400));
   return (year & 3) ? false : (year % 100) ? true : !(year % 400);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Corrects the object after an (possible) underflows. If the
//            object is integer after the operation, true is returned.
//Returns   : bool: Flag, if object is integer
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : Corrects the object after an (possible) overflows. If the object
//            is integer after the operation, true is returned.
//Returns   : bool: Flag, if object is integer
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the day to the passed value. If the date is not valid, it
//            is set to undefined.
//Parameters: Day: Day to set
/*--------------------------------------------------------------------------*/
void ADate::setDay (char Day) throw (std::invalid_argument) {
   day = Day;

   if (checkIntegrity ()) {
      day = 1;
      throw (std::invalid_argument ("ADate::setDay"));
   }
   else
      setDefined ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the month to the passed value. If the date is not valid,
//            it is set to undefined.
//Parameters: month: Month to set
/*--------------------------------------------------------------------------*/
void ADate::setMonth (char Month) throw (std::invalid_argument) {
   month = Month;

   if (checkIntegrity ()) {
      throw (std::invalid_argument ("ADate::setMonth"));
      undefine ();
   }
   else
      setDefined ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a struct tm.
//Returns   : struct tm: Date in struct tm-format; the time-part is set to zeros
//Remarks   : It is not checked if the date is in the right range for a
//            struct tm (after 1900 and before 2039)
/*--------------------------------------------------------------------------*/
struct tm ADate::toStructTM () const {
   struct tm result = { 0, 0, 0, 0, 0, 0 };
   if (isDefined ()) {
      result.tm_mday = day;
      result.tm_mon = month - 1;
      result.tm_year = year - 1900;
   }
   return result;
}
