//$Id: ATime.cpp,v 1.15 2002/11/04 00:51:18 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : ATime
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.15 $
//AUTHOR      : Markus Schwab
//CREATED     : 15.10.1999
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
#include <assert.h>
#include <locale.h>

#include <gzo-cfg.h>

#if SYSTEM == WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include <strstream>
#include <stdexcept>

#include "Trace_.h"
#include "Internal.h"

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
      setDefined ();
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
ATime& ATime::operator= (const char* pDate) throw (std::invalid_argument) {
   assert (pDate);
   assert (!checkIntegrity ());

   TRACE5 ("ATime::operator= (const char*): " << pDate);

#if defined (__BORLANDC__) || defined (_MSC_VER)
   std::istrstream help (const_cast <char*> (pDate));
#else
   istrstream help (pDate);
#endif
   readFromStream (help);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to an unformated string
//Returns   : String-representation of ATime
/*--------------------------------------------------------------------------*/
std::string ATime::toUnformatedString () const {
   char buffer[8];

   sprintf (buffer, "%02d%02d%02d", (unsigned)hour, (unsigned)min_, (unsigned)sec);
   return buffer;
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
   TRACE9 ("ATime::toString (const char*) const - " << format);
   assert (format);

   char szBuffer[80] = "";

   if (isDefined ()) {
      struct tm tm (toStructTM ());
      strftime (szBuffer, sizeof (szBuffer), format, &tm);
   }
   return szBuffer;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads string-representation from stream
//Parameters: in: Stream to parse
//TODO      : Parsing according to locale
/*--------------------------------------------------------------------------*/
void ATime::readFromStream (std::istream& in) throw (std::invalid_argument) {
   static unsigned char ATime::* const targets[] = {
      &ATime::hour, &ATime::min_, &ATime::sec };

   hour = min_ = sec = 0;

   int ch;
   int i(0);
   for (; i < 6; ++i) {
      ch = in.get ();
      if ((ch == EOF) || ((ch > '9') || (ch < '0')))
         break;

      TRACE8 ("ATime::readFromStream (istream&): Get: " << (char)ch)

      this->*(targets[i >> 1]) += (ch & 0xf);
      if (!(i & 1))
         this->*(targets[i >> 1]) *= 10;
   } // endfor

   TRACE9 ("ATime::readFromStream (istream&): Read: " << (int)hour << ':' << (int)min_
           << ':' << (int)sec);

   if ((i < 6) || checkIntegrity ()) {
      undefine ();
      if (i) {
         std::string error (_("Position %1"));
         error.replace (error.find ("%1"), 2, 1, char (i + '0'));
         throw std::invalid_argument (error);
      }
   }
   else {
      TRACE9 ("ATime::readFromStream (istream&): Define");
      setDefined ();
   }
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
   if (isDefined ()) {
      if (other.isDefined ()) {
         TRACE5 ("ATime::compare -> " << (((hour - other.hour) * 24
                                           + (min_ - other.min_) * 60)
                                          + (sec - other.sec)));

         return ((hour - other.hour) * 24 + (min_ - other.min_) * 60)
            + (sec - other.sec);
      }
      else
         return 1;                    // this defined, other not: Return bigger
   }
   else
      return other.isDefined () ? -1 : 0;
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
   TRACE9 ("ATime::checkIntegrity () const - " << (int)hour << ':'
           << (int)min_ << ':' << (int)sec);

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
      setDefined ();
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
      setDefined ();
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
      setDefined ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a struct tm
//Returns   : struct tm: Date in struct tm-format; time-part is set to zeros
//Remarks   : It is not checked if the date is in the right range for a
//            struct tm (after 1900 and before 2039)
/*--------------------------------------------------------------------------*/
struct tm ATime::toStructTM () const {
   struct tm result;

   memset (&result, '\0', sizeof (result));
   if (isDefined ()) {
      result.tm_hour = hour;
      result.tm_min = min_;
      result.tm_sec = sec;
   }
   return result;
}
