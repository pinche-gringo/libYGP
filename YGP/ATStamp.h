#ifndef ATSTAMP_H
#define ATSTAMP_H

//$Id: ATStamp.h,v 1.16 2003/03/03 05:56:28 markus Exp $

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

#include <ADate.h>
#include <ATime.h>


// Forward declarations
namespace std {
   class invalid_argument;
}

// Class for timestamp attributes. As every AttributValue is supports
// undefined values.
class ATimestamp : virtual public ADate, virtual public ATime {
 public:
   ATimestamp ();
   ATimestamp (bool now);
   ATimestamp (const ATimestamp& other) : ADate ((const ADate&)other)
      , ATime ((const ATime&)other) { }
   ATimestamp (char Day, char Month, int Year, char Hour,
               char minute, char second) throw (std::invalid_argument);
   ATimestamp (const char* pStamp) throw (std::invalid_argument) { operator= (pStamp); }
   ATimestamp (const std::string& stamp) throw (std::invalid_argument) { operator= (stamp); }
   ATimestamp (const struct tm& tm) { operator= (tm); }
   ATimestamp (const time_t& stamp, bool local = true) {
      local ? operator= (stamp) : assignGMT (stamp); }
   virtual ~ATimestamp ();

   bool         isDefined () const { return ATime::isDefined ();
   }
   virtual void undefine () {
      ADate::undefine ();
      ATime::undefine ();
   }

   static ATimestamp now () { return ATimestamp (true); }

   // Set-functions
   ATimestamp& operator= (const ATimestamp& other);
   ATimestamp& operator= (const char* pStamp) throw (std::invalid_argument);
   ATimestamp& operator= (const std::string& stamp) throw (std::invalid_argument) {
      return operator= (stamp.c_str ()); }
   ATimestamp& operator= (const struct tm& tm);
   ATimestamp& operator= (const time_t& stamp) { return operator= (*gmtime (&stamp)); }

   ATimestamp& assignGMT (const time_t& stamp) { return operator= (*::localtime (&stamp)); }

   virtual void readFromStream (std::istream& in) throw (std::invalid_argument);

   virtual void define () { ADate::define (); ATime::define (); }

   // Convertion
   virtual std::string toUnformatedString () const;
   virtual std::string toString () const;
   virtual std::string toString (const char* format) const;

   struct tm toStructTM () const;
   time_t toGMTTime () const;
   time_t toLocalTime () const {
      struct tm result (toStructTM ()); return mktime (&result); }
   time_t toSysTime () const { return toLocalTime (); }

   // Calculation
   ATimestamp& operator+= (const ATimestamp& rhs);
   ATimestamp& operator-= (const ATimestamp& rhs);

   friend ATimestamp operator+ (const ATimestamp& lhs, const ATimestamp& rhs);
   friend ATimestamp operator- (const ATimestamp& lhs, const ATimestamp& rhs);

   ATimestamp& add (char Day, char Month = 0, int Year = 0,
                    char Hour = 0, char minute = 0,char second = 0);
   ATimestamp& sub (char Day, char month = 0, int Year = 0,
                    char Hour = 0, char minute = 0,char second = 0);

   // Comparison
   bool operator== (const ATimestamp& other) { return !compare (other); }
   bool operator!= (const ATimestamp& other) { return compare (other) != 0; }
   bool operator<  (const ATimestamp& other) { return compare (other) < 0; }
   bool operator>  (const ATimestamp& other) { return compare (other) > 0; }
   bool operator<= (const ATimestamp& other) { return compare (other) <= 0; }
   bool operator>= (const ATimestamp& other) { return compare (other) >= 0; }
   long compare (const ATimestamp& other);

   // Usefull utility-functions
   int checkIntegrity () const;

 protected:
   virtual bool maxAdapt ();
   virtual bool minAdapt ();
};

#endif
