#ifndef YGP_ADATE_H
#define YGP_ADATE_H

//$Id: ADate.h,v 1.34 2008/05/18 13:21:27 markus Rel $

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

#include <YGP/Check.h>
#include <YGP/AYear.h>


// Forward declarations
namespace std {
   class invalid_argument;
}


namespace YGP {

/**Class for date attributes. As every AYear is supports undefined
   values.

   \note It's possible to calculate with ADate objects. But beware, that the
      results are not always intuitive (e.g. what's today - today?
      Mathematically it's the 0.0.0, which is not a valid date and therefore
      changed to the 31.12.-1; a result which might not be expected).

   \invariant Objects hold always a valid date; even if they are not defined.
*/
class ADate : public AYear {
 public:
   ADate ()                /// Default constructor; creates an undefined object
      : AYear (), day (1), month (1)  { year = 1900; }
   ADate (bool now);
   ADate (const ADate& other)     /// Copy constructor from another date object
      : AYear ((const AYear&)other) , day (other.day), month (other.month) { }
   ADate (char Day, char Month, int Year) throw (std::invalid_argument);
   ADate (const char* pDate) throw (std::invalid_argument) : AYear (), day (1), month (1) {
       operator= (pDate); }          ///< Constructor from a text (unformatted)
   ADate (const std::string& date) throw (std::invalid_argument) : AYear (), day (1), month (1) {
       operator= (date); }           ///< Constructor from a text (unformatted)
   ADate (const struct tm& tm) : AYear (), day (1), month (1) {
       operator= (tm); }                 ///< Constructor from broken down time
   ADate (const time_t& date) : AYear (), day (1), month (1) {
       operator= (date); }                      ///< Construct from system time
   virtual ~ADate ();

   /// \name Assignment methods
   //@{
   /// Assignment operator from an (unformatted) text
   ADate& operator= (const std::string& date) throw (std::invalid_argument) {
      return operator= (date.c_str ()); }
   ADate& operator= (const struct tm& date);
   ADate& operator= (const time_t& date) {  /// Assignment operator from system time
      return operator= (*gmtime (&date)); }
   ADate& operator= (const ADate& other);
   ADate& operator= (const char* pValue) throw (std::invalid_argument);

   void assign (const char* pTime, unsigned int len);

   virtual void readFromStream (std::istream& in) throw (std::invalid_argument);
   //@}

   /// Defining the object; setting it to a default value (of <tt>1/1/1900</tt>)
   virtual void define () {
      setDefined (); day = month = (unsigned char)1; year = 1900; }
   void setDay (char Day) throw (std::invalid_argument);
   void setMonth (char Month) throw (std::invalid_argument);
   /// Sets the year of the date object
   void setYear (int Year) { setDefined (); year = Year; }

   /// \name Query methods
   //@{
   char getDay () const { return day; }             ///< Returns the actual day
   char getMonth () const { return month; }       ///< Returns the actual month
   int getYear () const { return year; }           ///< Returns the actual year
   //@}

   static ADate today () { return ADate (true); } ///< Returns the current date

   /// \name Convertion
   //@{
   virtual std::string toUnformattedString () const;
   virtual std::string toString () const;
   virtual std::string toString (const char* format) const;

   /// Converting into a system time
   time_t toSysTime () const {
      struct tm result (toStructTM ()); return mktime (&result); }
   virtual struct tm toStructTM () const;
   //@}

   /// \name Calculation
   //@{
   ADate& operator+= (const ADate& rhs);
   ADate& operator-= (const ADate& rhs);

   friend ADate operator+ (const ADate& lhs, const ADate& rhs);
   friend ADate operator- (const ADate& lhs, const ADate& rhs);

   ADate& add (signed char Day, signed char Month = 0, int Year = 0);
   ADate& sub (signed char Day, signed char month = 0, int Year = 0);
   //@}

   /// \name Comparison
   //@{
   bool operator== (const ADate& other) const { /// Compares two objects for equalnesss
      return !compare (other); }
   bool operator!= (const ADate& other) const { /// Compares if two objects are not equal
      return compare (other) != 0; }
   /// Checks if one object is "younger" than the other. See compare() for details
   bool operator<  (const ADate& other) const { return compare (other) < 0; }
   /// Checks if one object is "older" than the other. See compare() for details
   bool operator>  (const ADate& other) const { return compare (other) > 0; }
   /// Checks if one object is "younger"  than or equal to the other. See compare() for details
   bool operator<= (const ADate& other) const {
      return compare (other) <= 0; }
   /// Checks if one object is "older" than or equal to the other. See compare() for details
   bool operator>= (const ADate& other) const {
      return compare (other) >= 0; }
   long compare (const ADate& other) const;
   //@}

   /// \name Usefull utility-functions
   //@{
   /// Returns the maximal day of the specified \c month (in the passed \c year)
   char maxDayOf () const { return maxDayOf (month, year); }
   static char maxDayOf (char month, int year);

   virtual int checkIntegrity () const;
   //@}

 protected:
   void incDay () { ++day; }                             ///< Increases the day
   void decDay () { --day; }                             ///< Decreases the day

   virtual bool minAdapt ();
   virtual bool maxAdapt ();

 private:
   unsigned char day;
   unsigned char month;
};

}

#endif
