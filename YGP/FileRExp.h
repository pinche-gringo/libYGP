#ifndef REGEXP_H
#define REGEXP

//$Id: FileRExp.h,v 1.2 2000/03/27 22:37:13 Markus Rel $

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


// Class to compare text with (UNIX-file-style) regular expressions
// '*' matches any number of any characters
// '?' matches any one character
// '[<match>] matches the characters specified in match
// '[^<match>] matches the characters not specified in match
//     <match> ::= | <char><match> | <region><match> | {}
//     <region> ::= <low>-<high>
//
// Note: The pRegExp-parameter is stored as is (and not copied); so care
//       yourself for all the neccessary cleanup-stuff
class FileRegularExpr {
 public:
   FileRegularExpr () : pFileRegExp (NULL) { }
   FileRegularExpr (const char* pRegExp) : pFileRegExp (pRegExp) { }
   ~FileRegularExpr () { }

   bool matches (const char* pCompare) const {
      assert (pCompare); assert (pFileRegExp); assert (!checkIntegrity ());
      return matches (pFileRegExp, pCompare); }

   int  checkIntegrity () const;
   FileRegularExpr& operator= (const char* pRegExp) { pFileRegExp = pRegExp; return *this; }

   enum { MULTIMATCH = '*', SINGLEMATCH = '?', REGIONBEGIN = '[',
          REGIONEND = ']', REGION = '-', NEGREGION = '^' };

 private:
   // Prohibited manager functions
   FileRegularExpr (const FileRegularExpr&);
   const FileRegularExpr& operator= (const FileRegularExpr&);

   bool matches (const char* pAktRegExp, const char* pCompare) const;

   const char* pFileRegExp;
};

#endif
