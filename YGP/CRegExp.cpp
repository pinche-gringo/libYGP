//$Id: CRegExp.cpp,v 1.40 2005/11/28 00:24:06 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : RegularExpression
//REFERENCES  :
//TODO        : Maybe "compile" when constructing? Like an array of
//              compare-objects (with repeat-factor). Maybe check, how
//              regexp is doing its compile.
//BUGS        : Probably (regular expressions are quite complex); YOU tell me
//REVISION    : $Revision: 1.40 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.5.2000
//COPYRIGHT   : Copyright (C) 2000 - 2004

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


#include "YGP/Internal.h"

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/CRegExp.h"
#include "YGP/Internal.h"
#include "YGP/ANumeric.h"

#ifndef HAVE_REGEX_H
#  include <cstdlib>
#  define isclass(type,str,len,ch) (strncmp ((str), #type, (len)) ? 0 : (is##type (ch) ? 2 : 1))
#endif


namespace YGP {

// Various constants for special characters used by regular expressions.
// Can't be in the header as fucked-up Visual C++ (in Version 6!) would consider
// them as pure virtual function. Fucking wanker (or fucking wanking creators)
// Furthermore can't even be const chars, as Borland C is that not constant
// enough for casees. Why do I bother about staying portable with Windows?
// Contstants for repeating
#define MULTIMATCHOPT '*'
#define MULTIMATCHMAND '+'
#define MULTIMATCH1 '?'
#define BOUNDBEG '{'
#define BOUNDEND '}'

// Special single characters
#define SINGLEMATCH '.'
#define LINEBEGIN '^'
#define LINEEND '$'
#define ESCAPE '\\'

// Contants for groups
#define GROUPBEGIN '('
#define GROUPEND ')'
#define ALTERNATIVE '|'

// Contants related to regions
#define REGIONBEGIN '['
#define REGIONEND ']'
#define RANGE '-'
#define NEGREGION1 '^'
#define NEGREGION2 '!'
#define REGIONCLASS ':'

// Escaped special characters (after a quoting backslash (\))
#define WORD 'w'
#define NOTWORD 'W'
#define WORDBORDER 'b'
#define NOTWORDBORDER 'B'
#define WORDBEGIN '<'
#define WORDEND '>'


//----------------------------------------------------------------------------
/// Constructor, specifies the regular expression to match.
/// \param pRegExp: Pointer to character array holding regular expression
/// \throw std::string: In case of an invalid regexp a describing text
/// \pre The input is not copied, so it must be valid during the lifetime
///      of the regular expression.
//----------------------------------------------------------------------------
RegularExpression::RegularExpression (const char* pRegExp) throw (std::string)
   : IRegularExpression (pRegExp) {
#ifdef HAVE_REGEX_H
   init (pRegExp);
#else
   checkIntegrity ();
#endif
}

//----------------------------------------------------------------------------
///Destructor
//----------------------------------------------------------------------------
RegularExpression::~RegularExpression () {
#ifdef HAVE_REGEX_H
   regfree (&regexp);
#endif
}


//----------------------------------------------------------------------------
/// Assignmentoperator; specifies the regular expression to match.
/// \param pRegExp: Pointer to character array holding regular expression
/// \return \c RegularExpression&: Reference to this
/// \throw std::string: In case of an invalid regexp a describing text
/// \pre The input is not copied, so it must be valid during the lifetime
///      of the regular expression.
//----------------------------------------------------------------------------
RegularExpression& RegularExpression::operator= (const char* pRegExp) throw (std::string) {
   IRegularExpression::operator= (pRegExp);
#ifdef HAVE_REGEX_H
   regfree (&regexp);
   init (pRegExp);
#else
   checkIntegrity ();
#endif
   return *this;
}

//----------------------------------------------------------------------------
/// Checks if the passed text matches the passed regular expression.
/// Both passed parameters must not be NULL-pointers and the regular expression
/// must be in a valid syntax (this can be checked with the
/// checkIntegrity()-method).
/// \param pActRegExp: Pointer to character array holding regular expression
/// \param pCompare: Pointer to character array holding value to compare
/// \return \c  bool: Result (true: match)
/// \pre \c pAktRegExp, \c pCompare: ASCIIZ-strings, not NULL
//----------------------------------------------------------------------------
bool RegularExpression::compare (const char* pActRegExp, const char* pCompare) {
   Check1 (pActRegExp); Check1 (pCompare); Check1 (!checkIntegrity ());

   TRACE1 ("RegularExpression::compare (const char*, const char*) -> "
           << pActRegExp << " <-> " << pCompare);

#ifdef HAVE_REGEX_H
   // Use system-regular expressions if available
   regmatch_t match[1];
   int ret (regexec (&regexp, pCompare, sizeof (match) / sizeof (match[0]), match, 0));
   if (ret < 0)
      throw (getError (ret, 0));

   TRACE5 ("RegularExpression::compare (const char*, const char*) -  "
           "Subexpr.: " << match[0].rm_so << " - " << match[0].rm_eo);

   return ret ? false
              : ((match[0].rm_so == 0)
                 && (static_cast<unsigned int> (match[0].rm_eo) == strlen (pCompare)));
#else
   pStartCompare = pCompare;
   bool rc (compareParts (pActRegExp, pCompare));
   pStartCompare = NULL;
   cGroups = 0;
   return rc ? !*pCompare : false;
#endif
}

#ifndef HAVE_REGEX_H

//----------------------------------------------------------------------------
/// Checks if the passed string matches the regular expression; the regular
/// expression is seperated in various parts (if neccessary).
/// \param pActRegExp: Pointer to regular expression
/// \param pCompare: String to compare
/// \return \c bool: Result (true: match)
/// \pre \c pActRegExp, \c pCompare: ASCIIZ-string; not NULL
//----------------------------------------------------------------------------
bool RegularExpression::compareParts (const char*& pActRegExp,
                                      const char*& pCompare, bool inGroup) {
   Check1 (pActRegExp); Check1 (pCompare); Check1 (!checkIntegrity ());

   TRACE1 ("RegularExpression::compareParts (const char*&, const char*&) -> "
           << pActRegExp << " <-> " << pCompare);

   const char* pSaveRE;
   const char* pSaveCompare = pCompare;
   do {
      pSaveRE = pActRegExp;
      if (doCompare (pActRegExp, pCompare))  {       // Compare the whole thing
         TRACE1 ("RegularExpression::compareParts (const char*&, const char*&)"
                 " - Found; Remaining: '" << pCompare << '\'');

         if (!*pCompare || inGroup)         // String to compare empty -> Found
            return true;
      }
      pCompare = pSaveCompare;      // start comparison again (with old values)
      pActRegExp = pSaveRE;

      if ((pActRegExp = findEndOfAlternative (pActRegExp, inGroup)) == 0)
         break;
   } while (*pActRegExp++); // end-do while alternatives available

   return false;
}

//----------------------------------------------------------------------------
/// Checks if the passed string matches the regular expression.
///   - \c pActRegExp: Pointer to regular expression
///   - \c pCompare: String to compare
/// \return \c bool: Result (true: match)
/// \pre \c pActRegExp, \c pCompare: ASCIIZ-string, not NULL
//----------------------------------------------------------------------------
bool RegularExpression::doCompare (const char*& pActRegExp, const char*& pCompare) {
   Check1 (pActRegExp); Check1 (pCompare); Check1 (!checkIntegrity ());
   TRACE1 ("RegularExpression::doCompare (const char*, const char*&) -> "
           << pActRegExp << " <-> " << pCompare);

   char ch;
   bool match (false);

   while ((ch = *pActRegExp) != 0) {
      // Check actual rex-exp for special characters (at the current position)
      // After this switch the the following conditions must be fulfilled:
      // - fnCompare points to the compare-function to use
      // - pEnd points to the end of the part of the reg-exp belonging to the
      //   current position (like end of group, ...)
      // - lastExpr contains the actual part of the regexp
      switch (ch) {                                   // Get current expression
      case REGIONBEGIN:
         TRACE8 ("RegularExpression::doCompare (const char*&, const char*&) - Found region");
         match = compRegion (++pActRegExp, pCompare);
         break;

      case GROUPBEGIN:
         TRACE8 ("RegularExpression::doCompare (const char*&, const char*&) - Found group");
         match = compGroup (++pActRegExp, pCompare);
         break;

      case ESCAPE:
         match = compEscChar (++pActRegExp, pCompare);
         break;

      case ALTERNATIVE:
         TRACE4 ("RegularExpression::doCompare (const char*, const char*&) - Alternative OK");
         return true;                   // OK til act. pos -> Return true

      default:
         Check3 (ch); Check3 (ch != MULTIMATCHMAND);
         Check3 (ch != MULTIMATCH1); Check3 (ch != MULTIMATCHOPT);
         TRACE4 ("RegularExpression::doCompare (const char*, const char*&) - Found char");

         match = RegularExpression::compChar (pActRegExp, pCompare);
      } // end-switch
      TRACE7 ("RegularExpression::doCompare (const char*, const char*&) - Found: "
              << (match ? "Yes" : "No"));

      if (match) {
         if (!*pActRegExp)
            return true;
      }
      else
         return false;

      match = false;
   } // end-while

   TRACE2 ("RegularExpression::doCompare (const char*, const char*&) - Found: "
           << static_cast<const char*> (pCompare ? "False": "True"));
   return !*pCompare;                             // Match OK, if regexp empty
}

//----------------------------------------------------------------------------
/// Checks if the passed string matches the region (incl\. handling of the
/// repeat-factor.
/// \param pActRegExp: Pointer to regular expression
/// \param pCompare: String to compare
/// \return \c bool: Result (true: match)
/// \pre \c pActRegExp, \c pCompare ASCIIZ-string, not NULL
//----------------------------------------------------------------------------
bool RegularExpression::compRegion (const char*& pActRegExp,
                                    const char*& pCompare) {
   Check1 (pActRegExp); Check1 (*pActRegExp); Check1 (pCompare);

   return compActREPart (&RegularExpression::doCompRegion, pActRegExp,
                         findEndOfRegion (pActRegExp) + 1, pCompare);
}

//----------------------------------------------------------------------------
/// Checks if the passed string matches the region.
/// \param pActRegExp: Pointer to regular expression
/// \param pEnd: Pointer to end of the region in the regular expression
/// \param pCompare: String to compare
/// \return \c bool: Result (true: match)
/// \pre
///    - \c pActRegExp, \c pEnd, \c pCompare ASCIIZ-string;
///    - \c pEnd > \c pActRegExp
//----------------------------------------------------------------------------
bool RegularExpression::doCompRegion (const char*& pActRegExp, const char* pEnd,
                                      const char*& pCompare) {
   Check1 (pActRegExp); Check1 (*pActRegExp); Check1 (pCompare);
   Check1 (pEnd); Check1 (pEnd > pActRegExp);

   const char* pActRE = pActRegExp;
   while (*pEnd-- != REGIONEND)
      Check3 (pEnd > pActRegExp); ;

#if TRACELEVEL > 2
   TRACE3 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) -> "
           << *pCompare << " in [" << std::string (pActRE, pEnd - pActRE + 1) << ']');
