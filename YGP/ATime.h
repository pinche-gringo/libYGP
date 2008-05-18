#ifndef YGP_ATIME_H
#define YGP_ATIME_H

//$Id: ATime.h,v 1.27 2008/05/18 13:21:27 markus Rel $

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


#include <ctime>

#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#endif

#include <string>
#include <iosfwd>

#include <YGP/AttrVal.h>


// Forward declarations
namespace std {
   class invalid_argument;
}


namespace YGP {

/**Class for time attributes. As every AttributValue is supports undefined
   values.
*/
class ATime : public AttributValue {
 public:
   ATime ()                /// Default constructor; creates an undefined object
       : AttributValue (), hour (0), min_ (0), sec (0), mode (MODE_LOCALE) { }
   ATime (bool now);
   ATime (const ATime& other)     /// Copy constructor from another time object
       : AttributValue ((const AttributValue&)other)
       , hour (other.hour), min_ (other.min_), sec (other.sec), mode (other.mode) { }
   ATime (char Hour, char minute, char second) throw (std::invalid_argument);
   ATime (const char* pTime) throw (std::invalid_argument)
      : AttributValue (), hour (0), min_ (0), sec (0), mode (MODE_LOCALE) {
      operator= (pTime); }           ///< Constructor from a text (unformatted)
   ATime (const std::string& time) throw (std::invalid_argument)
      : AttributValue (), hour (0), min_ (0), sec (0), mode (MODE_LOCALE) {
      operator= (time); }            ///< Constructor from a text (unformatted)
   ATime (const struct tm& tm) : AttributValue (), hour (0), min_ (0), sec (0), mode (MODE_LOCALE) {
      operator= (tm); }                  ///< Constructor from broken down time
   /// Construct from system time
   ATime (const time_t& time, bool local = true)
      : AttributValue (), hour (0), min_ (0), sec (0), mode (MODE_LOCALE) {
      local ?  operator= (time) : operator= (*localtime (&time)); }
   virtual ~ATime ();

   /// \name Assignment methods
   //@{
   /// Assignment operator from an (unformatted) text
   ATime& operator= (const std::string& time) throw (std::invalid_argument) {
      assign (time.c_str (), time.length ());
      return *this; }
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

   void assign (const char* pTime, unsigned int len);
   //@}

   /// Defining the object; setting it to a default value (of <tt>0:00:00</tt>)
   virtual void define () { setDefined (); hour = min_ = sec = 0; }
   void setHour (char Hour) throw (std::invalid_argument);
   void setMinute (char minute) throw (std::invalid_argument);
   void setSecond (char second) throw (std::invalid_argument);

   // Query-functions
   char getHour () const { return hour; }                 ///< Returns the hour
   char getMinute () const { return min_; }             ///< Returns the minute
   char getSecond () const { return sec; }              ///< Returns the second

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
   bool operator== (const ATime& other) const { return !compare (other); }
   bool operator!= (const ATime& other) const { /// Compares if two objects are not equal
      return compare (other) != 0; }
   /// Checks if one object is "younger" than the other. See compare() for details
   bool operator<  (const ATime& other) const { return compare (other) < 0; }
   /// Checks if one object is "older" than the other. See compare() for details
   bool operator>  (const ATime& other) const { return compare (other) > 0; }
   /// Checks if one object is "younger"  than or equal to the other. See compare() for details
   bool operator<= (const ATime& other) const { return compare (other) <= 0; }
   /// Checks if one object is "older" than or equal to the other. See compare() for details
   bool operator>= (const ATime& other) const { return compare (other) >= 0; }
   long compare (const ATime& other) const;

   // Utility-functions
   virtual int checkIntegrity () const;

   enum Mode { MODE_LOCALE = 0, MODE_HHMM, MODE_MMSS };
   void setMode (enum Mode newMode) { mode = newMode; }
   enum Mode getMode () const { return mode; }

 protected:
   virtual bool maxAdapt ();
   virtual bool minAdapt ();

 private:
   unsigned char hour;
   unsigned char min_;
   unsigned char sec;

   enum Mode mode;

   static const char* MODES[];
};

}

#endif
