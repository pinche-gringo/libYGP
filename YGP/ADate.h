#ifndef ADATE_H
#define ADATE_H

//$Id: ADate.h,v 1.28 2003/11/14 00:22:56 markus Exp $

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

#include <YGP/Check.h>
#include <YGP/AttrVal.h>


// Forward declarations
namespace std {
   class invalid_argument;
}
class istream;


/**Class for date attributes. As every AttributValue is supports undefined
   values.

   \note It's possible to calculate with ADate objects. But beware, that the
      results are not always intuitive (e.g. what's today - today?
      Mathematically it's the 0.0.0, which is not a valid date and therefore
      changed to the 31.12.-1; a result which might not be expected).

   \invariant Objects hold always a valid date; even if they are not defined.
*/
class ADate : public AttributValue {
 public:
   ADate ()                /// Default constructor; creates an undefined object
       : AttributValue (), day (1), month (1), year (1900) { }
   ADate (bool now);
   ADate (const ADate& other)     /// Copy constructor from another date object
       : AttributValue ((const AttributValue&)other)
       , day (other.day), month (other.month), year (other.year) { }
   ADate (char Day, char Month, int Year) throw (std::invalid_argument);
   ADate (const char* pDate) throw (std::invalid_argument) {
       operator= (pDate); }          ///< Constructor from a text (unformatted)
   ADate (const std::string& date) throw (std::invalid_argument) {
       operator= (date); }           ///< Constructor from a text (unformatted)
   ADate (const struct tm& tm) {
       operator= (tm); }                 ///< Constructor from broken down time
   ADate (const time_t& date) {
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
   bool operator== (const ADate& other) { /// Compares two objects for equalnesss
      return !compare (other); }
   bool operator!= (const ADate& other) { /// Compares if two objects are not equal
      return compare (other) != 0; }
   /// Checks if one object is "younger" than the other. See compare() for details
   bool operator<  (const ADate& other) { return compare (other) < 0; }
   /// Checks if one object is "older" than the other. See compare() for details
   bool operator>  (const ADate& other) { return compare (other) > 0; }
   /// Checks if one object is "younger"  than or equal to the other. See compare() for details
   bool operator<= (const ADate& other) {
      return compare (other) <= 0; }
   /// Checks if one object is "older" than or equal to the other. See compare() for details
   bool operator>= (const ADate& other) {
      return compare (other) >= 0; }
   long compare (const ADate& other);
   //@}

   /// \name Usefull utility-functions
   //@{
   /// Returns the maximal day of the specified \c month (in the passed \c year)
   char maxDayOf () const { return maxDayOf (month, year); }
   static char maxDayOf (char month, int year);
   static bool isLeapYear (int year);
   bool isLeapYear () const {      /// Tests, if the actual year is a leap year
      return isLeapYear (year); }

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
   int           year;
};

#endif