#endif

   // Compares the actual file-char with the region
   bool fNeg (false);
   if ((*pActRE == NEGREGION1) || (*pActRE == NEGREGION2)) { // Invert values?
      ++pActRE;
      fNeg = true;
   } // endif

   char ch (*pActRE);

   do {
      if (pActRE[1] == RANGE) {
         char chUpper (pActRE[2]);
         Check3 (chUpper != '\0'); Check3 (chUpper != REGIONEND);

         TRACE7 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - Check "
                 << *pCompare << " in [" << ch << '-' << chUpper << ']');
         if ((*pActRE >= ch) && (*pCompare <= chUpper))
            break;

         pActRE += 2;
      } // endif range found
      else {
         // Check for class of characters or ordinary char
         const char* pEndClass;

         if ((ch == REGIONBEGIN) && (pActRE[1] == REGIONCLASS)
             && ((pEndClass = strchr (pActRE + 2, REGIONCLASS)) != NULL)
             && (pEndClass[1] == REGIONEND)) {
	    TRACE7 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - Check "
                    << *pCompare << " against region-class "
                    << std::string (pActRE + 2, pEndClass - pActRE - 2));

            int temp, len (pEndClass - pActRE - 2);
            int val (
#ifdef ENHANCED_REGEXP
                     (len == 1)
                     ? ((pActRE[2] == WORDBEGIN) ? (int)isWordBeginn (pCompare) + 1 :
                        (pActRE[2] == WORDEND) ? (int)isWordEnd (pCompare) + 1 : 0) :
#endif
                       (temp = isclass (alnum, pActRE + 2, len, *pCompare)) ? temp :
                       (temp = isclass (alpha, pActRE + 2, len, *pCompare)) ? temp :
                       (temp = isclass (digit, pActRE + 2, len, *pCompare)) ? temp :
                       (temp = isclass (space, pActRE + 2, len, *pCompare)) ? temp :
                       (temp = isclass (cntrl, pActRE + 2, len, *pCompare)) ? temp :
                       (temp = isclass (graph, pActRE + 2, len, *pCompare)) ? temp :
                       (temp = isclass (print, pActRE + 2, len, *pCompare)) ? temp :
                       (temp = isclass (punct, pActRE + 2, len, *pCompare)) ? temp :
                       (temp = isclass (upper, pActRE + 2, len, *pCompare)) ? temp :
                       (temp = isclass (lower, pActRE + 2, len, *pCompare)) ? temp :
                       (isclass (xdigit, pActRE + 2, len, *pCompare)));
            if (val == 2) {
               TRACE8 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - "
                       << *pCompare << " matches region-class "
                       << std::string (pActRE + 2, pEndClass - pActRE - 2));

               break;
            } // endif class and input matches
            else {
               TRACE8 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - "
                       << *pCompare << " doesn't match region-class "
                       << std::string (pActRE + 2, pEndClass - pActRE - 2));

               if (val)
                  pActRE = pEndClass + 1;
            } // end-else class found, but input doesn't match
         } // endif
         else {
            TRACE7 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - Check "
                    << *pCompare << " == " << ch);
            if (ch == *pCompare)
               break;
         } // end-else ordinary character
      } // end-else non-range

      ch = *++pActRE;
   } while (pActRE <= pEnd); // end-do

   if ((pActRE <= pEnd) == fNeg)
      return false;

   ++pCompare;
   return true;
}

