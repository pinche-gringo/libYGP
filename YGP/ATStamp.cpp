//$Id: ATStamp.cpp,v 1.12 2002/11/30 05:03:32 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : ATimestamp
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.12 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.10.1999
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

#include <strstream>

#include <stdexcept>

#include "Check.h"
#include "Trace_.h"

#include "ATStamp.h"

/*--------------------------------------------------------------------------*/
//Purpose   : Defaultconstructor; the timestamp is not defined
/*--------------------------------------------------------------------------*/
ATimestamp::ATimestamp () : ADate (), ATime () {
   TRACE5 ("ATimestamp::ATimestamp");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; depending on the parameter the timestamp is either
//            set to the 1st of January, 1900 0:00:00 (now = false), or to the
//            current time (now = true).
//Parameters: now: Flag if current time or default time (1.1.1900) should be set
/*--------------------------------------------------------------------------*/
ATimestamp::ATimestamp (bool now) : ADate (now), ATime (now) {
   TRACE5 ("ATimestamp::ATimestamp (" << (now ? "true)" : "false)"));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; sets the passed time. The object is undefined, if
//            the passed parameters represent no valid date/time-combination
//            (e.g. Hour > 23, minute > 59, second > 61 or day > 31).
//Parameters: day: Day for this ATimestamp
//            month, year, hour, minute, second: Other time-parameters
/*--------------------------------------------------------------------------*/
ATimestamp::ATimestamp (char Day, char Month, int Year, char Hour,
                        char minute, char second) : ADate (Day, Month, Year)
   , ATime (Hour, minute, second) {
   if (checkIntegrity ()) {
      TRACE ("ATimestamp::ATimestamp (Day, Month, Year, Hour, minute, second)"
             " -> checkIntegrity failed with " << checkIntegrity ());
      undefine ();
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
ATimestamp::~ATimestamp () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator from another timestamp object
//Parameters: other: Object to assign
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator= (const ATimestamp& other) {
   Check3 (!checkIntegrity ()); Check3 (!other.checkIntegrity ());

   if (this != &other) {
      TRACE5 ("ATimestamp::operator=: " << other);

      ADate::operator= ((const ADate&)other);          // Calls checkIntegrity
      ATime::operator= ((const ATime&)other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator from an const char-pointer. The timestamp
//            must be passed as DDMMY[Y...] hhmmss. If the buffer does
//            not represent a valid timestamp, an exception is thrown.
//Parameters: pStamp: Character array specifying timestamp to assign
//Returns   : Reference to self
//Remarks   : A NULL-pointer as parameter is not permitted!
//Throws    : std::invalid_argument if the parameters has a wrong format
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator= (const char* pStamp) throw (std::invalid_argument) {
   Check3 (pStamp);
   Check3 (!checkIntegrity ());

   TRACE5 ("ATimestamp::operator= (const char*): " << pStamp);

#if defined (__BORLANDC__) || defined (_MSC_VER)
   std::istrstream help (const_cast <char*> (pStamp));
#else
   istrstream help (pStamp);
#endif
   readFromStream (help);
   return *this;

}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator; assigns the values from the passed struct tm.
//Parameters: tm: Object to assign as char-string
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator= (const struct tm& tm) {
   ADate::operator= (tm);
   ATime::operator= (tm);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the object into a string, in the format
//            DDMMYYYY[...] hhmmss.
//Returns   : String-representation of ATimestamp
/*--------------------------------------------------------------------------*/
std::string ATimestamp::toUnformatedString () const {
   std::string ret (ADate::toUnformatedString ());
   ret += ' ';
   ret += ATime::toUnformatedString ();
   return ret;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the object into a string, in a format specified by the
//            current locale.
//Returns   : String-representation of ATimestamp
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ATimestamp::toString () const {
   return toString ("%x %X");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the object into a string, in the specified format. The
//            parameter format can be any value accepted by the strftime
//            library-routine.
//Returns   : String-representation of ATimestamp
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ATimestamp::toString (const char* format) const {
   return ADate::toString (format);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads the timestamp in the format DDMMY[Y...] hhmmss (with
//            leading zeros) from a stream. If the input is not a valid
//            timestamp an exception is thrown.
//Parameters: in: Stream to parse
//Throws    : std::invalid_argument in case of a format error
/*--------------------------------------------------------------------------*/
void ATimestamp::readFromStream (std::istream& in) throw (std::invalid_argument) {
   char ch;

   ADate::readFromStream (in);
   in.get (ch);
   ATime::readFromStream (in);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds another timestamp-value to the object. An undefined
//            timestamp is treated as "0.0.0 0:00:00"; if both objects are
//            undefined, the result is undefined. Overflows are corrected.
//Parameters: rhs: Value to add
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator+= (const ATimestamp& rhs) {
   Check3 (!checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATime::operator+= ((const ATime&)rhs);
   ADate::operator+= ((const ADate&)rhs);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substracts another timestamp-value to the object. An undefined
//            timestamp is treated as "0.0.0 0:00:00"; if both objects are
//            undefined, the result is undefined. Underflows are corrected.
//Parameters: rhs: Value to substract
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator-= (const ATimestamp& rhs) {
   Check3 (!checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATime::operator-= ((const ATime&)rhs);
   ADate::operator-= ((const ADate&)rhs);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : If this is not undefined, the passed values are added. Overflows
//            are corrected. The result is returned.
//
//            In counterpart to the mathematic operators (+ and -)
//            this method does not change the object if it is undefined!
//Parameters: day: Day to add
//            month: Month to add
//            year: Year to add
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::add (char Day, char Month, int Year,
                             char Hour, char minute ,char second) {
   Check3 (!checkIntegrity ());

   ATime::add (Hour, minute, second);
   ADate::add (Day, Month, Year);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : If this is not undefined, the passed values are substracted.
//            Underflows are corrected. The result is returned.
//
//            In counterpart to the mathematic operators (+ and -)
//            this method does not change the object if it is undefined!
//Parameters: day: Day to substract
//            month: Month to substract
//            year: Year to substract
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::sub (char Day, char Month, int Year,
                             char Hour, char minute ,char second) {
   Check3 (!checkIntegrity ());

   ATime::sub (Hour, minute, second);
   ADate::sub (Day, Month, Year);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the (approximated) difference in days, or - if the day
//            is equal - in seconds between two timestamps. If both objects
//            are undefined, those difference is "0", if only this is
//            undefined the result is MINLONG, if only other is undefined
//            MAXLONG is returned (-> undefined times are considered to be
//            very old).
//Parameters: other: Object to compare
//Returns   : >0 if this is closer to the past than other; 0 if this == other; <0 else
/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the addition of two timestamp-values. An undefined time
//            is treated as "0.0.0 0:00:00"; if both objects are undefined,
//            the result is undefined. Overflows are corrected.
//Parameters: lhs: Left-hand-side of addition
//            rhs: Right-hand-side of addition
//Returns   : ATimestamp: Result of additon
/*--------------------------------------------------------------------------*/
ATimestamp operator+ (const ATimestamp& lhs, const ATimestamp& rhs) {
   Check3 (!lhs.checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATimestamp result (lhs);
   result += rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the substraction of two timestamp-values. An undefined
//            time is treated as "0.0.0 0:00:00"; if both objects are
//            undefined, the result is undefined. Underflows are corrected.
//Parameters: lhs: Left-hand-side of substraction
//            rhs: Right-hand-side of substraction
//Returns   : ATimestamp: Result of substraction
/*--------------------------------------------------------------------------*/
ATimestamp operator- (const ATimestamp& lhs, const ATimestamp& rhs) {
   Check3 (!lhs.checkIntegrity ()); Check3 (!rhs.checkIntegrity ());

   ATimestamp result (lhs);
   result -= rhs;
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if this object represents a valid timestamp. Note: Even
//            undefined times must have valid values!
//Returns   : Status; 0: OK
/*--------------------------------------------------------------------------*/
int ATimestamp::checkIntegrity () const {
   int rc (ATime::checkIntegrity ());
   return rc ? rc * 10 : ADate::checkIntegrity ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Corrects the object after underflows. If the the object is
//            integer after the operation, true is returned (else false).
//Returns   : bool: True, if object is integer after the operation
/*--------------------------------------------------------------------------*/
bool ATimestamp::minAdapt () {
   if (ATime::minAdapt ())
      decDay ();

   return ADate::minAdapt ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Corrects the object after overflows. If the the object is
//            integer after the operation, true is returned (else false).
//Returns   : bool: True, if object is integer after the operation
/*--------------------------------------------------------------------------*/
bool ATimestamp::maxAdapt () {
   if (ATime::maxAdapt ())
      incDay ();
   return ADate::maxAdapt ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the object to a struct tm.
//Returns   : struct tm: Timestamp in struct tm-format
//Remarks   : It is not checked if the date is in the right range for a
//            struct tm (after 1900 and before 2039)
/*--------------------------------------------------------------------------*/
struct tm ATimestamp::toStructTM () const {
   struct tm result (ADate::toStructTM ());
   if (isDefined ()) {
      result.tm_hour = getHour ();
      result.tm_min = getMinute ();
      result.tm_sec = getSecond ();
   }
   return result;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converts the object to a system-timestructure (as GMT).
//Returns   : time_t: Converted time
/*--------------------------------------------------------------------------*/
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
