#ifndef CREGEXP_H
#define CREGEXP_H

//$Id: CRegExp.h,v 1.14 2002/04/19 00:46:20 markus Exp $

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

#include <gzo-cfg.h>

#undef HAVE_REGEX_H
#undef HAVE_REGEXP_H

#ifdef HAVE_REGEX_H
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

#include <RegExp.h>

// Class to compare text with regular expressions. Here is a little
// intruduction into the supported constructs; see regex(7) or the GNU regexp
// manual for a better description.
// '*' Is not a construct by itself; it is a suffix which repeats the (smallest
//     possible) preceding regular expression as many times as possible.
// '+' is a suffix character similar to '*', except that it requires that the
//     preceeding regular expression is matched at least once.
// '?' is a suffix character similar to '*', except that it requires that the
//     preceeding regular expression is matched either once or not at all.
// '.' matches any single character.
// {i}, {i,}, {i,j} the previous regular expression must be repeated either
//                  exactly i times; i times or more; i through j (inclusive)
//                  times.
// '^' Matches an empty string in the beginning of the line.
// '$' Similar to the caret (^) this matches the end of the line.
// '\' - Quotes the following character (including the backlash (\) itself:
//     - '\DIGIT' to repeat the matched text of the DIGIT'th '\(...\)-construct.
//     - '\b' matches the empty string, provided its at the beginning or the
//            end of a word.
//     - '\B' matches the empty string, provided its not at the beginning or
//            the end of a word.
//     - '\<' matches the empty string, provided its at the beginning of a word.
//     - '\>' matches the empty string, provided its at the end of a word.
//     - '\w' matches any word-constituent character.
//     - '\W' matches any character that is not word-constituent.
// '[<match>] matches the characters specified in match.
// '[^<match>] or [!<match>] matches the characters not specified in match.
//     <match> ::= | <char><match> | <range><match> | <character-class><match> | {}
//     <range> ::= <low>-<high>
//     <character-class> ::= [:<class>:]
//     <class> ::= alnum | alpha | cntrl | digit | space | graph | lower
//                 | print | punct | space | upper
//     Note: To include the character square bracket ([) in the match, it
//           must be the first character; similar to the caret (^), wich must
//           *not* be the first character to get included.
// '|' seperates two alternatives.
// '(...)' groups regular expressions and marks the matching substring
//         for future reference.
//
// Note: The pExpression-parameter is stored as is (and not copied); so take
//       care it is valid during the life-time of the object.
//
// Use the matches-method of the parent (RegularExpression) to check if the
// object matches some data.
class RegularExpression : public IRegularExpression {
 public:
   RegularExpression (const char* pRegExp) throw (std::string);
   virtual ~RegularExpression ();

   virtual int checkIntegrity () const throw (std::string);
   RegularExpression& operator= (const char* pRegExp);

   // Contstants for repeating
   static const char MULTIMATCHOPT = '*';
   static const char MULTIMATCHMAND = '+';
   static const char MULTIMATCH1 = '?';
   static const char BOUNDBEG = '{';
   static const char BOUNDEND = '}';

   // Special single characters
   static const char SINGLEMATCH = '.';
   static const char LINEBEGIN = '^';
   static const char LINEEND = '$';
   static const char ESCAPE = '\\';

   // Contants related to regions
   static const char REGIONBEGIN = '[';
   static const char REGIONEND = ']';
   static const char RANGE = '-';
   static const char NEGREGION = '^';
   static const char REGIONCLASS = ':';

   // Escaped special characters (after a quoting backslash (\))
   static const char GROUPBEGIN = '(';
   static const char GROUPEND = ')';
   static const char ALTERNATIVE = '|';
   static const char WORD = 'w';
   static const char NOTWORD = 'W';
   static const char WORDBORDER = 'b';
   static const char NOTWORDBORDER = 'B';
   static const char WORDBEGIN = '<';
   static const char WORDEND = '>';

 protected:
   virtual bool compare (const char* pAktRegExp, const char* pCompare);
#ifndef HAVE_REGEX_H
   bool doCompRegion (const char*& pAktRegExp, const char* pEnd, const char*& pCompare) const;
   bool doCompChar (const char*& pAktRegExp, const char* pEnd, const char*& pCompare) const;
   bool doCompEscChar (const char*& pAktRegExp, const char* pEnd, const char*& pCompare) const;
   bool compRegion (const char*& pAktRegExp, const char*& pCompare);
   bool compGroup (const char*& pAktRegExp, const char*& pCompare);
   bool compChar (const char*& pAktRegExp, const char*& pCompare);
   bool compEscChar (const char*& pAktRegExp, const char*& pCompare);

   typedef bool (RegularExpression::*MFCOMPARE) (const char*&, const char*,
                                                 const char*&) const;

   const char* getRepeatFactor (const char* pRE, int& min, int& max) const;
   bool compActREPart (MFCOMPARE fnCompare, const char*& pAktRegExp,
                       const char* pEndRE, const char*& pCompare);
#endif

 private:
   // Prohibited manager functions
   RegularExpression ();
   RegularExpression (const RegularExpression&);
   const RegularExpression& operator= (const RegularExpression&);

   std::string getError (int rc, unsigned int pos) const;

   enum { REGION_OPEN, NO_PREV_EXP, RANGE_OPEN, GROUP_OPEN, INV_DIGIT,
          INV_RANGE, ENDING_BACKSLASH, INV_BOUND };

#ifdef HAVE_REGEX_H
   regex_t regexp;

   void init (const char* pRegExp) throw (std::string);
#else
   bool doCompare (const char*& pAktRegExp, const char*& pCompare);
   bool compareParts (const char*& pAktRegExp, const char*& pCompare, bool inGroup = false);
   const char* findEndOfAlternative (const char* pRegExp, bool inGroup = false) const;
   const char* findEndOfRegion (const char* pRegExp) const;
   const char* findEndOfGroup (const char* pRegExp) const;

   const char* pStartCompare;
#endif
};

#endif