//----------------------------------------------------------------------------
/// Checks if the passed string matches the group.
/// \param pActRegExp: Reference to pointer to actual position in match
/// \param pCompare: Value to compare with
/// \return \c bool: Result (true: match)
/// \pre
///     - \c pActRegExp, \c pCompare ASCIIZ-string, not NULL;
///     - \c *pActRegExp not NULL
//----------------------------------------------------------------------------
bool RegularExpression::compGroup (const char*& pActRegExp,
                                   const char*& pCompare) {
   Check1 (pActRegExp); Check1 (*pActRegExp); Check1 (pCompare);
   const char* pEnd = findEndOfGroup (pActRegExp);
   Check1 (pEnd); Check1 (pEnd > pActRegExp);

   ++cGroups;

   std::string group (pActRegExp, pEnd - pActRegExp);
   pActRegExp = group.c_str ();

   TRACE3 ("RegularExpression::compGroup (const char*&, const char*&) -> "
           << pCompare << " in (" << group << ')');

   unsigned int min, max;
   const char* pEndRE = getRepeatFactor (pEnd + 1, min, max);
   TRACE7 ("RegularExpression::compGroup (const char*&, const char*&) - "
           "Repeating group: " << min << " - " << max);

   Check3 (min <= max);

   return doCompGroup (pActRegExp, pEndRE, pCompare, min, max);
}

