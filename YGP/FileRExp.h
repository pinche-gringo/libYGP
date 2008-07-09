#ifndef YGP_FILEREXP_H
#define YGP_FILEREXP_H

//$Id: FileRExp.h,v 1.22 2008/05/18 13:21:27 markus Rel $

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
#include <stdexcept>

#include <YGP/RegExp.h>


namespace YGP {

/**Class to compare text with (UNIX-file-style) regular expressions

  \note Depending on the operating system this comparison is either
        case-sensitive (*X) or just case-preserving (DOS and derivates)!

   The following characters have a special meaning:

   - <b>*</b> (Asterisk): matches any number of any characters

   - <b>?</b> (Question mark): matches any single character

   - \b >[\<match\>] matches the characters specified in match.

   - '\b [^\<match\>] or \b [!\<match\>] matches the characters not specified in match.<br>
       \<match\> ::= | \<char\>\<match\> | \<range\>\<match\> | \<character-class\>\<match\> | {}<br>
       \<range\> ::= \<low\>-\<high\><br>
       \<character-class\> ::= [:\<class\>:]<br>
       \<class\> ::= alnum | alpha | cntrl | digit | space | graph | lower
                   | print | punct | space | upper</p><p>
       \note To include the character square bracket ([) in the match, it
             must be the first character; similar to the match-negators caret
             (^) and exclamaition-mark (!), wich must \b not be the first
             character to get included.

   \note The \c pExpression-parameter is stored as is (and not copied); so
         take care it is valid during the life-time of the object.

   Use IRegularExpression::matches to check if the object matches some data.
*/
class FileRegularExpr : public IRegularExpression {
 public:
   /// Constructor; sets the regular expression to match
   /// \pre The input is not copied, so it must be valid during the lifetime
   ///      of the regular expression.
   FileRegularExpr (const char* pRegExp) : IRegularExpression (pRegExp) { }
   virtual ~FileRegularExpr ();

   virtual int checkIntegrity () const throw (std::invalid_argument);
   /// Assignmentoperator; specifies the regular expression to match.
   /// \pre The input is not copied, so it must be valid during the lifetime
   ///      of the regular expression.
   FileRegularExpr& operator= (const char* pRegExp) {
      IRegularExpression::operator= (pRegExp);
      return *this; }

 protected:
   virtual bool compare (const char* pAktRegExp, const char* pCompare);

 private:
   // Prohibited manager functions
   FileRegularExpr ();
   FileRegularExpr (const FileRegularExpr&);
   const FileRegularExpr& operator= (const FileRegularExpr&);

   std::invalid_argument getError (const char* error, unsigned int pos) const;
};

}

#endif
