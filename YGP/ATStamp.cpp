//$Id: ATStamp.cpp,v 1.17 2003/06/15 23:53:01 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : ATimestamp
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.17 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.10.1999
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

#include <sstream>
#include <stdexcept>

#include "Check.h"
#include "Trace_.h"

#include "ATStamp.h"

//-----------------------------------------------------------------------------
/// Defaultconstructor; the timestamp is not defined
//-----------------------------------------------------------------------------
ATimestamp::ATimestamp () : ADate (), ATime () {
   TRACE5 ("ATimestamp::ATimestamp");
}

//-----------------------------------------------------------------------------
/// Constructor; depending on the parameter the timestamp is either set to the
/// 1st of January, 1900 0:00:00 (now = false), or to the current time (now =
/// true).
/// \param now: Flag if current time or default time (1.1.1900) should be set
//-----------------------------------------------------------------------------
ATimestamp::ATimestamp (bool now) : ADate (now), ATime (now) {
   TRACE5 ("ATimestamp::ATimestamp (" << (now ? "true)" : "false)"));
}

//-----------------------------------------------------------------------------
/// Constructor; sets the passed time. The object is undefined, if the passed
/// parameters represent no valid date/time-combination (e.g. Hour > 23,
/// minute > 59, second > 61 or day > 31).
/// \param Day: Day for this object
/// \param Month
/// \param Year
/// \param Hour
/// \param minute
/// \param second: Other time-parameters (self explaining, I think)
//-----------------------------------------------------------------------------
ATimestamp::ATimestamp (char Day, char Month, int Year, char Hour,
                        char minute, char second) throw (std::invalid_argument)
   : ADate (Day, Month, Year), ATime (Hour, minute, second) {
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ATimestamp::~ATimestamp () {
}


//-----------------------------------------------------------------------------
/// Assignment-operator from another timestamp object
/// \param other: Object to assign
/// \returns Reference to self
//-----------------------------------------------------------------------------
ATimestamp& ATimestamp::operator= (const ATimestamp& other) {
   Check3 (!checkIntegrity ()); Check3 (!other.checkIntegrity ());

   if (this != &other) {
      TRACE5 ("ATimestamp::operator=: " << other);

      ADate::operator= ((const ADate&)other);          // Calls checkIntegrity
      ATime::operator= ((const ATime&)other);
   }
   return *this;
}

//-----------------------------------------------------------------------------
/// Assignment-operator from an const char-pointer. The timestamp must be
/// passed as DDMMY[Y...] hhmmss. If the buffer does not represent a valid
/// timestamp, an exception is thrown.
/// \param pValue: Character array specifying timestamp to assign
/// \returns Reference to self
/// \throw \c std::invalid_argument if the parameters has a wrong format
/// \remarks A NULL-pointer as parameter is not permitted!
//-----------------------------------------------------------------------------
ATimestamp& ATimestamp::operator= (const char* pValue) throw (std::invalid_argument) {
   Check3 (pValue);
   Check3 (!checkIntegrity ());

   TRACE5 ("ATimestamp::operator= (const char*): " << pValue);

   std::istringstream help (pValue);
   readFromStream (help);
   return *this;
}

//-----------------------------------------------------------------------------
/// Assignment-operator; assigns the values from the passed struct tm.
/// \param tm: Object to assign as char-string
/// \returns Reference to self
//-----------------------------------------------------------------------------
ATimestamp& ATimestamp::operator= (const struct tm& tm) {
   ADate::operator= (tm);
   ATime::operator= (tm);
   return *this;
}

//-----------------------------------------------------------------------------
/// Converts the object into a string, in the format DDMMYYYY[...] hhmmss.
/// \returns String-representation of ATimestamp
//-----------------------------------------------------------------------------
std::string ATimestamp::toUnformattedString () const {
   std::string ret (ADate::toUnformattedString ());
   ret += ' ';
   ret += ATime::toUnformattedString ();
   return ret;
}

//-----------------------------------------------------------------------------
/// Converts the object into a string, in a format specified by the current
/// locale.
/// \returns String-representation of the object
/// \remarks Only dates valid for <tt>struct tm</tt> can be printed (e.g. dates
///     after 1900)
//-----------------------------------------------------------------------------
std::string ATimestamp::toString () const {
   return toString ("%x %X");
}

//-----------------------------------------------------------------------------
/// Converts the object into a string, in the specified format. The parameter
/// format can be any value accepted by the strftime library-routine.
/// \returns String-representation of the object
/// \remarks Only dates valid for <tt>struct tm</tt> can be printed (e.g. dates
///     after 1900)
//-----------------------------------------------------------------------------
std::string ATimestamp::toString (const char* format) const {
   return ADate::toString (format);
}

//-----------------------------------------------------------------------------
/// Reads the timestamp in the format DDMMY[Y...] hhmmss (with leading zeros)
/// from a stream. If the input is not a valid timestamp an exception is
/// thrown.
/// \param in: Stream to parse
/// \throw \c std::invalid_argument in case of a format error
//-----------------------------------------------------------------------------
void ATimestamp::readFromStream (std::istream& in) throw (std::invalid_argument) {
   char ch;

   ADate::readFromStream (in);
   in.get (ch);
   ATime::readFromStream (in);
}

//-----------------------------------------------------------------------------
/// Adds another timestamp-value to the object. An undefined timestamp is
/// treated as "0.0.0 0:00:00"; if both objects are undefined, the result is
/// undefined. Overflows are corrected.
/// \param rhs: Value to add
/// \returns Self
//-----------------------------------------------------------------------------
ATimestamp& ATimestamp::operator+= (const ATimestamp& rhs) {
   Check3 (!checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATime::operator+= ((const ATime&)rhs);
   ADate::operator+= ((const ADate&)rhs);
   return *this;
}

//-----------------------------------------------------------------------------
/// Substracts another timestamp-value to the object. An undefined timestamp
/// is treated as "0.0.0 0:00:00"; if both objects are undefined, the result
/// is undefined. Underflows are corrected.
/// \param rhs: Value to substract
/// \returns Self
//-----------------------------------------------------------------------------
ATimestamp& ATimestamp::operator-= (const ATimestamp& rhs) {
   Check3 (!checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATime::operator-= ((const ATime&)rhs);
   ADate::operator-= ((const ADate&)rhs);
   return *this;
}

//-----------------------------------------------------------------------------
/// If this is not undefined, the passed values are added. Overflows are
/// corrected. The result is returned. In counterpart to the mathematic
/// operators (+ and -) this method does not change the object if it is
/// undefined!
/// \param Day: Day to add
/// \param Month: Month to add
/// \param Year: Year to add
/// \param Hour: Hour to add
/// \param minute: Minute to add
/// \param second: Second to add
/// \returns Self
//-----------------------------------------------------------------------------
ATimestamp& ATimestamp::add (char Day, char Month, int Year,
                             char Hour, char minute ,char second) {
   Check3 (!checkIntegrity ());

   ATime::add (Hour, minute, second);
   ADate::add (Day, Month, Year);
   return *this;
}

//-----------------------------------------------------------------------------
/// If this is not undefined, the passed values are substracted. Underflows
/// are corrected. The result is returned. In counterpart to the mathematic
/// operators (+ and -) this method does not change the object if it is
/// undefined!
/// \param Day: Day to substract
/// \param Month: Month to substract
/// \param Year: Year to substract
/// \param Hour: Hour to substract
/// \param minute: Minute to substract
/// \param second: Second to substract
/// \returns Self
//-----------------------------------------------------------------------------
ATimestamp& ATimestamp::sub (char Day, char Month, int Year,
                             char Hour, char minute ,char second) {
   Check3 (!checkIntegrity ());

   ATime::sub (Hour, minute, second);
   ADate::sub (Day, Month, Year);
   return *this;
}

//-----------------------------------------------------------------------------
/// Returns the (approximated) difference in days, or - if the day is equal -
/// in seconds between two timestamps.
///
/// "Younger times" (closer to the past) are considered bigger than "older
/// times" (further in the past; that means the numeric value of the time is
/// compared (e.g.: "yesterday" < "now" < "tomorrow").
///
/// If both objects are undefined, those difference is "0", if only this is
/// undefined the result is MINLONG, if only other is undefined MAXLONG is
/// returned (-> undefined times are considered to be very old).
/// \param other: Object to compare
/// \returns \c long: >0 if this is closer to the past than other; 0 if this
///      == other; <0 else
//-----------------------------------------------------------------------------
long ATimestamp::compare (const ATimestamp& other) {
   Check3 (!checkIntegrity ()); Check3 (!other.checkIntegrity ());

   // Both sides are defined -> return (approximated) difference
   long rc (ADate::compare (other));
   if (!rc) {
      rc = ATime::compare (other);

      TRACE5 ("ATimestamp::compare -> " << rc);
   }
   return rc;
}

//-----------------------------------------------------------------------------
/// Returns the addition of two timestamp-values. An undefined time is treated
/// as "0.0.0 0:00:00"; if both objects are undefined, the result is
/// undefined. Overflows are corrected.
/// \param lhs: Left-hand-side of addition
/// \param rhs: Right-hand-side of addition
/// \returns \c ATimestamp: Result of additon
//-----------------------------------------------------------------------------
ATimestamp operator+ (const ATimestamp& lhs, const ATimestamp& rhs) {
   Check3 (!lhs.checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATimestamp result (lhs);
   result += rhs;
   return result;
}

//-----------------------------------------------------------------------------
/// Returns the substraction of two timestamp-values. An undefined time is
/// treated as "0.0.0 0:00:00"; if both objects are undefined, the result is
/// undefined. Underflows are corrected.
/// \param lhs: Left-hand-side of substraction
/// \param rhs: Right-hand-side of substraction
/// \returns \c ATimestamp: Result of substraction
//-----------------------------------------------------------------------------
ATimestamp operator- (const ATimestamp& lhs, const ATimestamp& rhs) {
   Check3 (!lhs.checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATimestamp result (lhs);
   result -= rhs;
   return result;
}

//-----------------------------------------------------------------------------
/// Checks if this object represents a valid timestamp. Note: Even undefined
/// times must have valid values!
/// \returns Status; 0: OK
//-----------------------------------------------------------------------------
int ATimestamp::checkIntegrity () const {
   int rc (ATime::checkIntegrity ());
   return rc ? rc * 10 : ADate::checkIntegrity ();
}

//-----------------------------------------------------------------------------
/// Corrects the object after underflows. If the the object is integer after
/// the operation, true is returned (else false).
/// \returns \c bool: True, if object is integer after the operation
//-----------------------------------------------------------------------------
bool ATimestamp::minAdapt () {
   if (ATime::minAdapt ())
      decDay ();

   return ADate::minAdapt ();
}

//-----------------------------------------------------------------------------
/// Corrects the object after overflows. If the the object is integer after
/// the operation, true is returned (else false).
/// \returns \c bool: True, if object is integer after the operation
//-----------------------------------------------------------------------------
bool ATimestamp::maxAdapt () {
   if (ATime::maxAdapt ())
      incDay ();
   return ADate::maxAdapt ();
}

//-----------------------------------------------------------------------------
/// Converts the object to a struct tm.
/// \returns <tt>struct tm</tt>: Timestamp in struct tm-format
/// \remarks It is not checked if the date is in the right range for a
///    <tt>struct tm</tt> (after 1900 and before 2039)
//-----------------------------------------------------------------------------
struct tm ATimestamp::toStructTM () const {
   struct tm result (ADate::toStructTM ());
   if (isDefined ()) {
      result.tm_hour = getHour ();
      result.tm_min = getMinute ();
      result.tm_sec = getSecond ();
   }
   return result;
}

//-----------------------------------------------------------------------------
/// Converts the object to a system-timestructure (as GMT).
/// \returns \c time_t: Converted time
//-----------------------------------------------------------------------------
time_t ATimestamp::toGMTTime () const {
#ifdef HAVE_TIMEGM
   struct tm result (toStructTM ());
   return timegm (&result);
#else
   std::string TZ (getenv ("TZ"));
   putenv ("TZ=UTC");
   _tzset ();
   time_t utcTime (toLocalTime ());
   TZ = "TZ=" + TZ;
   putenv (TZ.c_str ());
   _tzset ();
   return utcTime;
#endif
}
