//$Id: ATime.cpp,v 1.32 2004/11/04 16:31:18 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : ATime
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.32 $
//AUTHOR      : Markus Schwab
//CREATED     : 15.10.1999
//COPYRIGHT   : Copyright (C) 1999 - 2005

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

#include <ygp-cfg.h>

#if SYSTEM == WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include <sstream>
#include <stdexcept>

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Internal.h"

#include "YGP/ATime.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Constructor; depending on the parameter the time is either set to
/// "0:00:00" (now = false), or to the current time (now = true).
/// \param now: Flag if current time or default start-time (1.1.1900) should be set
//-----------------------------------------------------------------------------
ATime::ATime (bool now) : AttributValue () {
   if (now)
      operator= (time (NULL));
   else
      define ();
}

//-----------------------------------------------------------------------------
/// Constructor; sets the passed time. The object is undefined, if the passed
/// parameters represent no valid time (e.g. Hour > 23, minute > 59 or second
/// > 61)
/// \param Hour: Hour for this ATime
/// \param minute: Minute to set
/// \param second: Second to set
//-----------------------------------------------------------------------------
ATime::ATime (char Hour, char minute, char second) throw (std::invalid_argument)
   : AttributValue (true), hour (Hour), min_ (minute), sec (second)  {
   int status (checkIntegrity ());
   if (status)
      throw (std::invalid_argument (status == 3 ? "Hour"
                                    : status == 2 ? "Minute" : "Second"));
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ATime::~ATime () {
}


//-----------------------------------------------------------------------------
/// Assignment-operator from another ATime object
/// \param other: Object to assign
/// \returns Reference to self
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Assignment-operator from an const char-pointer. The time must be passed as
/// HHMMSS. If the buffer does not represent a valid time an excpetion is
/// thrown. A NULL-pointer as parameter is not permitted!
/// \param pTime: Character array specifying time to assign
/// \returns Reference to self
/// \throw std::invalid_argument if the parameter does not represent a
///     valid time
//-----------------------------------------------------------------------------
ATime& ATime::operator= (const char* pTime) throw (std::invalid_argument) {
   Check3 (pTime);
   Check3 (!checkIntegrity ());

   TRACE5 ("ATime::operator= (const char*): " << pTime);

   std::istringstream help (pTime);
   readFromStream (help);
   return *this;
}

//-----------------------------------------------------------------------------
/// Converts the time into a string, in format hhmmss (each part with leading
/// zeros).
/// \returns String-representation of ATime
//-----------------------------------------------------------------------------
std::string ATime::toUnformattedString () const {
   char buffer[8];

   sprintf (buffer, "%02d%02d%02d", (unsigned)hour, (unsigned)min_, (unsigned)sec);
   return std::string (buffer);
}

//-----------------------------------------------------------------------------
/// Converts the time into a string, in a format specified by the current
/// locale.
/// \returns String-representation of ATime
/// \remarks Only dates valid for struct tm can be printed (e.g. dates after
///     1900)
//-----------------------------------------------------------------------------
std::string ATime::toString () const {
   return toString ("%X");
}

//-----------------------------------------------------------------------------
/// Converts the time into a string, in the specified format. The parameter
/// format can be any value accepted by the strftime library-routine.
/// \returns String-representation of ATime
/// \remarks Only dates valid for struct tm can be printed (e.g. dates after
///     1900)
//-----------------------------------------------------------------------------
std::string ATime::toString (const char* format) const {
   TRACE9 ("ATime::toString (const char*) const - " << format);
   Check3 (format);

   std::string result;

   if (isDefined ()) {
      struct tm tm (toStructTM ());
#ifdef STRFTIME_RETURNS_LENGTH
      char aBuffer[strftime (NULL, 200, format, &tm) + 1];
#else
      char aBuffer[80];
#endif
      strftime (aBuffer, sizeof (aBuffer), format, &tm);
      result = aBuffer;
   }
   return result;
}

//-----------------------------------------------------------------------------
/// Reads the time in format hhmmss (with leading zeros) from a stream.
/// \param in: Stream to parse
/// \throw invalid_argument in case of an format error
//-----------------------------------------------------------------------------
void ATime::readFromStream (std::istream& in) throw (std::invalid_argument) {
   if (in.eof ()) {
      undefine ();
      return;
   }
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

//-----------------------------------------------------------------------------
/// Adds another time-value to the object. An undefined time is treated as
/// "0:00:00"; if both objects are undefined, the result is undefined.
/// Overflows of seconds or minutes are corrected, overflows of the hour
/// undefines the object.
/// \param rhs: Value to add
/// \returns Self Note : If lhs is not defined this is not changed
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Substracts another time-value from the object. An undefined time is
/// treated as "0:00:00"; the result is undefined if both objects are
/// undefined. Underflows (of day or month) are corrected; underflows of of
/// the hour undefines the object.
/// \param rhs: Value to substract
/// \returns Self
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// If this is not undefined, the passed values are added. Overflows of
/// seconds or minutes are corrected, overflows of the hour undefines the
/// object. The result is returned.
/// \param Hour: Hour to add
/// \param minute: Minute to add
/// \param second: Second to add
/// \returns Self
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// If this is not undefined, the passed values are substracted. Overflows of
/// seconds or minutes are corrected, overflows of the hour undefines the
/// object. The result is returned.
/// \param Hour: Hour to substract
/// \param minute: Minute to substract
/// \param second: Second to substract
/// \returns Self
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Returns the (approximated) difference in seconds between two times. If
/// both times are undefined, those difference is "0", if only this is
/// undefined the result is MINLONG, if only other is undefined MAXLONG is
/// returned (-> undefined times are considered as (very) old).
/// \param other: Object to compare
/// \returns \c long: >0 if this other; 0 if this == other; <0 else
///     \note Undefined values are considered as (incredible) old "Younger
///         dates" (closer to the past) are considered bigger than "older
///         dates"(further in the past; that means the numeric value of the
///         date is compared.
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Returns the addition of two time-values. An undefined time is treated as
/// "0:00:00"; if both objects are undefined, the result is undefined.
/// Overflows of seconds or minutes are corrected, overflows of the hour
/// undefines the object.
/// \param lhs: Left-hand-side of addition
/// \param rhs: Right-hand-side of addition
/// \returns \c ATime: Result of additon
//-----------------------------------------------------------------------------
ATime operator+ (const ATime& lhs, const ATime& rhs) {
   Check3 (!lhs.checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATime result (lhs);
   result += rhs;
   return result;
}

//-----------------------------------------------------------------------------
/// Returns the substraction of two time-values. An undefined time is treated
/// as "0:00:00"; if both objects are undefined, the result is undefined.
/// Underflows of seconds or minutes are corrected, underflows of the hour
/// undefines the object.
/// \param lhs: Left-hand-side of substraction
/// \param rhs: Right-hand-side of substraction
/// \returns \c ATime: Result of substraction
//-----------------------------------------------------------------------------
ATime operator- (const ATime& lhs, const ATime& rhs) {
   Check3 (!lhs.checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATime result (lhs);
   result -= rhs;
   return result;
}

//-----------------------------------------------------------------------------
/// Checks if this object represents a valid time. Note: Even undefined times
/// must have valid values!
/// \returns Status; 0: OK
//-----------------------------------------------------------------------------
int ATime::checkIntegrity () const {
   TRACE9 ("ATime::checkIntegrity () const - " << (int)hour << ':'
           << (int)min_ << ':' << (int)sec);

   return (hour > 23) ? 3 : (min_ > 59) ? 2 : (sec > 61);
}

//-----------------------------------------------------------------------------
/// Corrects the object after an (possible) underflows. If the hour has an
/// underflow. true is returned, else false.
/// \returns \c bool: True, if there�s a underflow of hour
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Corrects the object after an (possible) overflows. If the hour has an
/// overflow true is returned, else false.
/// \returns \c bool: True, if there�s a overflow of the hour
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Sets the hour of the passed valued
/// \param Hour: Hour to set
/// \throw std::invalid_argument if the parameter is bigger than 23
//-----------------------------------------------------------------------------
void ATime::setHour (char Hour) throw (std::invalid_argument) {
   if (hour > 23) {
      TRACE0 ("ATime::setHour -> Invalid parameter: " << Hour);
      throw std::invalid_argument ("ATime::setHour");
   }
   hour = Hour;
   setDefined ();
}

//-----------------------------------------------------------------------------
/// Sets the minute to the passed value
/// \param minute: Minute to set
/// \throw std::invalid_argument if the parameter is bigger than 59
//-----------------------------------------------------------------------------
void ATime::setMinute (char minute) throw (std::invalid_argument) {
   if (minute > 59) {
      TRACE0 ("ATime::setMinute -> Invalid parameter: " << minute);
      throw std::invalid_argument ("ATime::setMinute");
   }
   min_ = minute;
   setDefined ();
}

//-----------------------------------------------------------------------------
/// Sets the second to the passed value.
/// \param second: Second to set
/// \throw std::invalid_argument if the parameter is bigger than 61
//-----------------------------------------------------------------------------
void ATime::setSecond (char second) throw (std::invalid_argument) {
   if (second > 61) {
      TRACE0 ("ATime::setSecond -> Invalid parameter: " << second);
      throw std::invalid_argument ("ATime::setSecond");
   }
   sec = second;
   setDefined ();
}

//-----------------------------------------------------------------------------
/// Converts the time to a struct tm
/// \returns <tt>struct tm</tt>: Time in struct tm-format; the date-part is set
///     to zeros
/// \remarks It is not checked if the date is in the right range for a struct
///     tm (after 1900 and before 2039)
//-----------------------------------------------------------------------------
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

}
