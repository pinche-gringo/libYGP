//$Id: ATime.cpp,v 1.6 2001/01/11 20:18:46 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ATime
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 15.10.1999
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

#if defined UNIX || defined __GNUG__
#  include <strstream.h>
#else
#  ifdef WINDOWS
#     define WIN32_LEAN_AND_MEAN
#     include <windows.h>
#  endif
#  include <strstrea.h>
#endif

#define DEBUG 0
#include "Trace_.h"

#include "ATime.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: now: Flag if current time or default start-time (1.1.1900)
//                 should be set
/*--------------------------------------------------------------------------*/
ATime::ATime (bool now) : AttributValue () {
   if (now)
      operator= (time (NULL));
   else
      define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: Hour: Hour for this ATime
//            minute: Minute
//            second: Second to set
/*--------------------------------------------------------------------------*/
ATime::ATime (char Hour, char minute, char second) : AttributValue (),
   hour (Hour), min_ (minute), sec (second) {
   if (checkIntegrity ()) {
      TRACE ("ATime::ATime (Hour, minute, second) -> checkIntegrity failed with "
             << checkIntegrity ());
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
ATime::~ATime () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: other: Object to assign
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ATime& ATime::operator= (const ATime& other) {
   assert (!checkIntegrity ()); assert (!other.checkIntegrity ());

   if (this != &other) {
      TRACE5 ("ATime::operator=: " << other);

      hour = other.hour;
      min_ = other.min_;
      sec = other.sec;
      AttributValue::operator= ((const AttributValue&) other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: pDate: Object to assign as char-string
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ATime& ATime::operator= (const char* pDate) {
   assert (pDate);
   assert (!checkIntegrity ());

   TRACE5 ("ATime::operator= (const char*): " << pDate);

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
//Returns   : String-representation of ATime
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ATime::toString () const {
   return toString ("%X");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string giving a certain format (acc .to strftime);
//Returns   : String-representation of ATime
/*--------------------------------------------------------------------------*/
std::string ATime::toString (const char* format) const {
   assert (format);

   char szBuffer[80] = "";

   if (isDefined ()) {
      struct tm tm (toStructTM ());
      strftime (szBuffer, sizeof (szBuffer), format, &tm);
   }

   if (checkIntegrity ())
      TRACE ("ATime::toString: Invalid ATime " << (int)hour << '.'
             << (int)min_ << '.' << (char)sec);
   return szBuffer;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads string-representation from stream
//Parameters: in: Stream to parse
//TODO      : Parsing according to locale
/*--------------------------------------------------------------------------*/
void ATime::readFromStream (istream& in) {
   unsigned int first (0), second (0), third (0);
   char split;

   in >> first >> split >> second >> split >> third;
   TRACE9 ("ATime::readFromStream: Read: " << first << split << second
          << split << third);

   hour = (char)first;
   min_ = (char)second;
   sec = (char)third;

   TRACE9 ("ATime::readFromStream: result = " << toString ());
   if (checkIntegrity ()) {
      TRACE ("ATime::readFromStream -> checkIntegrity failed with "
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
ATime& ATime::operator+= (const ATime& rhs) {
   assert (!checkIntegrity ()); assert (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (isDefined ()) {
         hour += rhs.hour;
         min_ += rhs.min_;
         sec += rhs.sec;

         if (maxAdapt ())
            undefine ();
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
ATime& ATime::operator-= (const ATime& rhs) {
   assert (!checkIntegrity ()); assert (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (!isDefined ())
         *this = now ();

      hour -= rhs.hour;
      min_ -= rhs.min_;
      sec -= rhs.sec;

      if (minAdapt ())
         undefine ();

      assert (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: Hour: Hour to add
//            minute: Minute to add
//            sec: Second to add
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATime& ATime::add (char Hour, char minute, char second) {
   assert (!checkIntegrity ());

   if (isDefined ()) {
      hour += Hour;
      min_ += minute;
      sec += second;
      if (maxAdapt ())
         undefine ();

      assert (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: Hour: Hour to substract
//            minute: Minute to substract
//            second: Second to substract
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATime& ATime::sub (char Hour, char minute, char second) {
   assert (!checkIntegrity ());

   if (isDefined ()) {
      hour -= Hour;
      min_ -= minute;
      sec -= second;
      if (minAdapt ())
         undefine ();

      assert (!checkIntegrity ());
   }

   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares two ATime-values
//Parameters: other: Object to compare
//Returns   : >0 if this  other; 0 if this == other; <0 else
//Note      : Undefined values are considered as (incredible) old
//            "Younger dates" (closer to the past) are considered bigger
//            than "older dates" (further in the past; that means the numeric
//            value of the date is compared.
/*--------------------------------------------------------------------------*/
long ATime::compare (const ATime& other) {
   assert (!checkIntegrity ()); assert (!other.checkIntegrity ());

   // Both sides are defined -> return (approximated) difference
   if (isDefined () && other.isDefined ()) {
     TRACE5 ("ATime::compare -> " << (((hour - other.hour) * 24
                                       + (min_ - other.min_) * 60)
                                      + (sec - other.sec)));

      return ((hour - other.hour) * 24 + (min_ - other.min_) * 60)
              + (sec - other.sec);
   }

   if (isDefined ())                             // this defined: Return bigger
      return 1;
   else if (other.isDefined ())                // other defined: Return smaller
      return -1;
   else
      return 0;                               // Both not defined: Return equal
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds two ATime-values
//Parameters: lhs: Left-hand-side of addition
//            rhs: Right-hand-side of addition
//Returns   : ATime: Result of additon
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ATime operator+ (const ATime& lhs, const ATime& rhs) {
   assert (!lhs.checkIntegrity ()); assert (!rhs.checkIntegrity ());

   ATime result (lhs);
   result += rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substracts two ATime-values
//Parameters: lhs: Left-hand-side of substraction
//            rhs: Right-hand-side of substraction
//Returns   : ATime: Result of substraction
//Note      : Undefined values are ignored
/*--------------------------------------------------------------------------*/
ATime operator- (const ATime& lhs, const ATime& rhs) {
   assert (!lhs.checkIntegrity ()); assert (!rhs.checkIntegrity ());

   ATime result (lhs);
   result -= rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the status of the object
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int ATime::checkIntegrity () const {
   return (hour > 23) ? 3 : (min_ > 59) ? 2 : (sec > 61);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible underflow
//Returns   : bool: True, if there´s a underflow of hour
/*--------------------------------------------------------------------------*/
bool ATime::minAdapt () {
   if (sec > 61) {                                  // Adapt time if underflow
      --min_;
      sec += 59;
   }

   if (min_ > 59) {                                // Adapt month if underflow
      min_ += 59;               // Assuming calculation was with correct month
      --hour;
   }

   if (hour > 23) {              // Finish day-adaption after month-correction
      hour -= 23;
      return true;
   }
   assert (!ATime::checkIntegrity ());    // Can only ensure proper ATime-part
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible overflow
//Returns   : bool: True, if there´s a overflow of hour
/*--------------------------------------------------------------------------*/
bool ATime::maxAdapt () {
   if (sec > 59) {                                   // Adapt time if overflow
      sec -= 59;
      ++min_;
   }

   if (min_ > 59) {                                // Adapt minute if overflow
      min_ -= 59;              // Assuming calculation was with correct minute
      ++hour;
   }

   if (hour > 23) {
      hour -= 23;
      return true;
   }
   assert (!ATime::checkIntegrity ());     // Can only ensure proper ATime-part
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the day-part of this
//Parameters: Hour: Hour to set
/*--------------------------------------------------------------------------*/
void ATime::setHour (char Hour) {
   hour = Hour;

   if (checkIntegrity ()) {
      TRACE ("ATime::setHour -> checkIntegrity failed with " << checkIntegrity ());
      hour = 0;
      undefine ();
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the minute-part of this
//Parameters: minute: Minute to set
/*--------------------------------------------------------------------------*/
void ATime::setMinute (char minute) {
   min_ = minute;

   if (checkIntegrity ()) {
      TRACE ("ATime::setMinute -> checkIntegrity failed with " << checkIntegrity ());
      minute = 0;
      undefine ();
   }
   else
      AttributValue::define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the second-part of this
//Parameters: second: Second to set
/*--------------------------------------------------------------------------*/
void ATime::setSecond (char second) {
   sec = second;

   if (checkIntegrity ()) {
      TRACE ("ATime::setMinute -> checkIntegrity failed with " << checkIntegrity ());
      sec = 0;
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
struct tm ATime::toStructTM () const {
   struct tm result = { 0, 0, 0, 0, 0, 0 };
   if (isDefined ()) {
      result.tm_hour = hour;
      result.tm_min = min_;
      result.tm_sec = sec;
   }
   return result;
}