//----------------------------------------------------------------------------
/// Checks if the passed string matches the group.
/// \param pActPos: Reference to pointer to actual position in match
/// \param pEnd: Pointer to end of the region in the regular expression
/// \param pCompare: Pointer to group to compare
/// \param min: Minimal number of matches
/// \param max: Maximal number of matches
/// \return \c bool: Result (true: match)
/// \pre
///   - \ pActRegExp, \c pCompare ASCIIZ-string, not NULL;
///   - \c *pActRegExp not NULL
///   - \c min <= \c max
//----------------------------------------------------------------------------
bool RegularExpression::doCompGroup (const char*& pActRegExp, const char* pEnd,
                                     const char*& pCompare, unsigned int min,
                                     unsigned int max) {
   Check1 (pActRegExp); Check1 (*pActRegExp); Check1 (pCompare);
   Check1 (min <= max);

   TRACE8 ("RegularExpression::doCompGroup (const char*&, const char*, "
           "const char*&, unsigned int, unsigned int) - "
           "Checking for " << (min ? "mandatory" : "optional") << " (max. "
           << (min ? min : max) << ')');

   const char* pSaveComp = pCompare;
   const char* pSaveRE;
   const char* pStartRE = pActRegExp;
   if (!min) {
      const char* pTempEnd = pEnd;
      // Check if part after group is matching
      if (compareParts (pTempEnd, pCompare, true)) {
         pActRegExp = pTempEnd;
         return true;
      }
   }

   while (*pActRegExp && max >=1) {            // While alternatives available
      pSaveRE = pActRegExp;

      pCompare = pSaveComp;
      if (doCompare (pActRegExp, pCompare)) {         // Compare it; if found:
         TRACE1 ("RegularExpression::doCompGroup (const char*&, const char*&)"
                 " - Found from " << pSaveComp << "; Remaining: '" << pCompare << '\'');

         Check3 (pSaveComp < pCompare);
         std::string value (pSaveComp, pCompare - pSaveComp);
         TRACE8 ("RegularExpression::doCompGroup (const char*&, const char*, "
                 "const char*&, unsigned int, unsigned int) - "
                 " Storing found value[" << cGroups - 1 << "] = '" << value << '\'');

         Check3 (cGroups);
         if (groupValues.size () < cGroups) {
            Check3 (groupValues.size () == (cGroups - 1));
            groupValues.push_back (value);
         }
         else
            groupValues[cGroups - 1] = value;

         pActRegExp = pStartRE;                // Check if matches til minimum
         if (doCompGroup (pActRegExp, pEnd, pCompare,
                          min ? min - 1 : 0, max > 0 ? max - 1 : max))
            return true;
      } // endif group-alternative matches

      pCompare = pSaveComp;

      // Else: Last alternative didn't match -> Try with next one
      if ((pActRegExp = findEndOfAlternative (pSaveRE, true)) == 0)
         break;
      ++pActRegExp;
   }

   pActRegExp = pStartRE;
   return false;
}

//----------------------------------------------------------------------------
/// Checks if the passed string matches the single character (incl\. handling
/// of the repeat-factor).
/// \param pActPos: Reference to pointer to actual position in match
/// \param pCompare: Pointer to group to compare
/// \return \c bool: Result (true: match)
/// \pre \c pActRegExp, pCompare ASCIIZ-string
//----------------------------------------------------------------------------
bool RegularExpression::compChar (const char*& pActRegExp,
                                  const char*& pCompare) {
   Check1 (pActRegExp); Check1 (*pActRegExp); Check1 (pCompare);

   return compActREPart (&RegularExpression::doCompChar, pActRegExp,
                         pActRegExp + 1, pCompare);
}

