//$Id: ATStamp.cpp,v 1.1 1999/10/14 22:23:32 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ATimestamp
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.10.1999
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

#include "ATStamp.h"

/*--------------------------------------------------------------------------*/
//Purpose   : Default-constructor
/*--------------------------------------------------------------------------*/
ATimestamp::ATimestamp () : ADate (), hour (0), min (0), sec (0) {
   TRACE5 ("ATimestamp::ATimestamp");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: now: Flag if current time or default start-time (1.1.1900)
//                 should be set
/*--------------------------------------------------------------------------*/
ATimestamp::ATimestamp (bool now) : ADate () {
   TRACE5 ("ATimestamp::ATimestamp (" << now ? "true)" : "false)");
   if (now)
      operator= (time (NULL));
   else
      hour = min = sec = 0;

   if (checkIntegrity ()) {
      TRACE ("ATimestamp::ATimestamp -> checkIntegrity failed with " << checkIntegrity ())
      undefine ();
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: day: Day for this ATimestamp
//            mont, year, hour, minute, second: Other time-parameters
/*--------------------------------------------------------------------------*/
ATimestamp::ATimestamp (char Day, char Month, unsigned int Year, char Hour,
                        char minute, char second) : ADate (Day, Month, Year)
   , hour (Hour), min (minute), sec (second) {
   if (checkIntegrity ()) {
      TRACE ("ATimestamp::ATimestamp (Day, Month, Year) -> checkIntegrity failed with "
             << checkIntegrity ());
      undefine ();
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
ATimestamp::~ATimestamp () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: other: Object to assign
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator= (const ATimestamp& other) {
   assert (!checkIntegrity ()); assert (!other.checkIntegrity ());

   if (this != &other) {
      TRACE5 ("ATimestamp::operator=: " << other);

      hour = other.hour;
      min = other.min;
      sec = other.sec;
      ADate::operator= ((const ADate&) other);         // Calls checkIntegrity
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: pDate: Object to assign as char-string
//Returns   : Reference to self
//TODO      : Parsing according to locale
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator= (const struct tm& tm) {
   hour = tm.tm_hour;
   min = tm.tm_min;
   sec = tm.tm_sec;

   ADate::operator= (tm);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string
//Returns   : String-representation of ATimestamp
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ATimestamp::toString () const {
   return toString ("%c");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string giving a certain format (acc .to strftime);
//Returns   : String-representation of ATimestamp
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ATimestamp::toString (const char* format) const {
   return ADate::toString (format);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads string-representation from stream
//Parameters: in: Stream to parse
//TODO      : Parsing according to locale
/*--------------------------------------------------------------------------*/
void ATimestamp::readFromStream (istream& in) {
   ADate::readFromStream (in);
   if (!isDefined ())
      return;

   int one, two, three;
   char split;

   in >> one >> split >> two >> split >> three;
   hour = (char)one;
   min = (char)two;
   sec = (char)three;

   if (checkIntegrity ()) {
      TRACE ("ATimestamp::readFromStream: Invalid time-part " << (int)hour << ':'
             << (int)min << ':' << (int)sec);
      undefine ();
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: lhs: Value to add
//Returns   : Self
//Note      : If lhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator+= (const ATimestamp& rhs) {
   assert (!checkIntegrity ()); assert (!rhs.checkIntegrity ());

   if (isDefined () && rhs.isDefined ()) {
      hour += rhs.hour;
      min += rhs.min;
      sec += rhs.sec;
   }
   return (ATimestamp&)ADate::operator+= ((const ADate&)rhs);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substract a value from this
//Parameters: lhs: Value to substract
//Returns   : Self
//Note      : If lhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator-= (const ATimestamp& rhs) {
   assert (!checkIntegrity ()); assert (!rhs.checkIntegrity ());

   if (isDefined () && rhs.isDefined ()) {
      hour -= rhs.hour;
      min -= rhs.min;
      sec -= rhs.sec;
   }
   return (ATimestamp&)ADate::operator-= ((const ADate&)rhs);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: day: Day to add
//            month: Month to add
//            year: Year to add
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::add (char Day, char Month, unsigned int Year,
                             char Hour, char minute ,char second) {
   assert (!checkIntegrity ());

   hour += Hour;
   min += minute;
   sec += second;
   return (ATimestamp&)ADate::add (Day, Month, Year);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: day: Day to substract
//            month: Month to substract
//            year: Year to substract
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::sub (char Day, char Month, unsigned int Year,
                             char Hour, char minute ,char second) {
   assert (!checkIntegrity ());

   hour -= Hour;
   min -= minute;
   sec -= second;
   return (ATimestamp&)ADate::sub (Day, Month, Year);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares two ATimestamp-values
//Parameters: other: Object to compare
//Returns   : >0 if this  other; 0 if this == other; <0 else
//Note      : Undefined values are considered as (incredible) old
//            "Younger dates" (closer to the past) are considered bigger
//            than "older dates" (further in the past; that means the numeric
//            value of the date is compared.
/*--------------------------------------------------------------------------*/
long ATimestamp::compare (const ATimestamp& other) {
   assert (!checkIntegrity ()); assert (!other.checkIntegrity ());

   // Both sides are defined -> return (approximated) difference
   long rc (ADate::compare (other));
   if (!rc) {
      rc = ((hour - other.hour) * 24 + (min - other.min) * 60)
            + (sec - other.sec);

      TRACE5 ("ATimestamp::compare -> " << rc);
   }
   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds two ATimestamp-values
//Parameters: lhs: Left-hand-side of addition
//            rhs: Right-hand-side of addition
//Returns   : ATimestamp: Result of additon
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ATimestamp operator+ (const ATimestamp& lhs, const ATimestamp& rhs) {
   assert (!lhs.checkIntegrity ()); assert (!rhs.checkIntegrity ());

   ATimestamp result (lhs);
   result += rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substracts two ATimestamp-values
//Parameters: lhs: Left-hand-side of substraction
//            rhs: Right-hand-side of substraction
//Returns   : ATimestamp: Result of substraction
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ATimestamp operator- (const ATimestamp& lhs, const ATimestamp& rhs) {
   assert (!lhs.checkIntegrity ()); assert (!rhs.checkIntegrity ());

   ATimestamp result (lhs);
   result -= rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the status of the object
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int ATimestamp::checkIntegrity () const {
  return (hour > 23) ? 10 : (min > 59) ? 11 : (sec > 61) ? 12 : ADate::checkIntegrity ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible underflow
/*--------------------------------------------------------------------------*/
void ATimestamp::minAdapt () {
   if (sec > 59) {                                  // Adapt time if underflow
      --min;
      sec += 59;
   }

   if (min > 59) {                                // Adapt minute if underflow
      min += 59;               // Assuming calculation was with correct minute
      --hour;
   }

   if (hour > 23) {              // Finish day-adaption after month-correction
      hour -= 23;
      decDay ();
   }
   ADate::minAdapt ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible overflow
/*--------------------------------------------------------------------------*/
void ATimestamp::maxAdapt () {
   if (sec > 59) {                                   // Adapt date if overflow
      sec -= 59;
      ++min;
   }

   if (min > 59) {                                 // Adapt minute if overflow
      min -= 59;               // Assuming calculation was with correct minute
      ++hour;
   }

   if (hour > 23) {
      hour -= 23;
      incDay ();
   }
   ADate::maxAdapt ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the hour-part of this
//Parameters: Hour: Hour to set
/*--------------------------------------------------------------------------*/
void ATimestamp::setHour (char Hour) {
   hour = Hour;

   if (checkIntegrity ()) {
      TRACE ("ATimestamp::setHour -> checkIntegrity failed with " << checkIntegrity ());
      undefine ();
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the minute-part of this
//Parameters: minute: Minute to set
/*--------------------------------------------------------------------------*/
void ATimestamp::setMinute (char minute) {
   min = minute;

   if (checkIntegrity ()) {
      TRACE ("ATimestamp::setMinute -> checkIntegrity failed with " << checkIntegrity ());
      undefine ();
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the second-part of this
//Parameters: second: Second to set
/*--------------------------------------------------------------------------*/
void ATimestamp::setSecond (char second) {
   sec = second;

   if (checkIntegrity ()) {
      TRACE ("ATimestamp::setSecond -> checkIntegrity failed with " << checkIntegrity ());
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
struct tm ATimestamp::toStructTM () const {
   struct tm result (ADate::toStructTM ());
   if (isDefined ()) {
      result.tm_hour = hour;
      result.tm_min = min;
      result.tm_sec = sec;
   }
   return result;
}
