#ifndef REGEXP_H
#define REGEXP_H

//$Id: RegExp.h,v 1.1 2000/05/14 17:47:18 Markus Exp $

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


// Base-class for any regular expression. Provides interface for other
// regular-expression-like classes.
//
// Note: The pExpression-parameter is stored as is (and not copied); so take
//       care it is valied during the life-time of the object.
class RegularExpression {
 public:
   bool matches (const char* pCompare) const {
      assert (pCompare); assert (pRegExp); assert (!checkIntegrity ());
      return compare (pRegExp, pCompare); }

   virtual int  checkIntegrity () const { return !pRegExp; }
   RegularExpression& operator= (const char* pExpr) { pRegExp = pExpr; return *this; }

 protected:
   RegularExpression (const char* pExpression) : pRegExp (pExpression) { assert (pRegExp); }
   virtual ~RegularExpression () { }

   const char* getExpression () const { return pRegExp; }

   virtual bool compare (const char* pAktRegExp, const char* pCompare) const = 0;

 private:
   // Prohibited manager functions
   RegularExpression ();
   RegularExpression (const RegularExpression&);
   const RegularExpression& operator= (const RegularExpression&);

   const char* pRegExp;
};

#endif