//----------------------------------------------------------------------------
/// Checks if the passed string matches the single character.
/// \param pActPos: Reference to pointer to actual position in match
///     - Pointer to end of the region in the regular expression
/// \param pCompare: Pointer to group to compare
/// \return \c bool: Result (true: match)
/// \pre \c pActRegExp, pEnd, pCompare ASCIIZ-string; pEnd > pActRegExp
//----------------------------------------------------------------------------
bool RegularExpression::doCompChar (const char*& pActRegExp, const char*,
                                    const char*& pCompare) {

   Check1 (pActRegExp); Check1 (*pActRegExp); Check1 (pCompare);
   TRACE3 ("RegularExpression::doCompChar (const char*&, const char*, const char*&) -> "
           << *pActRegExp << " == " << *pCompare);

   switch (*pActRegExp) {
   case SINGLEMATCH:
      break;

   case LINEBEGIN:
      Check3 (pActRegExp >= pStartCompare);
      if ((pActRegExp == pStartCompare) || (pActRegExp[-1] == '\n'))
         break;
      return false;

   case LINEEND:
      if (!pActRegExp[1] || (pActRegExp[1] == '\n'))
         break;
      return false;

   default:
      if (*pCompare == *pActRegExp)
         break;
      return false;
   } // end-switch

   ++pCompare;
   return true;
}

//----------------------------------------------------------------------------
/// Checks if the passed string matches the single character (incl\.
//            handling of the repeat-factor).
/// \param pActPos: Reference to pointer to actual position in match
/// \param region: Region to compare
/// \return \c bool: Result (true: match)
/// \pre \c pActRegExp, pCompare ASCIIZ-string
//----------------------------------------------------------------------------
bool RegularExpression::compEscChar (const char*& pActRegExp,
                                     const char*& pCompare) {
   Check1 (pActRegExp); Check1 (*pActRegExp); Check1 (pCompare);

   return compActREPart (&RegularExpression::doCompEscChar, pActRegExp,
                         pActRegExp + 1, pCompare);
}

//----------------------------------------------------------------------------
/// Checks if the passed string matches the single (escaped) character
/// \param pActPos: Reference to pointer to actual position in match
//  \param pEnd: Pointer to regexp after char (incl. repeat-factor)
/// \param ch: Character to compare
/// \return \c bool: Result (true: match)
/// \pre \c pActRegExp, pEnd, pCompare ASCIIZ-string; pEnd > pActRegExp
//----------------------------------------------------------------------------
bool RegularExpression::doCompEscChar (const char*& pActRegExp, const char* pEnd,
                                       const char*& pCompare) {

   Check1 (pActRegExp); Check1 (*pActRegExp); Check1 (pCompare); Check1 (pEnd);
   Check1 (pEnd > pActRegExp);
   TRACE3 ("RegularExpression::doCompEscChar (const char*&, const char*&) -> "
           << *pActRegExp << " == " << *pCompare);

   switch (*pActRegExp) {
   case WORD:
      if (isWordConstituent (*pCompare))
         break;
      return false;

   case NOTWORD:
      if (!isWordConstituent (*pCompare))
         break;
      return false;

   case WORDBORDER:
      return isWordBorder (pCompare);

   case NOTWORDBORDER:
      return !isWordBorder (pCompare);

   case WORDBEGIN:
      return isWordBeginn (pCompare);

   case WORDEND:
      return isWordEnd (pCompare);

   default:
      if (isdigit (*pActRegExp)) {
         unsigned int group (*pActRegExp - '1');
         if (group == -1U)
            group = 10;

         TRACE8 ("RegularExpression::doCompEscChar (const char*&, const char*&) -> "
                 "Group " << group << " = '"
                 << (cGroups > group ? groupValues[group] : "") << '\'');

         const char* pHelp = cGroups > group ? groupValues[group].c_str () : "";
         return doCompare (pHelp, pCompare);
      }
      return doCompChar (pActRegExp, pEnd, pCompare);
   } // end-switch

   ++pCompare;
   return true;
}

//----------------------------------------------------------------------------
/// Checks if the actual position is at a word-border
/// \param pCompare: String to inspect
/// \return \c bool
/// \pre \c pCompare not NULL
//----------------------------------------------------------------------------
bool RegularExpression::isWordBorder (const char* pCompare) const {
   Check1 (pCompare);
   Check1 (pCompare >= pStartCompare);
   return ((pStartCompare == pCompare) || !pCompare[1]
           || ((isWordConstituent (*pCompare)) != isWordConstituent (pCompare[1]))
           || ((isWordConstituent (*pCompare)) != isWordConstituent (pCompare[-1])));
}

//----------------------------------------------------------------------------
/// Checks if the actual position is at the beginning of a word.
/// \param pCompare: String to inspect
/// \return \c bool
/// \pre \c pCompare not NULL
//----------------------------------------------------------------------------
bool RegularExpression::isWordBeginn (const char* pCompare) const {
   Check1 (pCompare);
   Check1 (pCompare >= pStartCompare);
   return ((pCompare == pStartCompare)
           || (isWordConstituent (*pCompare) && !isWordConstituent (pCompare[-1])));
}


