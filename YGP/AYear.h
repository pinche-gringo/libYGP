#ifndef YGP_AYEAR_H
#define YGP_AYEAR_H

//$Id: AYear.h,v 1.7 2008/05/18 13:21:27 markus Rel $

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


#include <string>

#include <YGP/Check.h>
#include <YGP/AttrVal.h>


// Forward declarations
namespace std {
   class invalid_argument;
}


namespace YGP {

/**Class for year attributes. As every AttributValue is supports undefined
   values.
*/
class AYear : public AttributValue {
 public:
   AYear ()                /// Default constructor; creates an undefined object
       : AttributValue (), year (1900) { }
   AYear (const AYear& other)     /// Copy constructor from another year object
       : AttributValue ((const AttributValue&)other) , year (other.year) { }
   AYear (int Year)                      ///< Constructor; sets the passed year
      : AttributValue (true), year (Year) { }
   AYear (const char* pYear) throw (std::invalid_argument) : AttributValue (), year (1900) {
       operator= (pYear); }                        ///< Constructor from a text
   AYear (const std::string& year) throw (std::invalid_argument) : AttributValue (), year (1900) {
       operator= (year); }                         ///< Constructor from a text
   virtual ~AYear ();

   /// \name Assignment methods
   //@{
   /// Assignment operator from an (unformatted) text
   AYear& operator= (const std::string& year) throw (std::invalid_argument) {
      return operator= (year.c_str ()); }
   AYear& operator= (const AYear& other);
   AYear& operator= (const char* pValue) throw (std::invalid_argument);
   AYear& operator= (int Year) { setDefined (); year = Year; return *this; }

   virtual void readFromStream (std::istream& in) throw (std::invalid_argument);
   //@}

   /// Defining the object; setting it to a default value (of <tt>1900</tt>)
   virtual void define () { setDefined (); year = 1900; }

   /// \name Query methods
   //@{
   operator int () const { return year; }                ///< Casting to an int
   //@}

   /// \name Convertion
   //@{
   virtual std::string toUnformattedString () const;
   //@}

   /// \name Calculation
   //@{
   AYear& operator+= (const AYear& rhs);
   AYear& operator-= (const AYear& rhs);

   friend AYear operator+ (const AYear& lhs, const AYear& rhs);
   friend AYear operator- (const AYear& lhs, const AYear& rhs);
   //@}

   /// \name Comparison
   //@{
   bool operator== (const AYear& other) const { /// Compares two objects for equalnesss
      return !compare (other); }
   bool operator!= (const AYear& other) const { /// Compares if two objects are not equal
      return compare (other) != 0; }
   /// Checks if one object is "younger" than the other. See compare() for details
   bool operator<  (const AYear& other) const { return compare (other) < 0; }
   /// Checks if one object is "older" than the other. See compare() for details
   bool operator>  (const AYear& other) const { return compare (other) > 0; }
   /// Checks if one object is "younger"  than or equal to the other. See compare() for details
   bool operator<= (const AYear& other) const {
      return compare (other) <= 0; }
   /// Checks if one object is "older" than or equal to the other. See compare() for details
   bool operator>= (const AYear& other) const {
      return compare (other) >= 0; }
   long compare (const AYear& other) const;
   //@}

   /// \name Usefull utility-functions
   //@{
   /// Returns the maximal day of the specified \c month (in the passed \c year)
   static bool isLeapYear (int year);
   bool isLeapYear () const {      /// Tests, if the actual year is a leap year
      return isLeapYear (year); }
   //@}

 protected:
   int year;                                              ///< Stores the year
};

}

#endif
