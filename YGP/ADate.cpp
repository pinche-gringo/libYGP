//$Id: ADate.cpp,v 1.16 2001/03/25 09:52:42 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ADate
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.16 $
//AUTHOR      : Markus Schwab
//CREATED     : 11.10.1999
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

#include <gzo-cfg.h>

#if SYSTEM == UNIX
#  include <strstream.h>
#else
#  if SYSTEM == WINDOWS
#     define WIN32_LEAN_AND_MEAN
#     include <windows.h>
#  endif
#  include <strstrea.h>
#endif

#define DEBUG 0
#include "Trace_.h"
#include "ADate.h"

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: now: Flag if current time or default start-time (1.1.1900)
//                 should be set
/*--------------------------------------------------------------------------*/
ADate::ADate (bool now) : AttributValue () {
   if (now)
      operator= (time (NULL));
   else
      define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: day: Day for this ADate
//            month: Month
//            year: Year to set
/*--------------------------------------------------------------------------*/
ADate::ADate (char Day, char Month, int Year) : AttributValue (true) {
   day = Day;
   month = Month;
   year = Year;

   if (checkIntegrity ()) {
      TRACE ("ADate::ADate (Day, Month, Year) -> checkIntegrity failed with "
             << checkIntegrity ());
      AttributValue::undefine ();
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
ADate::~ADate () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: other: Object to assign
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ADate& ADate::operator= (const ADate& other) {
   assert (!checkIntegrity ()); assert (!other.checkIntegrity ());

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
//Purpose   : Assignment-operator
//Parameters: pDate: Object to assign as char-string
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ADate& ADate::operator= (const char* pDate) {
   assert (pDate);
   assert (!checkIntegrity ());

   TRACE5 ("ADate::operator= (const char*): " << pDate);

#if defined (__BORLANDC__) || defined (_MSC_VER)
   istrstream help (const_cast <char*> (pDate));
#else
   istrstream help (pDate);
#endif
   readFromStream (help);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string
//Returns   : String-representation of ADate
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ADate::toString () const {
   return toString ("%x");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string giving a certain format (acc .to strftime);
//Returns   : String-representation of ADate
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ADate::toString (const char* format) const {
   assert (format);

   char szBuffer[80] = "";

   if (isDefined ()) {
      struct tm tm (toStructTM ());
      strftime (szBuffer, sizeof (szBuffer), format, &tm);
   }

   if (checkIntegrity ())
      TRACE ("ADate::toString: Invalid ADate " << (int)day << '.'
             << (int)month << '.' << year);
   return std::string (szBuffer);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads string-representation from stream
//Parameters: in: Stream to parse
//TODO      : Parsing according to locale
/*--------------------------------------------------------------------------*/
void ADate::readFromStream (istream& in) {
   unsigned int first (0), second (0);
   char split;

   in >> first >> split >> second >> split >> year;
   TRACE9 ("ADate::readFromStream: Read: " << first << split << second
          << split << year);

   day = (char)first;
   month = (char)second;

   TRACE9 ("ADate::readFromStream: result = " << toString ());
   if (checkIntegrity ()) {
      TRACE ("ADate::readFromStream -> checkIntegrity failed with "
             << checkIntegrity ());
      undefine ();
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: lhs: Value to add
//Returns   : Self
//Note      : If lhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ADate& ADate::operator+= (const ADate& rhs) {
   assert (!checkIntegrity ()); assert (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (isDefined ()) {
         day += rhs.day;
         month += rhs.month;
         year += rhs.year;

         maxAdapt ();
      }
      else
         operator= (rhs);

      assert (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substract a value from this
//Parameters: lhs: Value to substract
//Returns   : Self
//Note      : - If lhs is not defined, this is not changed
//            - If rhs is a date after this, this is set to undefined
/*--------------------------------------------------------------------------*/
ADate& ADate::operator-= (const ADate& rhs) {
   assert (!checkIntegrity ()); assert (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (isDefined ()) {
         day -= rhs.day;
         month -= rhs.month;
         year -= rhs.year;
      }
      else {
         AttributValue::define ();
         day = -rhs.day;
         month = -rhs.month;
         year = -rhs.year;
      } // endif this not defined

      minAdapt ();
      assert (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: day: Day to add
//            month: Month to add
//            year: Year to add
//Returns   : Self
/*--------------------------------------------------------------------------*/
ADate& ADate::add (signed char Day, signed char Month, int Year) {
   TRACE7 ("ADate::add: " << toString () << " + " << (int)Day << '.'
	   << (int)Month << '.' << Year);
   assert (!checkIntegrity ());

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

      assert (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: day: Day to substract
//            month: Month to substract
//            year: Year to substract
//Returns   : Self
/*--------------------------------------------------------------------------*/
ADate& ADate::sub (signed char Day, signed char Month, int Year) {
   TRACE7 ("ADate::sub: " << toString () << " - " << (int)Day << '.'
	   << (int)Month << '.' << Year);

   assert (!checkIntegrity ());

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

      assert (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares two ADate-values
//Parameters: other: Object to compare
//Returns   : >0 if this "younger" other; 0 if this == other; <0 else
//Note      : Undefined values are considered as (incredible) old
//            "Younger dates" (closer to the past) are considered bigger
//            than "older dates" (further in the past; that means the numeric
//            value of the date is compared.
/*--------------------------------------------------------------------------*/
long ADate::compare (const ADate& other) {
   assert (!checkIntegrity ()); assert (!other.checkIntegrity ());

   // Both sides are defined -> return (approximated) difference
   if (isDefined () && other.isDefined ()) {
     TRACE5 ("ADate::compare -> " << ((year - other.year) * 365
                                      + ((month - other.month) * 31)
                                      + day - other.day));

      return ((year - other.year) * 365 + ((month - other.month) * 31)
              + day - other.day);
   }

   if (isDefined ())                             // this defined: Return bigger
      return 1;
   else if (other.isDefined ())                // other defined: Return smaller
      return -1;
   else
      return 0;                               // Both not defined: Return equal
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds two ADate-values
//Parameters: lhs: Left-hand-side of addition
//            rhs: Right-hand-side of addition
//Returns   : ADate: Result of additon
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ADate operator+ (const ADate& lhs, const ADate& rhs) {
   assert (!lhs.checkIntegrity ()); assert (!rhs.checkIntegrity ());

   ADate result (lhs);
   result += rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substracts two ADate-values
//Parameters: lhs: Left-hand-side of substraction
//            rhs: Right-hand-side of substraction
//Returns   : ADate: Result of substraction
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ADate operator- (const ADate& lhs, const ADate& rhs) {
   assert (!lhs.checkIntegrity ()); assert (!rhs.checkIntegrity ());

   ADate result (lhs);
   result -= rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the status of the object
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int ADate::checkIntegrity () const {
   return ((month < 1) || (month > 12)) ? 2 : (day > maxDayOf ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the maximal day of the passed month/year-combination
//Parameters: month: Month to check
//            year: Year to check
//Returns   : int: Max. day
/*--------------------------------------------------------------------------*/
char ADate::maxDayOf (char month, int year) {
   assert ((month > 0) && (month < 13));

   if (month == (unsigned char)2)              // Special-handling of february
      return (unsigned char)(isLeapYear (year) ? 29 : 28);

   if (month > (unsigned char)7)    // Adapt month after july for easier calc.
      --month;
   return (unsigned char)(month & 1 ? 31 : 30);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed day is a leap-year (years which can be
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
//Purpose   : Adapt value after recalculation with possible underflow
//Returns   : bool: Flag, if object is integer
/*--------------------------------------------------------------------------*/
bool ADate::minAdapt () {
   TRACE7 ("ADate::minAdapt: " << toString ());
   if (((unsigned char)(month - 1)) > 11) {
      int mon (255 - (unsigned char)(month - 1));
      year -= (mon / 12) + 1;
      month = 12 - (mon % 12);
   }
   
   TRACE9 ("ADate::minAdapt (month dapted (1)): " << toString ());

   while ((day < 1) || (day > maxDayOf ())) {       // Adapt date if underflow
      --month;
      if (month < 1) {
         month = 12;
         --year;
         TRACE9 ("ADate::minAdapt (month adapted (2)): " << toString ());
      }

      if (((unsigned char)day - 1) & 0x80)
         day += maxDayOf ();
      else {
         day -= maxDayOf ();
         month++;
         if (month > 12) {
            month = 1;
            year++;
         }
      }
      TRACE9 ("ADate::minAdapt (day adapted (1)): " << toString ());
   } // endif day invalid

   return !ADate::checkIntegrity ();      // Can only ensure proper ADate-part
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible overflow
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
//Purpose   : Sets the day-part of this
//Parameters: Day: Day to set
/*--------------------------------------------------------------------------*/
void ADate::setDay (char Day) {
   day = Day;

   if (checkIntegrity ()) {
      TRACE ("ADate::setDay -> checkIntegrity failed with " << checkIntegrity ());
      day = 1;
      undefine ();
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the month-part of this
//Parameters: month: Month to set
/*--------------------------------------------------------------------------*/
void ADate::setMonth (char Month) {
   month = Month;

   if (checkIntegrity ()) {
      TRACE ("ADate::setMonth -> checkIntegrity failed with " << checkIntegrity ());
      month = 1;
      undefine ();
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a struct tm
//Returns   : struct tm: Date in struct tm-format; time-part is set to zeros
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
