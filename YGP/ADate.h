#ifndef ADATE_H
#define ADATE_H

//$Id: ADate.h,v 1.6 1999/10/15 21:34:26 Markus Rel $

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

#include <string>

#include "AttrVal.h"


// Forward declarations
class istream;


// Class for date attributes. As every AttributValue is supports undefined
// values.
class ADate : public AttributValue {
 public:
   ADate () : AttributValue (), day (1), month (1), year (1900) { }
   ADate (bool now);
   ADate (const ADate& other) : AttributValue ((const AttributValue&)other)
      , day (other.day), month (other.month), year (other.year) { }
   ADate (char Day, char Month, unsigned int Year);
   ADate (const char* pDate) { operator= (pDate); }
   ADate (const std::string& date) { operator= (date); }
   ADate (const struct tm& tm) { operator= (tm); }
   ADate (const time_t date) { operator= (date); }
   virtual ~ADate ();

   // Set-functions
   ADate& operator= (const ADate& other);
   ADate& operator= (const char* pDate);
   ADate& operator= (istream& stream);
   ADate& operator= (const std::string& date) { return operator= (date.c_str ()); }
   ADate& operator= (const struct tm& tm) { year = tm.tm_year + 1900;
      month = tm.tm_mon + 1; setDay (tm.tm_mday); }
   ADate& operator= (const time_t date) { operator= (*localtime (&date)); }

   virtual void readFromStream (istream& in);

   virtual void define () { AttributValue::define (); day = month = 1; year = 1900; }
   void setDay (char Day);
   void setMonth (char Month);
   void setYear (unsigned int Year) { AttributValue::define (); year = Year; }

   // Query-functions
   char getDay () const { return day; }
   char getMonth () const { return month; }
   unsigned int getYear () const { return year; }

   static ADate ADate::today () { return ADate (true); }

   // Conversion
   virtual std::string toString () const;
   virtual std::string toString (const char* format) const;

   virtual struct tm toStructTM () const;
   time_t toSysTime () const {
      struct tm result (toStructTM ()); return mktime (&result); }

   // Calculation
   ADate& operator+= (const ADate& rhs);
   ADate& operator-= (const ADate& rhs);

   friend ADate operator+ (const ADate& lhs, const ADate& rhs);
   friend ADate operator- (const ADate& lhs, const ADate& rhs);

   ADate& add (char Day, char Month = 0, unsigned int Year = 0);
   ADate& sub (char Day, char month = 0, unsigned int Year = 0);

   // Comparison
   bool operator== (const ADate& other) { return !compare (other); }
   bool operator!= (const ADate& other) { return compare (other); }
   bool operator<  (const ADate& other) { return compare (other) < 0; }
   bool operator>  (const ADate& other) { return compare (other) > 0; }
   bool operator<= (const ADate& other) { return compare (other) <= 0; }
   bool operator>= (const ADate& other) { return compare (other) >= 0; }
   long compare (const ADate& other);

   // Usefull utility-functions
   char maxDayOf () const { return maxDayOf (month, year); }
   static char maxDayOf (char month, unsigned int year);
   bool isLeapYear () const { return isLeapYear (year); }
   static bool isLeapYear (unsigned int year);

   virtual int checkIntegrity () const;

 protected:
   void incDay () { ++day; }
   void decDay () { --day; }

   virtual bool minAdapt ();
   virtual bool maxAdapt ();

 private:
   unsigned char day;
   unsigned char month;
   unsigned int year;
};

#endif
