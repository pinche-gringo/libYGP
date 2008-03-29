#ifndef CREGEXP_H
#define CREGEXP_H

//$Id: CRegExp.h,v 1.38 2008/03/29 17:35:17 markus Rel $

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


#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif


#include <ygp-cfg.h>

// Uncommment to work with own regular-expressions, even if real ones are present
// #undef HAVE_REGEX_H
// #undef HAVE_REGEXP_H

#ifdef HAVE_REGEX_H
#  include <sys/types.h>
#  include <regex.h>
#else
#  include <ctype.h>
#endif

#include <string>
#include <vector>
#include <stdexcept>

#include <YGP/RegExp.h>

namespace YGP {

/**Class to compare text with (UNIX-style) regular expressions. This is just
   a little intruduction into the supported constructs; see regex(7) or the
   GNU regexp manual for a better description.

   \attention This class implements regular expressions as the functions in \c
         regex.h do (at least in glibc 2.2.4), which might differ from what
         you expect from the reading (or how - e.g. - emacs behaves). But I
         have no idea what the *real* standard is and for testing it was the
         easiest to follow an existing implementation!

   Anyway; here's something to give you a glue:

   - <b>*</b> (Asterisk) Is not a construct by itself; it is a suffix which
         repeats the (smallest possible) preceding regular expression as many
         times as possible.

   - <b>+</b> (Plus) is a suffix character similar to '*', except that it
         requires that the preceeding regular expression is matched at least
         once.

   - <b>?</b> (Question mark) is a suffix character similar to '*', except
         that it requires that the preceeding regular expression is matched
         either once or not at all.

   - <b>.</b> (Dot) matches any single character.

   - <b>{i}, {i,}, {i,j}</b> the previous regular expression must be repeated
         either exactly i times; i times or more; i through j (inclusive)
         times.

   - <b>^</b> (Caret) Matches an empty string in the beginning of the line.

   - <b>$</b> (Dollar) Similar to the caret (^) this matches the end of the
        line.

   - <b>\\</b> (Backslash):
     - Quotes the following character (including the backlash (\) itself:
     - <b>\\DIGIT</b> to repeat the matched text of the DIGIT'th
         (...)-construct.
     - <b>\\b</b> matches the empty string, provided its at the beginning or the
            end of a word.
     - <b>\\B</b> matches the empty string, provided its not at the beginning or
            the end of a word.
     - <b>\\\<</b> matches the empty string, provided its at the beginning of a word.
     - <b>\\\></b> matches the empty string, provided its at the end of a word.
     - <b>\\w</b> matches any word-constituent character.
     - <b>\\W</b> matches any character that is not word-constituent.

   - <b>[\<match\>]</b> matches the characters specified in match.

   - <b>[^\<match\>]</b> or <b>[!\<match\>]</b> matches the characters not specified in match.<br>
       \<match\> ::= | \<char\>\<match\> | \<range\>\<match\> | \<character-class\>\<match\> | {}<br>
       \<range\> ::= \<low\>-\<high\><br>
       \<character-class\> ::= [:\<class\>:]<br>
       \<class\> ::= alnum | alpha | cntrl | digit | space | graph | lower
                   | print | punct | space | upper</p><p>
       \note To include the character square bracket ([) in the match, it
             must be the first character; similar to the match-negators caret
             (^) and exclamaition-mark (!), wich must \b not be the first
             character to get included.

   - <b>|</b> seperates two alternatives.

   -  <b>(...)</b> groups regular expressions and marks the matching substring
             for future reference.

   If the class is compiled with \c ENHANCED_REGEXP defined the following
   constructs are also supported (Warning: Those things are not regulary
   tested):

   - \<match\> ::= ... | \<wordborder\>\<match\><br>
        \<wordborder\> ::= [:\<:] | [:\>:]           (Matches beginn or end of word)


   \note The \c pExpression-parameter is stored as is (and not copied); so
         take care it is valid during the life-time of the object.

   Use IRegularExpression::matches to check if the object matches some data.
*/
class RegularExpression : public IRegularExpression {
 public:
   RegularExpression (const char* pRegExp) throw (std::invalid_argument);
   virtual ~RegularExpression ();

   virtual int checkIntegrity () const throw (std::invalid_argument);
   RegularExpression& operator= (const char* pRegExp) throw (std::invalid_argument);

 protected:
   virtual bool compare (const char* pAktRegExp, const char* pCompare);

 private:
   // Prohibited manager functions
   RegularExpression ();
   RegularExpression (const RegularExpression&);
   RegularExpression& operator= (const RegularExpression&);

   std::invalid_argument getError (int rc, unsigned int pos) const;

   enum { REGION_OPEN, NO_PREV_EXP, RANGE_OPEN, GROUP_OPEN, INV_DIGIT,
          INV_RANGE, ENDING_BACKSLASH, INV_BOUND };

#ifdef HAVE_REGEX_H
   regex_t regexp;

   void init (const char* pRegExp) throw (std::invalid_argument);
#else
   bool doCompare (const char*& pAktRegExp, const char*& pCompare);

   bool doCompGroup (const char*& pAktRegExp, const char* pEnd,
                     const char*& pCompare, unsigned int min, unsigned int max);
   bool doCompRegion (const char*& pAktRegExp, const char* pEnd, const char*& pCompare);
   bool doCompChar (const char*& pAktRegExp, const char* pEnd, const char*& pCompare);
   bool doCompEscChar (const char*& pAktRegExp, const char* pEnd, const char*& pCompare);
   bool compRegion (const char*& pAktRegExp, const char*& pCompare);
   bool compGroup (const char*& pAktRegExp, const char*& pCompare);
   bool compChar (const char*& pAktRegExp, const char*& pCompare);
   bool compEscChar (const char*& pAktRegExp, const char*& pCompare);

   bool isWordConstituent (char ch) const { return isalnum (ch) || (ch == '_'); }
   bool isWordBorder (const char* pCompare) const;
   bool isWordBeginn (const char* pCompare) const;
   bool isWordEnd (const char* pCompare) const;

   typedef bool (RegularExpression::*MFCOMPARE) (const char*&, const char*, const char*&);

   const char* getRepeatFactor (const char* pRE, unsigned int& min,
                                unsigned int& max) const;
   bool compActREPart (MFCOMPARE fnCompare, const char*& pAktRegExp,
                       const char* pEndRE, const char*& pCompare);

   bool compareParts (const char*& pAktRegExp, const char*& pCompare,
                      bool inGroup = false);
   const char* findEndOfAlternative (const char* pRegExp, bool inGroup = false) const;
   const char* findEndOfRegion (const char* pRegExp) const;
   const char* findEndOfGroup (const char* pRegExp) const;

   const char* pStartCompare;
   std::vector<std::string> groupValues;
   unsigned int cGroups;
#endif
};

}

#endif
