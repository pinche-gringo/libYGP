#ifndef ATIME_H
#define ATIME_H

//$Id: ATime.h,v 1.7 2001/08/17 13:19:23 markus Exp $

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

#include <AttrVal.h>


// Forward declarations
class istream;
class invalid_argument;


// Class for date attributes. As every AttributValue is supports undefined
// values.
class ATime : public AttributValue {
 public:
   ATime () : AttributValue (), hour (0), min_ (0), sec (0) { }
   ATime (bool now);
   ATime (const ATime& other) : AttributValue ((const AttributValue&)other)
      , hour (other.hour), sec (other.sec), min_ (other.min_) { }
   ATime (char Hour, char minute, char second);
   ATime (const char* pTime) throw (invalid_argument) { operator= (pTime); }
   ATime (const std::string& time) throw (invalid_argument) { operator= (time); }
   ATime (const struct tm& tm) { operator= (tm); }
   ATime (const time_t time) { operator= (time); }
   virtual ~ATime ();

   // Set-functions
   ATime& operator= (const ATime& other);
   ATime& operator= (const char* pTime) throw (invalid_argument);
   ATime& operator= (const std::string& time) throw (invalid_argument) {
      return operator= (time.c_str ()); }
   ATime& operator= (const struct tm& tm) { hour = (unsigned char)tm.tm_hour;
      min_ = (unsigned char)tm.tm_min; setSecond ((unsigned char)tm.tm_sec); return *this; }
   ATime& operator= (const time_t time) { return operator= (*localtime (&time)); }

   virtual void readFromStream (istream& in) throw (invalid_argument);

   virtual void define () { AttributValue::define (); hour = min_ = sec = 0; }
   void setHour (char Hour);
   void setMinute (char minute);
   void setSecond (char second);

   // Query-functions
   char getHour () const { return hour; }
   char getMinute () const { return min_; }
   char getSecond () const { return sec; }

   static ATime now () { return ATime (true); }

   // Conversion
   virtual std::string toUnformatedString () const;
   virtual std::string toString () const;
   virtual std::string toString (const char* format) const;

   virtual struct tm toStructTM () const;
   time_t toSysTime () const {
      struct tm result (toStructTM ()); return mktime (&result); }

   // Calculation
   ATime& operator+= (const ATime& rhs);
   ATime& operator-= (const ATime& rhs);

   friend ATime operator+ (const ATime& lhs, const ATime& rhs);
   friend ATime operator- (const ATime& lhs, const ATime& rhs);

   ATime& add (char Hour, char minute = 0, char second = 0);
   ATime& sub (char Hour, char minute = 0, char second = 0);

   // Comparison
   bool operator== (const ATime& other) { return !compare (other); }
   bool operator!= (const ATime& other) { return compare (other) != 0; }
   bool operator<  (const ATime& other) { return compare (other) < 0; }
   bool operator>  (const ATime& other) { return compare (other) > 0; }
   bool operator<= (const ATime& other) { return compare (other) <= 0; }
   bool operator>= (const ATime& other) { return compare (other) >= 0; }
   long compare (const ATime& other);

   // Utility-functions
   virtual int checkIntegrity () const;

 protected:
   virtual bool maxAdapt ();
   virtual bool minAdapt ();

 private:
   unsigned char hour;
   unsigned char min_;
   unsigned char sec;
};

#endif