//----------------------------------------------------------------------------
/// Checks if the actual position is at the end of a word.
/// \param pCompare: String to inspect
/// \return \c bool
/// \pre \c pCompare not NULL
//----------------------------------------------------------------------------
bool RegularExpression::isWordEnd (const char* pCompare) const {
   Check1 (pCompare);
   Check1 (pCompare >= pStartCompare);
   return ((pCompare != pStartCompare)
           && (!isWordConstituent (*pCompare) && isWordConstituent (pCompare[-1])));
}

//----------------------------------------------------------------------------
/// Finds the end of a region.
/// \param pRegExp: Pointer to the beginning of the region (*after* bracket)
/// \return \c const char*: Pointer to end of region (points to closing bracket)
/// \pre \c pRegExp not NULL, \c pRegExp[-1] == REGIONBEGIN
//----------------------------------------------------------------------------
const char* RegularExpression::findEndOfRegion (const char* pRegExp) const {
   Check1 (pRegExp); Check1 (pRegExp[-1] == REGIONBEGIN);

   switch (*pRegExp) {                    // Skip leading "special" characters
   case NEGREGION1:
   case NEGREGION2:
   case REGIONEND:
      ++pRegExp;
   } // end-switch

   if (*pRegExp == REGIONEND)
      ++pRegExp;

   // Search for end-of-region, with regard of region-classes ([:xxx:])
   for (bool bClass = false; *pRegExp != REGIONEND; ++pRegExp) {
      TRACE9 ("RegularExpression::findEndOfRegion (const char*) const "
              "- Search for region-end: " << *pRegExp);

      if (*pRegExp == REGIONCLASS) {
	 if (pRegExp[-1] == REGIONBEGIN)
	    bClass = true;
	 else
	    if (pRegExp[1] == REGIONEND)
	       if (!bClass)                 // If no class left -> Exit anyway
		  break;
               else
                  bClass = false;
	 ++pRegExp;
      } // endif region-class found
   } // end-for region-end found

   return pRegExp;
}

//----------------------------------------------------------------------------
/// Finds the end of a group.
/// \param pRegExp: Pointer to the beginning of the group (*after* parent.)
///
/// \return \c const char*: Pointer to end of group (points to closing parent.)
/// \pre \c pRegExp not \c NULL, \c pRegExp[-1] == GROUPBEGIN
//----------------------------------------------------------------------------
const char* RegularExpression::findEndOfGroup (const char* pRegExp) const {
   Check1 (pRegExp); Check1 (pRegExp[-1] == GROUPBEGIN);

   int cGroups_ = 1;
   do {
      if (*pRegExp == GROUPEND)
         --cGroups_;
      else if (*pRegExp++ == GROUPBEGIN)
         ++cGroups_;
   } while (cGroups_); // end-do

   TRACE9 ("RegularExpression::findEndOfGroup (const char*) const - End: " << pRegExp + 1);
   return pRegExp;
}

//----------------------------------------------------------------------------
/// Finds the end of an alternative.
/// \param pRegExp: Pointer to the beginning of the regexp
/// \param inGroup: Flag, if we are searching only inside a group
/// \return \c const char*: Pointer to alternative-seperator (or NULL, if no
//                         alternative found)
/// \pre \c pRegExp not \c NULL and at least 1 char long
//----------------------------------------------------------------------------
const char* RegularExpression::findEndOfAlternative (const char* pRegExp,
                                                     bool inGroup) const {
   Check1 (pRegExp);
   if (!*pRegExp)
      return NULL;

   // Search for alternative with attention of regions
   while (*++pRegExp) {
      TRACE9 ("RegularExpression::findEndOfAlternative (const char*) const - Search for alternative: "
              << pRegExp);

      switch (*pRegExp) {
      case REGIONBEGIN:
         if (pRegExp[-1] != ESCAPE)
            pRegExp = findEndOfRegion (pRegExp + 1);
         break;

      case GROUPBEGIN:
         if (pRegExp[-1] != ESCAPE)
            pRegExp = findEndOfGroup (pRegExp + 1);
         break;

      case ALTERNATIVE:
 	 if (pRegExp[-1] != ESCAPE)
            return pRegExp;
         break;

      case GROUPEND:
         if (inGroup)
            return NULL;
      } // end-switch act. char
   } // end-while regexp not empty

   return NULL;
}

