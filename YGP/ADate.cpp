//$Id: ADate.cpp,v 1.6 1999/10/15 21:34:26 Markus Rel $

//PROJECT     : General
//SUBSYSTEM   : ADate
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
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

#include <assert.h>
#include <values.h>
#include <locale.h>

#ifdef UNIX
#include <strstream.h>
#else
#include <strstrea.h>
#endif

#define DEBUG 0
#include "Trace.h"

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
ADate::ADate (char Day, char Month, unsigned int Year) : AttributValue () {
   day = Day;
   month = Month;
   year = Year;

   if (checkIntegrity ()) {
      TRACE ("ADate::ADate (Day, Month, Year) -> checkIntegrity failed with "
             << checkIntegrity ());
   }
   else
      AttributValue::define ();
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

   istrstream help (pDate);
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
   return szBuffer;
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
//Note      : If lhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ADate& ADate::operator-= (const ADate& rhs) {
   assert (!checkIntegrity ()); assert (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (!isDefined ())
         *this = today ();

      day -= rhs.day;
      month -= rhs.month;
      year -= rhs.year;

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
ADate& ADate::add (char Day, char Month, unsigned int Year) {
   assert (!checkIntegrity ());

   if (isDefined ()) {
      day += Day;
      month += Month;
      year += Year;
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
ADate& ADate::sub (char Day, char Month, unsigned int Year) {
   assert (!checkIntegrity ());

   if (isDefined ()) {
      day -= Day;
      month -= Month;
      year -= Year;
      minAdapt ();

      assert (!checkIntegrity ());
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares two ADate-values
//Parameters: other: Object to compare
//Returns   : >0 if this  other; 0 if this == other; <0 else
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
      return MAXLONG;
   else if (other.isDefined ())                // other defined: Return smaller
      return MINLONG;
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
char  ADate::maxDayOf (char month, unsigned int year) {
   assert ((month > 0) && (month < 13));

   if (month == 2)                             // Special-handling of february
      return isLeapYear (year) ? 29 : 28;

   if (month > 7)             // Adapt month after july for easier calculation
      --month;
   return month & 1 ? 31 : 30;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed day is a leap-year (years which can be
//            divided by 4; except if it is also divideable by 100)
//Parameters  year: Year to check
//Returns   : bool: True, if leap-year
/*--------------------------------------------------------------------------*/
bool ADate::isLeapYear (unsigned int year) {
  TRACE9 ("ADate::isLeapYear: " << year << " %4 = " << (year & 3) << " %100 = "
           << (year % 100) << " %400 = " << (year % 400));
   return (year & 3) ? false : (year % 100) ? true : !(year % 400);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible underflow
/*--------------------------------------------------------------------------*/
bool ADate::minAdapt () {
   if ((day < 1) || (day > maxDayOf ()))            // Adapt date if underflow
      --month;

   if ((month < 1) || (month > 12)) {              // Adapt month if underflow
      month += 12;              // Assuming calculation was with correct month
      --year;
   }

   if ((day < 1) || (day > maxDayOf ()))                    // Finish adaption
      day += maxDayOf ();

   return !checkIntegrity ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible overflow
/*--------------------------------------------------------------------------*/
bool ADate::maxAdapt () {
   int maxDay (maxDayOf ());                         // Adapt date if overflow
   if (day > maxDay) {
      day -= maxDay;
      ++month;
   }

   if (month > 12) {                                // Adapt month if overflow
      month -= 12;              // Assuming calculation was with correct month
      ++year;
   }
   return !checkIntegrity ();
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
