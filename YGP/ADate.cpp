//$Id: ADate.cpp,v 1.2 1999/10/12 21:39:14 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ADate
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
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

#include <time.h>
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
   define ();
   if (now) {
      time_t curtime (time (NULL));
      struct tm* locTime (localtime (&curtime)); assert (locTime);
      day = locTime->tm_mday;
      month = locTime->tm_mon + 1;                   // tm_mon is from 0 to 11
      year = locTime->tm_year + 1900;
   }
   else {
      day = month = 1;
      year = 1900;
   }

   if (checkIntegrity ())
      TRACE ("ADate::ADate -> checkIntegrity failed with " << checkIntegrity ());
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

   maxAdapt ();

   if (checkIntegrity ())
      TRACE ("ADate::ADate (Day, Month, Year) -> checkIntegrity failed with "
             << checkIntegrity ());

}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
ADate::~ADate () {
   assert (!checkIntegrity ());
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

      if (checkIntegrity ())
         TRACE ("ADate::operator= -> checkIntegrity failed with "
                << checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: pDate: Object to assign as char-string
//Returns   : Reference to self
//TODO      : Parsing according to locale
/*--------------------------------------------------------------------------*/
ADate& ADate::operator= (const char* pDate) {
   assert (pDate);
   assert (!checkIntegrity ());

   TRACE5 ("ADate::operator= (const char*): " << pDate);

   unsigned int first (0), second (0);
   char split;
   
   istrstream help (pDate);
   help >> first >> split >> second >> year;
   
   day = (char)first;
   month = (char)second;
   define ();
   
   if (checkIntegrity ()) {
      TRACE ("ADate::operator= (const char* -> checkIntegrity failed with "
             << checkIntegrity ());
      maxAdapt ();
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string
//Returns   : String-representation of ADate
/*--------------------------------------------------------------------------*/
std::string ADate::toString () const {
   assert (!checkIntegrity ());
   return toString ("%x");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string giving a certain format (acc .to strftime);
//Returns   : String-representation of ADate
/*--------------------------------------------------------------------------*/
std::string ADate::toString (const char* format) const {
   assert (!checkIntegrity ());
   assert (format);

   char szBuffer[20] = "";

   if (isDefined ()) {
      struct tm tm = { 0, 0, 0, day, month - 1, year - 1900 };
      strftime (szBuffer, sizeof (szBuffer), format, &tm);
   }
   return szBuffer;
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
      day += rhs.day;
      month += rhs.month;
      year += rhs.year;
      
      maxAdapt ();
      AttributValue::define ();

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
      day -= rhs.day;
      month -= rhs.month;
      year -= rhs.year;

      minAdapt ();
      AttributValue::define ();

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

   day += Day;
   month += Month;
   year += Year;
   maxAdapt ();

   assert (!checkIntegrity ());
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

   day -= Day;
   month -= Month;
   year -= Year;
   minAdapt ();

   assert (!checkIntegrity ());
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
   assert ((month > 0) && (month < 12));

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
void ADate::minAdapt () {
   if (day < 1)                                     // Adapt date if underflow
      --month;

   if ((month < 1) || (month > 12)) {              // Adapt month if underflow
      month += 12;              // Assuming calculation was with correct month
      --year;
   }

   if (day < 1)                  // Finish day-adaption after month-correction
      day += maxDayOf ();

   assert (!checkIntegrity ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible overflow
/*--------------------------------------------------------------------------*/
void ADate::maxAdapt () {
   int maxDay (maxDayOf ());                         // Adapt date if overflow
   if (day > maxDay) {
      day -= maxDay;
      ++month;
   }

   if (month > 12) {                                // Adapt month if overflow
      month -= 12;              // Assuming calculation was with correct month
      ++year;
   }
   assert (!checkIntegrity ());
}