//----------------------------------------------------------------------------
/// Compares the acutal RE-part according to the repeat-factor.
/// \param fnCompare: Member for comparing the regular expression
/// \param pActRegExp: Pointer to regular expression
/// \param pEnd: Pointer to end of the region in the regular expression
/// \param pCompare: String to compare
/// \return \c bool: True, if matches; false, if not
/// \pre All Pointers not NULL; \c pEndRE >= \c pActRegExp
//----------------------------------------------------------------------------
bool RegularExpression::compActREPart (MFCOMPARE fnCompare, const char*& pActRegExp,
                                       const char* pEndRE, const char*& pCompare) {
   Check1 (fnCompare); Check1 (pActRegExp); Check1 (pEndRE);
   Check1 (pEndRE >= pActRegExp); Check1 (pCompare);

   TRACE8 ("RegularExpression::compActREPart (MFCOMPARE, const char*&, const char*,"
           " const char*&) - Analyzing: '" << std::string (pActRegExp, pEndRE - pActRegExp)
           << '\'');

   unsigned int min, max;
   pEndRE = getRepeatFactor (pEndRE, min, max);     // Check, RE repeat-factor
   Check3 (min <= max);

   TRACE3 ("RegularExpression::compActREPart (MFCOMPARE, const char*&, const char*,"
           " const char*&) - Repeating: " << min << " - " << max);

   unsigned int i;
   for (i = 1; i <= min; ++i) {
      TRACE3 ("RegularExpression::compActREPart (MFCOMPARE, const char*&, const char*,"
              " const char*&) - Repeating: " << i << ". mandatory");

      if (!(this->*fnCompare) (pActRegExp, pEndRE, pCompare)) // Match til min
         return false;
   }

   const char* pSaveEnd = pEndRE;
   const char* pSaveComp = pCompare;
   for (; i <= max; ++i) {                               // then (til max) try
      if (!*pCompare) {
         if (*pEndRE)
            return false;
         else
            break;
      }

      TRACE3 ("RegularExpression::compActREPart (MFCOMPARE, const char*&, const char*,"
              " const char*&) - Repeating: " << i << ". optional");

      if (compareParts (pEndRE, pCompare))        // If remaining parts match;
         break;                                        // If yes: Return found

      pEndRE = pSaveEnd;
      pCompare = pSaveComp;                        // Else: Check if act. part
      if ((this->*fnCompare) (pActRegExp, pEndRE, pCompare)) {     // is valid
         pSaveComp = pCompare;                             // and repeat proc.
      }
      else
         return false;
   }

   TRACE8 ("RegularExpression::compActREPart (MFCOMPARE, const char*&, const char*,"
           " const char*&) - Found:\n\t->ActRegExp = '" << pActRegExp << "', pEnd = '"
           << pEndRE << "', pCompare = " << pCompare);

   pActRegExp = pEndRE;
   return true;
 }


//----------------------------------------------------------------------------
/// Checks how often the last regular expression must be repeated.
/// \param pRE: Ptr. to regular expression
/// \param min: Minimal number, how often the previous part of the regular
///               expression must be repeated
/// \param max: Maximal number
/// \return \c const char*: End of repeat-factor (char behind)
/// \pre \c pRE, \c ASCIIZ-string; not \c NULL
//----------------------------------------------------------------------------
const char* RegularExpression::getRepeatFactor (const char* pRE, unsigned int& min,
                                                unsigned int& max) const {
   Check1 (pRE);

   TRACE3 ("RegularExpression::getRepeatFactor (const char*, unsigned int&, "
           "unsigned int&) const - Checking: " << pRE);

   char* pEnd = const_cast<char*> (pRE);

   switch (*pRE) {
   case MULTIMATCHOPT:
      min = 0;
      max = -1U;
      ++pEnd;
      break;

   case MULTIMATCHMAND:
      min = 1;
      max = -1U;
      ++pEnd;
      break;

   case MULTIMATCH1:
      min = 0;
      max = 1;
      ++pEnd;
      break;

   case BOUNDBEG:
      if (isdigit (pRE[1])) {
         min = static_cast<int> (strtoul (pRE + 1, &pEnd, 10));
         max = min;
         if (*pEnd == ',')
            max = isdigit (*++pEnd) ? strtoul (pEnd, &pEnd, 10) : -1U;
         ++pEnd;
      }
      else
         min = max = 1;
      break;

   default:
      min = max = 1;
      break;
   } // end-switch

   return pEnd;
}
#endif

