//$Id: ADate.cpp,v 1.56 2008/06/10 21:47:20 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : ADate
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.56 $
//AUTHOR      : Markus Schwab
//CREATED     : 11.10.1999
//COPYRIGHT   : Copyright (C) 1999 - 2008

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdio>
#include <cstring>

#include <ygp-cfg.h>

#if SYSTEM == WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include <stdexcept>

#include "YGP/Trace.h"
#include "YGP/Internal.h"

#include "YGP/ADate.h"


namespace YGP {

//----------------------------------------------------------------------------
/// Constructor; depending on the parameter the date is either set to the first
/// of January, 1900 (now = false), or to the current day (now = true)
/// \param now Flag which date to set (1900/1/1 (false) or now (true))
//----------------------------------------------------------------------------
ADate::ADate (bool now) : AYear (1900), day ((unsigned char)1), month ((unsigned char)1) {
   if (now)
      operator= (time (NULL));
   else
      setDefined ();
}

//----------------------------------------------------------------------------
/// Constructor; sets the passed date. The date is undefined, if the passed
/// parameters represent no valid day.
/// \param Day Day for this ADate
/// \param Month Month
/// \param Year Year to set
/// \throw std::invalid_argument in case of an invalid input
//----------------------------------------------------------------------------
ADate::ADate (char Day, char Month, int Year) throw (std::invalid_argument)
   : AYear (Year), day (Day), month (Month) {
   int status (checkIntegrity ());
   if (status)
      throw (std::invalid_argument (status == 2 ? "Month" : "Day"));
}

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
ADate::~ADate () {
}


//----------------------------------------------------------------------------
/// Assignment-operator
/// \param other Object to assign
/// \return ADate& Reference to self
//----------------------------------------------------------------------------
ADate& ADate::operator= (const ADate& other) {
   Check1 (!checkIntegrity ()); Check1 (!other.checkIntegrity ());
   TRACE5 ("ADate::operator=: " << other);

   day = other.day;
   month = other.month;
   AYear::operator= ((const AYear&) other);
   return *this;
}

//----------------------------------------------------------------------------
/// Assignment-operator from a const char-pointer. The date must be passed as
/// <tt>DDMMY[Y...]</tt>. If the buffer does not represent a valid day, an
/// exception is thrown.
/// \param pValue Character array holding date to assign
/// \return ADate& Reference to self
/// \throw std::invalid_argument if the characters don't represent a valid date
//----------------------------------------------------------------------------
ADate& ADate::operator= (const char* pValue) throw (std::invalid_argument) {
   Check1 (!checkIntegrity ());
   TRACE5 ("ADate::operator= (const char*): " << pValue);

   if (pValue && *pValue)
      assign (pValue, strlen (pValue));
   else
      undefine ();
   return *this;
}

//-----------------------------------------------------------------------------
/// Assignment-operator from a const char-pointer. The date must be passed
/// either in the local format or as DDMMY[...}. If the buffer does not
/// represent a valid date an excpetion is thrown.
/// \param pDate Character array specifying date to assign
/// \param len Length of the passed date
/// \throw std::invalid_argument if the parameter does not represent a
///     valid date
//-----------------------------------------------------------------------------
void ADate::assign (const char* pDate, unsigned int len) {
   TRACE5 ("ADate::assign (const char*, unsigned int): " << pDate << " ("
	   << len << ')');
   if (!(len && pDate && *pDate)) {
      undefine ();
      return;
   }

#ifdef HAVE_STRPTIME
   struct tm result;
   memset (&result, '\0', sizeof (result));

   const char* fail (NULL);
   switch (len) {
   case 12:
   case 11:
   case 10:
   case 9:
      fail = strptime (pDate, "%x", &result);
      break;
   case 8:
   case 7:
      fail = strptime (pDate, "%d %m %Y", &result);
      break;
   case 6:
   case 5:
      fail = strptime (pDate, "%d %m %y", &result);
      break;
   default:
      fail = NULL;
   } // endswitch
   operator= (result);
   if (!fail || (*fail && !isspace (*fail)) || checkIntegrity ()) {
      undefine ();
      if (!fail)
	 fail = pDate;
      TRACE9 ("ADate::assign (const char*, unsigned int) - Failed: " << fail);
      std::string error (_("Invalid date: %1"));
      error.replace (error.find ("%1"), 2, 1, char ((fail - pDate) + '0'));
      throw std::invalid_argument (error);
   }
#else
   day = month = 1;
   int read (0);
   unsigned int _day, _year, _month;

   switch (len) {
   case 12:
   case 11:
   case 10: {
   case 9:
      ADate tmp (22, 11, 2000);
      std::string format (tmp.toString ());
      size_t posY (format.find ("2000")); Check3 (posY != std::string::npos);
      size_t posM (format.find ("11")); Check3 (posM != std::string::npos);
      size_t posD (format.find ("22")); Check3 (posD != std::string::npos);

      format.replace (posD, 2, "%2u");
      format.replace (posM, 2, "%2u");
      format.replace (posY, 2, "%2u");

      read = ((posY < posM)
	      ? ((posD < posY)
		 ? sscanf (pDate, format.c_str (), &_day, &_year, &_month)
		 : ((posM < posY)
		    ? sscanf (pDate, format.c_str (), &_year, &_day, &_month)
		    : sscanf (pDate, format.c_str (), &_year, &_month, &_day)))
	      : ((posD < posM)
		 ? sscanf (pDate, format.c_str (), &_day, &_month, &_year)
		 : ((posD < posY)
		    ? sscanf (pDate, format.c_str (), &_month, &_day, &_year)
		    : sscanf (pDate, format.c_str (), &_month, &_year, &_day))));
      if (read != 3)
	 read = -1;
      break; }

   case 8:
   case 7:
   case 6:
   case 5:
      read = sscanf (pDate, "%2u%2u%d", &_day, &_month, &_year);
      if (read != 3)
	 read = -1;
      break;
   default:
      read = -1;
   } // endswitch
   TRACE5 ("ADate::assign (const char*, unsigned int) - Read: " << read);

   if ((read == -1) || checkIntegrity ()) {
      undefine ();
      TRACE9 ("ADate::assign (const char*, unsigned int) - Failed: " << pDate);
      std::string error (_("Invalid date: %1"));
      error.replace (error.find ("%1"), 2, 1, '0');
   }
   else {
      day = _day;
      month = _month;
      year = _year;
      setDefined ();
   }
#endif
   return;
}

//----------------------------------------------------------------------------
/// Assignment operator from a broken down time structure.
/// \param date Structure holding the date to assign
/// \return ADate& Reference to self
//----------------------------------------------------------------------------
ADate& ADate::operator= (const struct tm& date) {
   setDefined ();
   year = date.tm_year + 1900;
   month = (unsigned char)(date.tm_mon + 1);
   setDay ((unsigned char)date.tm_mday); Check3 (!checkIntegrity ());
   return *this;
}

//----------------------------------------------------------------------------
/// Converts the date into a string, in the format <code>DDMMYYYY[...]</code>;
/// each entry is filled up with zeros.
/// \return std::string String-representation of ADate
//----------------------------------------------------------------------------
std::string ADate::toUnformattedString () const {
   char buffer[20] = "";

   if (isDefined ())
      snprintf (buffer, sizeof (buffer), "%02d%02d%d", (unsigned)day, (unsigned)month, year);
   return std::string (buffer);
}

//----------------------------------------------------------------------------
/// Converts the date into a string, in the format specified by the current
/// locale
/// \return std::string String-representation of ADate
/// \remarks Only dates valid for <tt>struct tm</tt> can be printed (e.g. dates
///     after 1900)
//----------------------------------------------------------------------------
std::string ADate::toString () const {
   return toString ("%x");
}

//----------------------------------------------------------------------------
//// Converts the date into a string, in the specified format. The parameter
///  format can be any value accepted by the \c strftime
//            library-routine.
/// \return std::string String-representation of ADate
/// \remarks Only dates valid for <tt>struct tm</tt> can be printed (e.g. dates
///     after 1900)
//----------------------------------------------------------------------------
std::string ADate::toString (const char* format) const {
   Check1 (format);
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

//----------------------------------------------------------------------------
/// Reads a date as DDMMY[Y...] from a stream. If the input is not valid, an
/// exception is thrown.
/// \param in:Stream to parse
/// \throw std::invalid_argument in case of an invalid input
//----------------------------------------------------------------------------
void ADate::readFromStream (std::istream& in) throw (std::invalid_argument) {
   if (in.eof ()) {
      undefine ();
      return;
   }

   char buffer[40];
   char* pb = buffer;
   in >> *pb;
   while (!in.eof () && !isspace (*pb)
	  && ((unsigned int)(pb - buffer) < (sizeof (buffer) - 1)))
      in.get (*++pb);
   in.unget ();
   *pb = '\0';

   operator= (buffer);
}

//----------------------------------------------------------------------------
/// Adds another date-value to the date-object. An undefined date is treated
/// as <tt>0.0.0</tt>; so only if both objects are undefined, the result is
/// (remains) undefined. Overflows (of day or month) are corrected.
/// \param rhs Value to add
/// \return ADate& Self
//----------------------------------------------------------------------------
ADate& ADate::operator+= (const ADate& rhs) {
   Check1 (!checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (isDefined ()) {
         day += rhs.day;
         month += rhs.month;
         year += rhs.year;

         maxAdapt ();
      }
      else
         operator= (rhs);

      Check3 (!checkIntegrity ());
   }
   return *this;
}

//----------------------------------------------------------------------------
/// Substracts another date-value from the date-object. An undefined date
/// is treated as <tt>0.0.0</tt>; the result is (remains) undefined if both
///  objects are undefined. Underflows (of day or month) are corrected.
/// \param rhs Value to substract
/// \return ADate& Self
//----------------------------------------------------------------------------
ADate& ADate::operator-= (const ADate& rhs) {
   Check1 (!checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   if (rhs.isDefined ()) {
      if (isDefined ()) {
         day -= rhs.day;
         month -= rhs.month;
         year -= rhs.year;
      }
      else {
         setDefined ();
         day = -rhs.day;
         month = -rhs.month;
         year = -rhs.year;
      } // endif this not defined

      minAdapt ();
      Check3 (!checkIntegrity ());
   }
   return *this;
}

//----------------------------------------------------------------------------
/// If this is not undefined, the passed values are added (with attention to
/// overflows). The result is returned.
/// \param Day Day to add
/// \param Month Month to add
/// \param Year Year to add
/// \return ADate& Self
/// \remarks In counterpart to the mathematic operators (operator+() and
///    operator-()) this method does not change the object if it is undefined!
//----------------------------------------------------------------------------
ADate& ADate::add (signed char Day, signed char Month, int Year) {
   TRACE7 ("ADate::add: " << toString () << " + " << (int)Day << '.'
	   << (int)Month << '.' << Year);
   Check1 (!checkIntegrity ());

   if (isDefined ()) {
      Year += Month / 12;
      Month %= 12;
      month += Month;
      year += Year;
      maxAdapt ();

      signed char maxDay;
      while (maxDay = maxDayOf (), Day > maxDay) {
         Day -= maxDay;
         ++month;
	 maxAdapt ();
      } // end-while passed days bigger than one month
      day += Day;
      maxAdapt ();

      Check3 (!checkIntegrity ());
   }
   return *this;
}

//----------------------------------------------------------------------------
/// If this is defined, the passed values are substracted (with attention to
/// underflows). The result is returned.
/// \param Day Day to substract
/// \param Month Month to substract
/// \param Year Year to substract
/// \return ADate& Self
/// \remarks In counterpart to the mathematic operators (operator+() and
///    operator-()) this method does not change the object if it is undefined!
//----------------------------------------------------------------------------
ADate& ADate::sub (signed char Day, signed char Month, int Year) {
   TRACE7 ("ADate::sub: " << toString () << " - " << (int)Day << '.'
	   << (int)Month << '.' << Year);
   Check1 (!checkIntegrity ());

   if (isDefined ()) {
      Year += Month / 12;                      // Correct month and year first
      Month %= 12;
      month -= Month;
      year -= Year;
      minAdapt ();

      signed char maxDay;
      while (maxDay = maxDayOf (month > 1 ? month - 1 : 12, year),
	     Day > maxDay) {
         Day -= maxDay;
         --month;
	 minAdapt ();
      } // end-while passed days bigger than one month
      if (Day) {
	 day -= Day;
	 minAdapt ();
      }

      Check3 (!checkIntegrity ());
      Check3 (isDefined ());
   }
   return *this;
}

//----------------------------------------------------------------------------
/// Returns the (approximated) difference in days between two dates.
/// "Younger dates" (closer to the past) are considered bigger than "older
/// dates" (further in the past; that means the numeric value of the date is
/// compared (e.g.: "yesterday" < "today" < "tomorrow").
///
///  If both dates are undefined, those difference is "0", if only this is
///  undefined the result is MINLONG; if other is undefined MAXLONG is
///  returned (-> undefined dates are considered as (very) old).
///
/// \param other Object to compare
/// \return long >0 if this "younger" other; 0 if this == other; <0 else
//----------------------------------------------------------------------------
long ADate::compare (const ADate& other) const {
   Check1 (!checkIntegrity ()); Check1 (!other.checkIntegrity ());

   // Both sides are defined -> return (approximated) difference
   if (isDefined ()) {
      if (other.isDefined ()) {
         TRACE5 ("ADate::compare -> " << ((year - other.year) * 365
                                          + ((month - other.month) * 31)
                                          + day - other.day));

         return ((year - other.year) * 365 + ((month - other.month) * 31)
                 + day - other.day);
      }
      else
         return 1;                    // this defined, other not: Return bigger
   }
   else
      return other.isDefined () ? -1 : 0;
}

//----------------------------------------------------------------------------
/// Adds two date-values and returns the result. An undefined date is
/// treated as "0.0.0"; the result is (remains) undefined if both objects are
/// undefined. Overflows (of day or month) are corrected.
/// \param lhs Left-hand-side of addition
/// \param rhs Right-hand-side of addition
/// \return ADate Result of additon
//----------------------------------------------------------------------------
ADate operator+ (const ADate& lhs, const ADate& rhs) {
   Check1 (!lhs.checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   ADate result (lhs);
   result += rhs;
   return result;
}

//----------------------------------------------------------------------------
/// Substracts two date-values and returns the result. An undefined date is
/// treated as "0.0.0"; the result is (remains) undefined if both objects
/// are undefined. Overflows (of day or month) are corrected.
/// \param lhs Left-hand-side of substraction
/// \param rhs Right-hand-side of substraction
/// \return ADate Result of substraction
//----------------------------------------------------------------------------
ADate operator- (const ADate& lhs, const ADate& rhs) {
   Check1 (!lhs.checkIntegrity ()); Check1 (!rhs.checkIntegrity ());

   ADate result (lhs);
   result -= rhs;
   return result;
}

//----------------------------------------------------------------------------
/// Checks if this object represents a valid date.
/// \return int Status; 0: OK
/// \remarks Even undefined dates must have valid values!
//----------------------------------------------------------------------------
int ADate::checkIntegrity () const {
   TRACE9 ("ADate::checkIntegrity () const - " << (int)day << '.' << (int)month
           << '.' << year);
   return isDefined () ? ((month < 1) || (month > 12)) ? 2 : (day > maxDayOf ()) : 0;
}

//----------------------------------------------------------------------------
/// Returns the maximal day for the the passed month/year-combination
/// \param month Month to check
/// \param year Year to check
/// \return char Maximal day for the passed parameters [28 - 31]
//----------------------------------------------------------------------------
char ADate::maxDayOf (char month, int year) {
   Check1 ((month > 0) && (month < 13));

   if (month == (unsigned char)2)              // Special-handling of february
      return (unsigned char)(isLeapYear (year) ? 29 : 28);

   if (month > (unsigned char)7)    // Adapt month after july for easier calc.
      --month;
   return (unsigned char)(month & 1 ? 31 : 30);
}

//----------------------------------------------------------------------------
/// Corrects the object after an (possible) underflows. If the object is
/// integer after the operation, true is returned.
/// \return bool Flag, if object is integer
//----------------------------------------------------------------------------
bool ADate::minAdapt () {
   TRACE7 ("ADate::minAdapt: " << toString ());
   if (((unsigned char)(month - 1)) > 11) {
      int mon (255 - (unsigned char)(month - 1));
      year -= (mon / 12) + 1;
      month = 12 - (mon % 12);
   }

   TRACE9 ("ADate::minAdapt (month adapted (1)): " << toString ());

   if ((signed char)day > maxDayOf ())
      day = maxDayOf ();
   else
      while (((signed char)day) < 1) {               // Adapt date if underflow
         --month;
         if (month < 1) {
            month = 12;
            --year;
            TRACE9 ("ADate::minAdapt (month adapted (2)): " << toString ());
         }
         day += maxDayOf ();
         TRACE9 ("ADate::minAdapt (day adapted (1)): " << toString ());
      } // endif day invalid

   return !ADate::checkIntegrity ();      // Can only ensure proper ADate-part
}

//----------------------------------------------------------------------------
/// Corrects the object after an (possible) overflows. If the object is
/// integer after the operation, true is returned.
/// \return bool Flag, if object is integer
//----------------------------------------------------------------------------
bool ADate::maxAdapt () {
   TRACE7 ("ADate::maxAdapt: " << toString ());
   if (((unsigned char)(month - 1)) > 11) {
      year += month / 12;
      month = month % 12;
   }

   TRACE9 ("ADate::maxAdapt (month dapted (1)): " << toString ());

   unsigned char maxDay (maxDayOf ());               // Adapt date if overflow
   while (day > maxDay) {
      day -= maxDay;
      ++month;
   }

   if (month > (unsigned char)12) {                 // Adapt month if overflow
      month -= (unsigned char)12;     // Assuming calc. was with correct month
      ++year;
   }
   return !ADate::checkIntegrity ();      // Can only ensure proper ADate-part
}

//----------------------------------------------------------------------------
/// Sets the day to the passed value. If the date is not valid, it is set to
/// undefined.
/// \param Day Day to set
//----------------------------------------------------------------------------
void ADate::setDay (char Day) throw (std::invalid_argument) {
   day = Day;

   if (checkIntegrity ()) {
      day = 1;
      throw (std::invalid_argument ("ADate::setDay"));
   }
   else
      setDefined ();
}

//----------------------------------------------------------------------------
/// Sets the month to the passed value. If the date is not valid, it is set to
/// undefined.
/// \param Month Month to set
//----------------------------------------------------------------------------
void ADate::setMonth (char Month) throw (std::invalid_argument) {
   month = Month;

   if (checkIntegrity ()) {
      throw (std::invalid_argument ("ADate::setMonth"));
      undefine ();
   }
   else
      setDefined ();
}

//----------------------------------------------------------------------------
/// Converting to a struct tm.
/// \return struct tm Date in struct tm-format; the time-part is set to zeros
/// \remarks It is not checked if the date is in the right range for a
//        <tt>struct tm</tt> (after 1900 and before 2039)
//----------------------------------------------------------------------------
struct tm ADate::toStructTM () const {
   struct tm result;
   memset (&result, '\0', sizeof (result));
   if (isDefined ()) {
      result.tm_mday = day;
      result.tm_mon = month - 1;
      result.tm_year = year - 1900;
   }
   return result;
}

}
