#ifndef ATIME_H
#define ATIME_H

//$Id: ATime.h,v 1.18 2003/06/15 05:19:12 markus Rel $

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


#include <ctime>

#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#endif

#include <string>
#include <iosfwd>

#include <AttrVal.h>


// Forward declarations
namespace std {
   class invalid_argument;
}


/**Class for time attributes. As every AttributValue is supports undefined
   values.
*/
class ATime : public AttributValue {
 public:
   ATime ()                /// Default constructor; creates an undefined object
       : AttributValue (), hour (0), min_ (0), sec (0) { }
   ATime (bool now);
   ATime (const ATime& other)     /// Copy constructor from another time object
       : AttributValue ((const AttributValue&)other)
       , hour (other.hour), min_ (other.min_), sec (other.sec) { }
   ATime (char Hour, char minute, char second) throw (std::invalid_argument);
   ATime (const char* pTime) throw (std::invalid_argument) {
      operator= (pTime); }           ///< Constructor from a text (unformatted)
   ATime (const std::string& time) throw (std::invalid_argument) {
      operator= (time); }            ///< Constructor from a text (unformatted)
   ATime (const struct tm& tm) {
      operator= (tm); }                  ///< Constructor from broken down time
   /// Construct from system time
   ATime (const time_t& time, bool local = true) {
      local ?  operator= (time) : operator= (*localtime (&time)); }
   virtual ~ATime ();

   /// \name Assignment methods
   //@{
   /// Assignment operator from an (unformatted) text
   ATime& operator= (const std::string& time) throw (std::invalid_argument) {
      return operator= (time.c_str ()); }
   ATime& operator= (const char* pTime) throw (std::invalid_argument);
   ATime& operator= (const ATime& other);
   ATime& operator= (const struct tm& tm) { /// Assignment operator from broken down time
      setDefined (); hour = (unsigned char)tm.tm_hour;
      min_ = (unsigned char)tm.tm_min; setSecond ((unsigned char)tm.tm_sec);
      return *this; }
   ATime& operator= (const time_t& time) { /// Assignment operator from system time (local time)
      return operator= (*gmtime (&time)); }
   void setGMT (const time_t& time) { /// Assignment operator from system time (GMT)
      operator= (*localtime (&time)); }

   virtual void readFromStream (std::istream& in) throw (std::invalid_argument);
   //@}

   /// Defining the object; setting it to a default value (of <tt>0:0:00</tt>)
   virtual void define () { setDefined (); hour = min_ = sec = 0; }
   void setHour (char Hour) throw (std::invalid_argument);
   void setMinute (char minute) throw (std::invalid_argument);
   void setSecond (char second) throw (std::invalid_argument);

   // Query-functions
   char getHour () const { return hour; }
   char getMinute () const { return min_; }
   char getSecond () const { return sec; }

   static ATime now () { return ATime (true); }    ///< Returns the actual time

   /// \name Convertion
   //@{
   virtual std::string toUnformattedString () const;
   virtual std::string toString () const;
   virtual std::string toString (const char* format) const;

   virtual struct tm toStructTM () const;
   time_t toSysTime () const {                /// Converting into a system time
      struct tm result (toStructTM ()); return mktime (&result); }
   //@}

   /// \name Calculation
   //@{
   ATime& operator+= (const ATime& rhs);
   ATime& operator-= (const ATime& rhs);

   friend ATime operator+ (const ATime& lhs, const ATime& rhs);
   friend ATime operator- (const ATime& lhs, const ATime& rhs);

   ATime& add (char Hour, char minute = 0, char second = 0);
   ATime& sub (char Hour, char minute = 0, char second = 0);
   //@}

   /// \name Comparison
   //@{
   /// Compares two objects for equalnesss
   bool operator== (const ATime& other) { return !compare (other); }
   bool operator!= (const ATime& other) { /// Compares if two objects are not equal
      return compare (other) != 0; }
   /// Checks if one object is "younger" than the other. See compare() for details
   bool operator<  (const ATime& other) { return compare (other) < 0; }
   /// Checks if one object is "older" than the other. See compare() for details
   bool operator>  (const ATime& other) { return compare (other) > 0; }
   /// Checks if one object is "younger"  than or equal to the other. See compare() for details
   bool operator<= (const ATime& other) { return compare (other) <= 0; }
   /// Checks if one object is "older" than or equal to the other. See compare() for details
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