//----------------------------------------------------------------------------
/// Checks the syntax of the regular expression. If everything is OK, 0 is
/// returned; 1 if there is no regular expression at all. In case of any
/// other error an exception is thrown.
/// \return \c int: Status; 0: OK
/// \throw std::string: In case of an invalid regexp a describing text
/// \pre \c pFileRegExp is a valid regexp
//----------------------------------------------------------------------------
int RegularExpression::checkIntegrity () const throw (std::string) {
#ifndef HAVE_REGEX_H
   const char* pRegExp = getExpression ();
   if (!pRegExp)
      return 1;

   const char* pPrevExpr = NULL;
   int cGroups_ = 0;

   while (*pRegExp) {
      switch (*pRegExp) {
      case  REGIONBEGIN:
         if (!*++pRegExp)
            throw (getError (RANGE_OPEN, pRegExp - getExpression ()));

         // Skip leading range-inversion
         if ((*pRegExp == NEGREGION1) || (*pRegExp == NEGREGION2))
            ++pRegExp;

         if (*pRegExp == REGIONEND)      // Skip leading end-of-range charcter
            ++pRegExp;

         while (*pRegExp != REGIONEND) {
            switch (*pRegExp) {
            case RANGE:
               if (!pRegExp[1] || (pRegExp[1] == REGIONEND))
                  throw (getError (REGION_OPEN, pRegExp - getExpression ()));

               if (pRegExp[-1] > pRegExp[1])
                  throw (getError (INV_RANGE, pRegExp - getExpression ()));
               break;

            case '\0':
               throw (getError (REGION_OPEN, pRegExp - getExpression ()));
            } // endif

            ++pRegExp;
         } // end-while
         pPrevExpr = pRegExp;
         break;

      case GROUPBEGIN:
         ++cGroups_;
         break;

      case GROUPEND:
         --cGroups_;
         break;

      case ESCAPE:
         if (!pRegExp[1])
            throw (getError (ENDING_BACKSLASH, pRegExp - getExpression ()));

         if (isdigit (pRegExp[1]) && ((pRegExp[1] - '1') > cGroups_))
            throw (getError (INV_DIGIT, pRegExp - getExpression ()));

         ++pRegExp;
         break;

      case MULTIMATCHOPT:
      case MULTIMATCHMAND:
      case MULTIMATCH1:
         if (!pPrevExpr)
            throw (getError (NO_PREV_EXP, pRegExp - getExpression ()));
         pPrevExpr = NULL;
         break;

      case BOUNDBEG:
         if (isdigit (pRegExp[1])) {        // Check if bound or just '{' found
            if (!pPrevExpr)
               throw (getError (NO_PREV_EXP, pRegExp - getExpression ()));

	    char* pEnd;
            unsigned long min (strtoul (pRegExp + 1, &pEnd, 10));
            unsigned long max (min);
            if (*pEnd == ',')
               max = isdigit (*++pEnd) ? strtoul (pEnd, &pEnd, 10) : -1UL;

            TRACE7 ("RegularExpression::checkIntegrity () const - Bound: " << min
                    << '/' << max);
            if (min > max)
               throw (getError (INV_BOUND, pRegExp - getExpression () + 1));

            pRegExp = pEnd;
	    if (*pRegExp != BOUNDEND)
               throw (getError (INV_BOUND, pRegExp - getExpression ()));
         } // endif bound found
         break;

      default:
         pPrevExpr = pRegExp;
      } // end-switch

      ++pRegExp;
   } // end-while

   if (cGroups_)
      throw (getError (GROUP_OPEN, 0));

#endif
   return 0;
}

//----------------------------------------------------------------------------
/// Builds the error-string for checkIntegrity ()
/// \param rc: Occured error
/// \param pos: Position of the error inside the regular expression
/// \return \c std::string: Text describing error in human-readable format
//----------------------------------------------------------------------------
std::string RegularExpression::getError (int rc, unsigned int pos) const {
#ifdef HAVE_REGEX_H
   char error[256];
   regerror (rc, &regexp, error, sizeof (error));
#else
   const char* error;

   switch (rc) {
   case REGION_OPEN: error = N_("Unmatched [ or [^"); break;
   case GROUP_OPEN: error = N_("Unmatched ( or )"); break;
   case RANGE_OPEN: error = N_("Invalid range end"); break;
   case NO_PREV_EXP: error = N_("Repeating suffix without previous expression"); break;
   case INV_DIGIT: error = N_("Invalid group-number"); break;
   case INV_RANGE: error = N_("Invalid range (lower border larger than upper border)"); break;
   case INV_BOUND: error = N_("Invalid bound"); break;
   case ENDING_BACKSLASH: error = N_("Regular expression ends with escape-character (\\)"); break;
   default: error = N_("Unknown error"); break;
   } // end-switch
#endif

   std::string err (_("`%1', position %2: %3"));
   err.replace (err.find ("%1"), 2, getExpression ());
   err.replace (err.find ("%2"), 2, ANumeric::toString ((unsigned long)pos + 1));
   err.replace (err.find ("%3"), 2, error);

   TRACE1 ("RegularExpression::getError (int, unsinged int): " << err);
   return err;
}


#ifdef HAVE_REGEX_H
//----------------------------------------------------------------------------
/// Initializes the regular-expression
/// \param pRegExp: Pointer to ASCIIZ-string holding regexp
/// \pre \c pRegExp is an ASCIIZ-string (not NULL)
//----------------------------------------------------------------------------
void RegularExpression::init (const char* pRegExp) throw (std::string) {
   Check1 (pRegExp);

   int rc = regcomp (&regexp, pRegExp, REG_EXTENDED);
   if (rc)
      throw (getError (rc, 0));
   }
#endif

}

