//$Id: ATStamp.cpp,v 1.5 2000/03/23 19:28:45 Markus Rel $

//PROJECT     : General
//SUBSYSTEM   : ATimestamp
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
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

#ifdef UNIX
#include <strstream.h>
#else
#include <strstrea.h>
#endif

#define DEBUG 0
#include "Trace_.h"

#include "ATStamp.h"

/*--------------------------------------------------------------------------*/
//Purpose   : Default-constructor
/*--------------------------------------------------------------------------*/
ATimestamp::ATimestamp () : ADate (), ATime () {
   TRACE5 ("ATimestamp::ATimestamp");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: now: Flag if current time or default start-time (1.1.1900)
//                 should be set
/*--------------------------------------------------------------------------*/
ATimestamp::ATimestamp (bool now) : ADate (now), ATime (now) {
   TRACE5 ("ATimestamp::ATimestamp (" << now ? "true)" : "false)");
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: day: Day for this ATimestamp
//            mont, year, hour, minute, second: Other time-parameters
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
//Purpose   : Assignment-operator
//Parameters: other: Object to assign
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator= (const ATimestamp& other) {
   assert (!checkIntegrity ()); assert (!other.checkIntegrity ());

   if (this != &other) {
      TRACE5 ("ATimestamp::operator=: " << other);

      ADate::operator= ((const ADate&)other);          // Calls checkIntegrity
      ATime::operator= ((const ATime&)other);
   }
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: pStamp: Char-String to assign
//Returns   : Reference to self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator= (const char* pStamp) {
   assert (pStamp);
   assert (!checkIntegrity ());

   TRACE5 ("ATimestamp::operator= (const char*): " << pDate);

#if defined (__BORLANDC__) || defined (_MSC_VER)
   istrstream help (const_cast <char*> (pStamp));
#else
   istrstream help (pStamp);
#endif
   readFromStream (help);
   return *this;

}

/*--------------------------------------------------------------------------*/
//Purpose   : Assignment-operator
//Parameters: pDate: Object to assign as char-string
//Returns   : Reference to self
//TODO      : Parsing according to locale
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator= (const struct tm& tm) {
   ADate::operator= (tm);
   ATime::operator= (tm);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Converting to a string
//Returns   : String-representation of ATimestamp
//Remarks   : Only dates valid for struct tm can be printed (e.g. dates after
//            1900)
/*--------------------------------------------------------------------------*/
std::string ATimestamp::toString () const {
   return toString ("%x %X");
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
   ATime::readFromStream (in);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: lhs: Value to add
//Returns   : Self
//Note      : If lhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator+= (const ATimestamp& rhs) {
   assert (!checkIntegrity ()); assert (!rhs.checkIntegrity ());

   ATime::operator+= ((const ATime&)rhs);
   ADate::operator+= ((const ADate&)rhs);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Substract a value from this
//Parameters: lhs: Value to substract
//Returns   : Self
//Note      : If lhs is not defined this is not changed
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::operator-= (const ATimestamp& rhs) {
   assert (!checkIntegrity ()); assert (!rhs.checkIntegrity ());

   ATime::operator-= ((const ATime&)rhs);
   ADate::operator-= ((const ADate&)rhs);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: day: Day to add
//            month: Month to add
//            year: Year to add
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::add (char Day, char Month, int Year,
                             char Hour, char minute ,char second) {
   assert (!checkIntegrity ());

   ATime::add (Hour, minute, second);
   ADate::add (Day, Month, Year);
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a value to this
//Parameters: day: Day to substract
//            month: Month to substract
//            year: Year to substract
//Returns   : Self
/*--------------------------------------------------------------------------*/
ATimestamp& ATimestamp::sub (char Day, char Month, int Year,
                             char Hour, char minute ,char second) {
   assert (!checkIntegrity ());

   ATime::sub (Hour, minute, second);
   ADate::sub (Day, Month, Year);
   return *this;
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
      rc = ATime::compare (other);

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
   int rc (ATime::checkIntegrity ());
   return rc ? rc * 10 : ADate::checkIntegrity ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible underflow
/*--------------------------------------------------------------------------*/
bool ATimestamp::minAdapt () {
   if (ATime::minAdapt ())
      decDay ();

   return ADate::minAdapt ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adapt value after recalculation with possible overflow
/*--------------------------------------------------------------------------*/
bool ATimestamp::maxAdapt () {
   if (ATime::maxAdapt ())
      incDay ();
   return ADate::maxAdapt ();
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
      result.tm_hour = getHour ();
      result.tm_min = getMinute ();
      result.tm_sec = getSecond ();
   }
   return result;
}
