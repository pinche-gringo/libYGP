//$Id: ATime.cpp,v 1.18 2003/01/16 16:46:22 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ATime
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.18 $
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

#include <gzo-cfg.h>

#if SYSTEM == WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include <strstream>
#include <stdexcept>

#include "Check.h"
#include "Trace_.h"
#include "Internal.h"

#include "ATime.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; depending on the parameter the time is either set
//            to "0:00:00" (now = false), or to the current time (now = true).
//Parameters: now: Flag if current time or default start-time (1.1.1900) should be set
/*--------------------------------------------------------------------------*/
ATime::ATime (bool now) : AttributValue () {
   if (now)
      operator= (time (NULL));
   else
      define ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; sets the passed time. The object is undefined, if
//            the passed parameters represent no valid time (e.g. Hour > 23,
//            minute > 59 or second > 61)
//Parameters: Hour: Hour for this ATime
//            minute: Minute to set
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
//Purpose   : Assignment-operator from another ATime object
//Parameters: other: Object to assign
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ATime& ATime::operator= (const ATime& other) {
   Check3 (!checkIntegrity ()); Check3 (!other.checkIntegrity ());

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
//Purpose   : Assignment-operator from an const char-pointer. The time must be
//            passed as HHMMSS. If the buffer does not represent a valid time
//            an excpetion is thrown. A NULL-pointer as parameter is not
//            permitted!
//Parameters: pTime: Character array specifying time to assign
//Returns   : Reference to self
//Throws    : std::invalid_argument if the parameter does not represent a valid time
/*--------------------------------------------------------------------------*/
ATime& ATime::operator= (const char* pTime) throw (std::invalid_argument) {
   Check3 (pTime);
   Check3 (!checkIntegrity ());

   TRACE5 ("ATime::operator= (const char*): " << pTime);

#if defined (__BORLANDC__) || defined (_MSC_VER)
   std::istrstream help (const_cast <char*> (pTime));
#else
   istrstream help (pTime);
#endif
   readFromStream (help);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the time into a string, in format hhmmss (each part with
//            leading zeros).
//Returns   : String-representation of ATime
/*--------------------------------------------------------------------------*/
std::string ATime::toUnformatedString () const {
   char buffer[8];

   sprintf (buffer, "%02d%02d%02d", (unsigned)hour, (unsigned)min_, (unsigned)sec);
   return std::string (buffer);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the time into a string, in a format specified by the
//            current locale.
//Returns   : String-representation of ATime
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ATime::toString () const {
   return toString ("%X");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the time into a string, in the specified format. The
//            parameter format can be any value accepted by the strftime
//            library-routine.
//Returns   : String-representation of ATime
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ATime::toString (const char* format) const {
   TRACE9 ("ATime::toString (const char*) const - " << format);
   Check3 (format);

   std::string result;

   if (isDefined ()) {
      struct tm tm (toStructTM ());
#if defined (__BORLANDC__) || defined (_MSC_VER)
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
//Purpose   : Reads the time in format hhmmss (with leading zeros) from a stream.
//Parameters: in: Stream to parse
//Throws    : invalid_argument in case of an format error
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
//Purpose   : Adds another time-value to the object. An undefined time is
//            treated as "0:00:00"; if both objects are undefined, the result
//            is undefined. Overflows of seconds or minutes are corrected,
//            overflows of the hour undefines the object.
//Parameters: lhs: Value to add
//Returns   : Self
//Note      : If lhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ATime& ATime::operator+= (const ATime& rhs) {
   Check3 (!checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

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

      Check3 (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substracts another time-value from the object. An undefined
//            time is treated as "0:00:00"; the result is undefined if both
//            objects are undefined. Underflows (of day or month) are
//            corrected; underflows of of the hour undefines the object.
//Parameters: lhs: Value to substract
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATime& ATime::operator-= (const ATime& rhs) {
   Check3 (!checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (!isDefined ())
         *this = now ();

      hour -= rhs.hour;
      min_ -= rhs.min_;
      sec -= rhs.sec;

      if (minAdapt ())
         undefine ();

      Check3 (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : If this is not undefined, the passed values are added.
//            Overflows of seconds or minutes are corrected, overflows of the
//            hour undefines the object. The result is returned.
//Parameters: Hour: Hour to add
//            minute: Minute to add
//            second: Second to add
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATime& ATime::add (char Hour, char minute, char second) {
   Check3 (!checkIntegrity ());

   if (isDefined ()) {
      hour += Hour;
      min_ += minute;
      sec += second;
      if (maxAdapt ())
         undefine ();

      Check3 (!checkIntegrity ());
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : If this is not undefined, the passed values are substracted.
//            Overflows of seconds or minutes are corrected, overflows of the
//            hour undefines the object. The result is returned.
//Parameters: Hour: Hour to substract
//            minute: Minute to substract
//            second: Second to substract
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATime& ATime::sub (char Hour, char minute, char second) {
   Check3 (!checkIntegrity ());

   if (isDefined ()) {
      hour -= Hour;
      min_ -= minute;
      sec -= second;
      if (minAdapt ())
         undefine ();

      Check3 (!checkIntegrity ());
   }

   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the (approximated) difference in seconds between two
//            times. If both times are undefined, those difference is "0", if
//            only this is undefined the result is MINLONG, if only other is
//            undefined MAXLONG is returned (-> undefined times are considered
//            as (very) old).
//Parameters: other: Object to compare
//Returns   : >0 if this  other; 0 if this == other; <0 else
//Note      : Undefined values are considered as (incredible) old
//            "Younger dates" (closer to the past) are considered bigger
//            than "older dates" (further in the past; that means the numeric
//            value of the date is compared.
/*--------------------------------------------------------------------------*/
long ATime::compare (const ATime& other) {
   Check3 (!checkIntegrity ()); Check3 (!other.checkIntegrity ());

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
//Purpose   : Returns the addition of two time-values. An undefined time is
//            treated as "0:00:00"; if both objects are undefined, the result
//            is undefined. Overflows of seconds or minutes are corrected,
//            overflows of the hour undefines the object.
//Parameters: lhs: Left-hand-side of addition
//            rhs: Right-hand-side of addition
//Returns   : ATime: Result of additon
/*--------------------------------------------------------------------------*/
ATime operator+ (const ATime& lhs, const ATime& rhs) {
   Check3 (!lhs.checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATime result (lhs);
   result += rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the substraction of two time-values. An undefined time
//            is treated as "0:00:00"; if both objects are undefined, the
//            result is undefined. Underflows of seconds or minutes are
//            corrected, underflows of the hour undefines the object.
//Parameters: lhs: Left-hand-side of substraction
//            rhs: Right-hand-side of substraction
//Returns   : ATime: Result of substraction
/*--------------------------------------------------------------------------*/
ATime operator- (const ATime& lhs, const ATime& rhs) {
   Check3 (!lhs.checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATime result (lhs);
   result -= rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if this object represents a valid time. Note: Even
//            undefined times must have valid values!
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int ATime::checkIntegrity () const {
   TRACE9 ("ATime::checkIntegrity () const - " << (int)hour << ':'
           << (int)min_ << ':' << (int)sec);

   return (hour > 23) ? 3 : (min_ > 59) ? 2 : (sec > 61);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Corrects the object after an (possible) underflows. If the hour
//            has an underflow. true is returned, else false.
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
   Check3 (!ATime::checkIntegrity ());    // Can only ensure proper ATime-part
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Corrects the object after an (possible) overflows. If the hour
//            has an overflow true is returned, else false.
//Returns   : bool: True, if there´s a overflow of the hour
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
   Check3 (!ATime::checkIntegrity ());     // Can only ensure proper ATime-part
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the hour of the passed valued
//Parameters: Hour: Hour to set
//Throws    : std::invalid_argument if the parameter is bigger than 23
/*--------------------------------------------------------------------------*/
void ATime::setHour (char Hour) throw (std::invalid_argument) {
   if (hour > 23) {
      TRACE ("ATime::setHour -> Invalid parameter: " << Hour);
      throw std::invalid_argument ("ATime::setSHour");
   }
   hour = Hour;
   setDefined ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the minute to the passed value
//Parameters: minute: Minute to set
//Throws    : std::invalid_argument if the parameter is bigger than 59
/*--------------------------------------------------------------------------*/
void ATime::setMinute (char minute) throw (std::invalid_argument) {
   if (minute > 59) {
      TRACE ("ATime::setMinute -> Invalid parameter: " << minute);
      throw std::invalid_argument ("ATime::setMinute");
   }
   min_ = minute;
   setDefined ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the second to the passed value.
//Parameters: second: Second to set
//Throws    : std::invalid_argument if the parameter is bigger than 61
/*--------------------------------------------------------------------------*/
void ATime::setSecond (char second) throw (std::invalid_argument) {
   if (second > 61) {
      TRACE ("ATime::setSecond -> Invalid parameter: " << second);
      throw std::invalid_argument ("ATime::setSecond");
   }
   sec = second;
   setDefined ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the time to a struct tm
//Returns   : struct tm: Time in struct tm-format; the date-part is set to zeros
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
