#ifndef ATSTAMP_H
#define ATSTAMP_H

//$Id: ATStamp.h,v 1.1 1999/10/14 22:23:32 Markus Exp $

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

#include "ADate.h"


// Class for timestamp attributes. As every AttributValue is supports
// undefined values.
class ATimestamp : public ADate {
 public:
   ATimestamp ();
   ATimestamp (bool now);
   ATimestamp (const ATimestamp& other) : ADate ((const ADate&)other)
      , hour (other.hour), min (other.min), sec (other.sec) { }
   ATimestamp (char Day, char Month, unsigned int Year, char Hour,
               char minute, char second);
   ATimestamp (const char* pStamp) { operator= (pStamp); }
   ATimestamp (const std::string& stamp) { operator= (stamp); }
   ATimestamp (const struct tm& tm) { operator= (tm); }
   ATimestamp (const time_t stamp) { operator= (stamp); }
   virtual ~ATimestamp ();

   // Set-functions
   ATimestamp& operator= (const ATimestamp& other);
   ATimestamp& operator= (const char* pStamp) { return (ATimestamp&)ADate::operator= (pStamp); }
   ATimestamp& operator= (const std::string& stamp) { return operator= (stamp.c_str ()); }
   ATimestamp& operator= (const struct tm& tm);
   ATimestamp& operator= (const time_t date) { operator= (*localtime (&date)); }

   virtual void readFromStream (istream& in);

   virtual void define () { ADate::define (); hour = min = sec = 0; }
   void setHour (char Hour);
   void setMinute (char minute);
   void setSecond (char second);

   // Query-functions
   char getHour () const { return hour; }
   char getMinute () const { return min; }
   char getSecond () const { return sec; }

   static ATimestamp ATimestamp::now () { return ATimestamp (true); }

   // Convertion
   virtual std::string toString () const;
   virtual std::string toString (const char* format) const;

   virtual struct tm toStructTM () const;
   time_t    toSysTime () const {
      struct tm result (toStructTM ()); return mktime (&result); }

   // Calculation
   ATimestamp& operator+= (const ATimestamp& rhs);
   ATimestamp& operator-= (const ATimestamp& rhs);

   friend ATimestamp operator+ (const ATimestamp& lhs, const ATimestamp& rhs);
   friend ATimestamp operator- (const ATimestamp& lhs, const ATimestamp& rhs);

   ATimestamp& add (char Day, char Month = 0, unsigned int Year = 0,
                    char Hour = 0, char minute = 0,char second = 0);
   ATimestamp& sub (char Day, char month = 0, unsigned int Year = 0,
                    char Hour = 0, char minute = 0,char second = 0);

   // Comparison
   bool operator== (const ATimestamp& other) { return !compare (other); }
   bool operator!= (const ATimestamp& other) { return compare (other); }
   bool operator<  (const ATimestamp& other) { return compare (other) < 0; }
   bool operator>  (const ATimestamp& other) { return compare (other) > 0; }
   bool operator<= (const ATimestamp& other) { return compare (other) <= 0; }
   bool operator>= (const ATimestamp& other) { return compare (other) >= 0; }
   long compare (const ATimestamp& other);

   // Usefull utility-functions
   int checkIntegrity () const;

 private: 
   char hour;
   char min;
   unsigned int sec;

   void minAdapt ();
   void maxAdapt ();
};

#endif
