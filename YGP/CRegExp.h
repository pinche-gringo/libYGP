#ifndef CREGEXP_H
#define CREGEXP_H

//$Id: CRegExp.h,v 1.1 2000/05/15 00:14:44 Markus Exp $

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


#ifdef HAVE_REGEX_H
//#  undef HAVE_REGEX_H
#  include <sys/types.h>
#  include <regex.h>
#else
#  ifdef HAVE_REGEXP_H
#     define HAVE_REGEX_H
#     include <regexp.h>
#  endif
#endif

#include <assert.h>

#include <string>

#include "RegExp.h"

// Class to compare text with regular expressions. Here is a little
// intruduction into the supported constructs; see the EMACS-info for a
// better description.
// '*' Is not a construct by itself; it is a suffix which repeats the (smallest
//     possible) preceding regular expression as many times as possible.
// '+' is a suffix character similar to '*', except that it requires that the
//     preceeding regular expression is matched at least once.
// '?' is a suffix character similar to '*', except that it requires that the
//     preceeding regular expression is matched either once or not at all.
// '.' matches any single character.
// '^' Matches an empty string in the beginning of the line.
// '$' Similar to the caret (') this matches the end of the line.
// '\' - Quotes the following character (including the backlash (\) itself
//     - '\|' seperates two alternatives
//     - '\(...\)' groups regular expressions and marks the matching substring
//                 for future reference
//     - '\DIGIT' to repeat the matched text of the DIGIT'th '\(...\)-construt.
//     - '\`' matches the empty string, provided its at the beginning of the buffer.
//       '\'' matches the empty string, provided its at the end of the buffer.
//     - '\b' matches the empty string, provided its at the beginning or the
//            end of a word
//     - '\B' matches the empty string, provided its not at the beginning or
//            the end of a word
//     - '\<' matches the empty string, provided its at the beginning of a word
//     - '\>' matches the empty string, provided its at the end of a word
//     - '\w' matches any word-constituent character.
//     - '\W' matches any character that is not word-constituent.
//     - '\sCODE' matches any character whose syntax is CODE.
//     - '\SCODE' matches any character whose syntax is not CODE.
// '[<region>] matches the characters specified in match
// '[^<region>] matches the characters not specified in match
//     <region> ::= | <char><region> | <range><region> | {}
//     <range> ::= <low>-<high>
//     Note: To include the character square bracket ([) in the match, it
//           must be the first character; similar to the caret (^), wich must
//           *not* be the first character to get included.
//
// Note: The pExpression-parameter is stored as is (and not copied); so take
//       care it is valied during the life-time of the object.
//
// Use the matches-method of the parent (RegularExpression) to check if the
// object matches some data.
class RegularExpression : public IRegularExpression {
 public:
   RegularExpression (const char* pRegExp);
   virtual ~RegularExpression ();

   virtual int checkIntegrity () const throw (std::string);
   RegularExpression& operator= (const char* pRegExp);

   enum { MULTIMATCHOPT = '*', MULTIMATCHMAND = '+', MULTIMATCH1 = '?',
          SINGLEMATCH = '.', LINEBEGIN = '^', LINEEND = '$', QUOTE = '\\',
          REGIONBEGIN = '[', REGIONEND = ']', RANGE = '-', NEGREGION = '^' };

 protected:
   virtual bool compare (const char* pAktRegExp, const char* pCompare) const;

 private:
   // Prohibited manager functions
   RegularExpression ();
   RegularExpression (const RegularExpression&);
   const RegularExpression& operator= (const RegularExpression&);

   std::string getError (int rc, unsigned int pos) const;

   enum { REGION_OPEN, NO_PREV_EXP, RANGE_OPEN };

#ifdef HAVE_REGEX_H
   regex_t regexp;
#endif

};

#endif
