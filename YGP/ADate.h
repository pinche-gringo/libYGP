#ifndef ADATE_H
#define ADATE_H

//$Id: ADate.h,v 1.2 1999/10/12 21:39:14 Markus Exp $

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
#include <assert.h>

#include <string>

#include "AttrVal.h"


// Class for date attributes. As every AttributValue is supports undefined
// values.
class ADate : public AttributValue {
 public:
   ADate () : AttributValue () { }
   ADate (bool now);
   ADate (const ADate& other) : AttributValue ((const AttributValue&)other)
      , day (other.day), month (other.month), year (other.year) { }
   ADate (char Day, char Month, unsigned int Year);
   virtual ~ADate ();

   ADate& operator= (const ADate& other);
   ADate& operator= (const char* pDate);

   void setDay (char Day) { assert ((Day > 0) && (Day <= maxDayOf ()));
      AttributValue::define (); day = Day; }
   void setMonth (char Month) { assert ((Month > 0) && (Month < 12));
      AttributValue::define (); month = Month; }
   void setYear (unsigned int Year) { AttributValue::define (); year = Year; }

   char getDay () { return day; }
   char getMonth () { return month; }
   unsigned int getYear () { return year; }

   static ADate ADate::today () { return ADate (true); }

   virtual void define () { AttributValue::define (); day = month = 1; year = 1900; }
   virtual std::string toString () const;
   virtual std::string toString (const char* format) const;

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

   char maxDayOf () const { return maxDayOf (month, year); }
   static char maxDayOf (char month, unsigned int year);
   bool isLeapYear () const { return isLeapYear (year); }
   static bool isLeapYear (unsigned int year);

   int checkIntegrity () const;

 private: 
   char day;
   char month;
   unsigned int year;

   void minAdapt ();
   void maxAdapt ();
};

#endif
