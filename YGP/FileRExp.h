#ifndef FILEREXP_H
#define FILEREXP_H

//$Id: FileRExp.h,v 1.3 2000/05/14 17:47:37 Markus Exp $

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


#include <stddef.h>

#include <assert.h>

#include "RegExp.h"

// Class to compare text with (UNIX-file-style) regular expressions
// '*' matches any number of any characters
// '?' matches any one character
// '[<match>] matches the characters specified in match
// '[^<match>] matches the characters not specified in match
//     <match> ::= | <char><match> | <region><match> | {}
//     <region> ::= <low>-<high>
//
// Note: The pExpression-parameter is stored as is (and not copied); so take
//       care it is valied during the life-time of the object.
//
// Use the matches-method of the parent (RegularExpression) to check if the
// object matches some data.
class FileRegularExpr : public RegularExpression {
 public:
   FileRegularExpr (const char* pRegExp) : RegularExpression (pRegExp) { }
   virtual ~FileRegularExpr ();

   virtual int  checkIntegrity () const;
   FileRegularExpr& operator= (const char* pRegExp) {
      return (FileRegularExpr&)RegularExpression::operator= (pRegExp); }

   enum { MULTIMATCH = '*', SINGLEMATCH = '?', REGIONBEGIN = '[',
          REGIONEND = ']', REGION = '-', NEGREGION = '^' };

 protected:
   virtual bool compare (const char* pAktRegExp, const char* pCompare) const;

 private:
   // Prohibited manager functions
   FileRegularExpr ();
   FileRegularExpr (const FileRegularExpr&);
   const FileRegularExpr& operator= (const FileRegularExpr&);
};

#endif
