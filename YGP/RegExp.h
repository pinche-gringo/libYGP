#ifndef REGEXP_H
#define REGEXP_H

//$Id: RegExp.h,v 1.7 2002/08/20 05:18:42 markus Rel $

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

#include <assert.h>

// Base-class for any regular expression. Provides interface for other
// regular-expression-like classes.
//
// Note: The pExpression-parameter is stored as is (and not copied); so take
//       care it is valid during the life-time of the object.
class IRegularExpression {
 public:
   bool matches (const char* pCompare) {
      assert (pCompare); assert (pRegExp); assert (!checkIntegrity ());
      return compare (pRegExp, pCompare); }

   virtual int checkIntegrity () const { return 0; }

 protected:
   IRegularExpression (const char* pExpression) : pRegExp (pExpression) { }
   virtual ~IRegularExpression () { }

   IRegularExpression& operator= (const char* pExpr) { pRegExp = pExpr; return *this; }
   const char* getExpression () const { return pRegExp; }

   virtual bool compare (const char* pAktRegExp, const char* pCompare) = 0;

 private:
   // Prohibited manager functions
   IRegularExpression ();
   IRegularExpression (const IRegularExpression&);
   const IRegularExpression& operator= (const IRegularExpression&);

   const char* pRegExp;
};

#endif
