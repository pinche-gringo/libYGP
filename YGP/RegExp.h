#ifndef YGP_REGEXP_H
#define YGP_REGEXP_H

//$Id: RegExp.h,v 1.15 2008/05/18 13:21:27 markus Rel $

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


#include <YGP/Check.h>

namespace YGP {

/**Base-class for any regular expression. Provides interface for other
   regular-expression-like classes.

   \note The <tt>pExpression</tt>-parameter is stored as is (and not copied);
       so take care it is valid during the life-time of the object.
*/
class IRegularExpression {
 public:
   /// Checks, if the regular expression matches the passed text
   /// \param pCompare Pointer to text to match
   bool matches (const char* pCompare) {
      Check1 (pCompare); Check1 (pRegExp); Check1 (!checkIntegrity ());
      return compare (pRegExp, pCompare); }

   /// Checks the integrity of the object
   virtual int checkIntegrity () const { return 0; }

 protected:
   /// Constructor; sets the regular expression
   IRegularExpression (const char* pExpression) : pRegExp (pExpression) { }
   /// Destructor
   virtual ~IRegularExpression () { }

   /// Assignment operator; sets the regular expression
   IRegularExpression& operator= (const char* pExpr) { pRegExp = pExpr; return *this; }
   /// Returns the regular expression
   const char* getExpression () const { return pRegExp; }

   /// Method to compare (a part of) the regular expression with some text
   virtual bool compare (const char* pAktRegExp, const char* pCompare) = 0;

 private:
   // Prohibited manager functions
   IRegularExpression ();
   IRegularExpression (const IRegularExpression&);
   const IRegularExpression& operator= (const IRegularExpression&);

   const char* pRegExp;
};

}

#endif
