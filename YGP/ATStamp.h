#ifndef ATSTAMP_H
#define ATSTAMP_H

//$Id: ATStamp.h,v 1.20 2003/11/14 00:22:56 markus Exp $

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

#include <string>

#include <YGP/ADate.h>
#include <YGP/ATime.h>


// Forward declarations
namespace std {
   class invalid_argument;
}

/**Class for timestamp (containing of date and time; as e.g. files)
   attributes. As every AttributValue is supports undefined values.
*/
class ATimestamp : virtual public ADate, virtual public ATime {
 public:
   ATimestamp ();
   ATimestamp (bool now);
   ATimestamp (const ATimestamp& other) : ADate ((const ADate&)other)
      , ATime ((const ATime&)other) { }  ///< Copy constructor from another ATimestamp object
   ATimestamp (char Day, char Month, int Year, char Hour,
               char minute, char second) throw (std::invalid_argument);
   ATimestamp (const char* pStamp) throw (std::invalid_argument) {
      operator= (pStamp); }         ///< Constructor from an (unformatted) text
   ATimestamp (const std::string& stamp) throw (std::invalid_argument) {
      operator= (stamp); }          ///< Constructor from an (unformatted) text
   ATimestamp (const struct tm& tm) {
      operator= (tm); }                ///< Constructor from a broken down time
   ATimestamp (const time_t& stamp, bool local = true) {
      local ? operator= (stamp) : assignGMT (stamp); } ///< Constructor from a system time
   virtual ~ATimestamp ();

   /// Checks if the object is defined (has a value)
   bool isDefined () const { return ATime::isDefined (); }
   virtual void undefine () {   /// Undefines the object (has no value anymore)
      ADate::undefine ();
      ATime::undefine ();
   }

   /// Returns the current date/time
   static ATimestamp now () { return ATimestamp (true); }

   /// \name Assignment methods
   //@{
   /// Assignment operator from an (unformatted) text
   ATimestamp& operator= (const std::string& stamp) throw (std::invalid_argument) {
      return operator= (stamp.c_str ()); }
   ATimestamp& operator= (const char* pValue) throw (std::invalid_argument);
   ATimestamp& operator= (const struct tm& tm);
   ATimestamp& operator= (const ATimestamp& other);
   ATimestamp& operator= (const time_t& stamp) { return operator= (*gmtime (&stamp)); }

   ATimestamp& assignGMT (const time_t& stamp) { return operator= (*::localtime (&stamp)); }

   virtual void readFromStream (std::istream& in) throw (std::invalid_argument);
   //@}

   /// Defining the object; setting it to a default value (of <tt>1/1/1900 0:0:0</tt>)
   virtual void define () { ADate::define (); ATime::define (); }

   /// Input from a stream using the readFromStream() method
   friend std::istream& operator>> (std::istream& in, ATimestamp& inValue) {
      inValue.readFromStream (in); return in; }
   /// Output to a stream using the toUnformattedString() method
   friend std::ostream& operator<< (std::ostream& out, const ATimestamp& outValue) {
      if (outValue.isDefined ())
         out << outValue.toUnformattedString ();
      return out; }

   /// \name Convertion
   //@{
   virtual std::string toUnformattedString () const;
   virtual std::string toString () const;
   virtual std::string toString (const char* format) const;

   /// Converting into a system time (GMT)
   time_t toSysTime () const { return toLocalTime (); }
   struct tm toStructTM () const;
   time_t toGMTTime () const;
   time_t toLocalTime () const { /// Converting into a system time (local time)
      struct tm result (toStructTM ()); return mktime (&result); }
   //@}

   /// \name Calculation
   //@{
   ATimestamp& operator+= (const ATimestamp& rhs);
   ATimestamp& operator-= (const ATimestamp& rhs);

   friend ATimestamp operator+ (const ATimestamp& lhs, const ATimestamp& rhs);
   friend ATimestamp operator- (const ATimestamp& lhs, const ATimestamp& rhs);

   ATimestamp& add (char Day, char Month = 0, int Year = 0,
                    char Hour = 0, char minute = 0,char second = 0);
   ATimestamp& sub (char Day, char month = 0, int Year = 0,
                    char Hour = 0, char minute = 0,char second = 0);
   //@}

   /// \name Comparison
   //@{
   bool operator== (const ATimestamp& other) { /// Compares two objects for equalnesss
      return !compare (other); }
   bool operator!= (const ATimestamp& other) { /// Compares if two objects are not equal
      return compare (other) != 0; }
   /// Checks if one object is "younger" than the other. See compare() for details
   bool operator<  (const ATimestamp& other) { return compare (other) < 0; }
   /// Checks if one object is "older" than the other. See compare() for details
   bool operator>  (const ATimestamp& other) { return compare (other) > 0; }
   /// Checks if one object is "younger"  than or equal to the other. See compare() for details
   bool operator<= (const ATimestamp& other) { return compare (other) <= 0; }
   /// Checks if one object is "older" than or equal to the other. See compare() for details
   bool operator>= (const ATimestamp& other) { return compare (other) >= 0; }
   long compare (const ATimestamp& other);
   //@}

   // Usefull utility-functions
   int checkIntegrity () const;

 protected:
   virtual bool maxAdapt ();
   virtual bool minAdapt ();
};

#endif
