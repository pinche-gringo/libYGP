//$Id: CRegExp.cpp,v 1.17 2002/04/18 03:53:29 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RegularExpression
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.17 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.5.2000
//COPYRIGHT   : Anticopyright (A) 2000, 2001, 2002

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


#define DEBUG 9
#include "Trace_.h"
#include "CRegExp.h"
#include "Internal.h"
#include "ANumeric.h"

#ifndef HAVE_REGEX_H
#  include <stdlib.h>
#  include <ctype.h>
#  define isclass(type,str,len,ch) (strncmp ((str), #type, (len)) ? 0 : (is##type (ch) ? 2 : 1))
#endif


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: pRegExp: Pointer to regular expression
/*--------------------------------------------------------------------------*/
RegularExpression::RegularExpression (const char* pRegExp) throw (std::string)
   : IRegularExpression (pRegExp), pStartCompare (NULL) {
#ifdef HAVE_REGEX_H
   init (pRegExp);
#else
   checkIntegrity (); 
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
RegularExpression::~RegularExpression () {
#ifdef HAVE_REGEX_H
   regfree (&regexp);
#endif
}


/*--------------------------------------------------------------------------*/
//Purpose   : Changes the regular expression to check
//Parameters: pRegExp: Pointer to regular expression
/*--------------------------------------------------------------------------*/
RegularExpression& RegularExpression::operator= (const char* pRegExp) {
   IRegularExpression::operator= (pRegExp);
#ifdef HAVE_REGEX_H
   regfree (&regexp);
   init (pRegExp);
#else
   checkIntegrity ();
#endif
   return *this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the regular expression
//Parameters: pAktRegExp: Pointer to regular expression
//            pCompare: String to compare
//Returns   : bool: Result (true: match)
//Requires  : pAktRegExp, pCompare: ASCIIZ-string
/*--------------------------------------------------------------------------*/
bool RegularExpression::compare (const char* pAktRegExp, const char* pCompare) {
   assert (pAktRegExp); assert (pCompare); assert (!checkIntegrity ());

   TRACE1 ("RegularExpression::compare (const char*, const char*) -> "
           << pAktRegExp << " <-> " << pCompare);

#ifdef HAVE_REGEX_H
   // Use system-regular expressions if available
   regmatch_t match[1];
   int ret (regexec (&regexp, pCompare, sizeof (match) / sizeof (match[0]), match, 0));
   if (ret < 0)
      throw (getError (rc, 0));

   TRACE5 ("RegularExpression::compare (const char*, const char*) -  "
           "Subexpr.: " << match[0].rm_so << " - " << match[0].rm_eo);

   
   return ret ? false
              : (match[0].rm_so == 0) && (match[0].rm_eo == strlen (getExpression ()));
#else
   pStartCompare = pCompare;
   bool rc (compareParts (pAktRegExp, pCompare));
   pStartCompare = NULL;
   return rc ? !*pCompare : false;
#endif
}

#ifndef HAVE_REGEX_H

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the regular expression; the
//            regular expression is seperated in various parts (if neccessary)
//Parameters: pAktRegExp: Pointer to regular expression
//            pCompare: String to compare
//Returns   : bool: Result (true: match)
//Requires  : pAktRegExp, pCompare: ASCIIZ-string
/*--------------------------------------------------------------------------*/
bool RegularExpression::compareParts (const char* pAktRegExp,
                                      const char*& pCompare, bool inGroup) {
   assert (pAktRegExp); assert (pCompare); assert (!checkIntegrity ());

   TRACE1 ("RegularExpression::compareParts (const char*, const char*&) -> "
           << pAktRegExp << " <-> " << pCompare);

   pStartCompare = pCompare;
   bool rc (false);

   do {
      rc = doCompare (pAktRegExp, pCompare);         // Compare the whole thing
      if (rc) {                               // If matched and both regexp and
         TRACE1 ("RegularExpression::compareParts (const char*, const char*&) -> Remaining: '"
                 << pCompare << '\'');

         if (!*pCompare)                    // string to compare empty -> Found
            break;
         else {                            // Part found, but remaining string:
            if (inGroup)                      // If in a group let caller check
               return true;                 // with its rest; if not in a group
            else                                // check remaining alternatives
               pCompare = pStartCompare;
         }
      }
      else                             // Not found: Search for alternative and
         pCompare = pStartCompare;  // start comparison again (with old values)

      rc = false;
      pAktRegExp = findEndOfAlternative (pAktRegExp + 1);
   } while (*pAktRegExp++); // end-do while alternatives available

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the regular expression
//Parameters: pAktRegExp: Pointer to regular expression
//            pCompare: String to compare
//Returns   : bool: Result (true: match)
//Requires  : pAktRegExp, pCompare: ASCIIZ-string
/*--------------------------------------------------------------------------*/
bool RegularExpression::doCompare (const char* pAktRegExp, const char*& pCompare) {
   assert (pAktRegExp); assert (pCompare); assert (!checkIntegrity ());
   TRACE1 ("RegularExpression::doCompare (const char*, const char*&) -> "
           << pAktRegExp << " <-> " << pCompare);

   char ch;
   bool match (false);

   while ((ch = *pAktRegExp)) {
      // Check actual rex-exp for special characters (at the current position)
      // After this switch the the following conditions must be fulfilled:
      // - fnCompare points to the compare-function to use
      // - pEnd points to the end of the part of the reg-exp belonging to the
      //   current position (like end of group, ...)
      // - lastExpr contains the actual part of the regexp
      switch (ch) {                                   // Get current expression
      case REGIONBEGIN:
         TRACE8 ("RegularExpression::doCompare (const char*&, const char*&) - Found region");
         match = RegularExpression::compRegion (++pAktRegExp, pCompare);
         break;

      case GROUPBEGIN:
         TRACE8 ("RegularExpression::doCompare (const char*&, const char*&) - Found group");
         match = RegularExpression::compGroup (++pAktRegExp, pCompare);
         break;

      case ESCAPE:
         match = RegularExpression::compEscChar (pAktRegExp, pCompare);
         break;

      case ALTERNATIVE:
         TRACE4 ("RegularExpression::doCompare (const char*, const char*&) - Alternative OK");
         return true;                   // OK til act. pos -> Return true

      default:
         assert (ch); assert (ch != MULTIMATCHMAND);
         assert (ch != MULTIMATCH1); assert (ch != MULTIMATCHOPT);
         TRACE4 ("RegularExpression::doCompare (const char*, const char*&) - Found char");

         match = RegularExpression::compChar (pAktRegExp, pCompare);
      } // end-switch 

      if (match) {
         if (!*pAktRegExp)
            return true;
      }
      else
         return false;
   } // end-while

   TRACE2 ("RegularExpression::doCompare (const char*, const char*&) - Found: "
           << static_cast <const char*> (ch ? "False": "True"));
   return !ch;                                    // Match OK, if regexp empty
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the region (incl. handling
//            of the repeat-factor)
//Parameters: pAktPos: Reference to pointer to actual position in match
//            region: Region to compare
//Returns   : bool: Result (true: match)
//Requires  : pAktPos ASCIIZ-string; region not empty
/*--------------------------------------------------------------------------*/
bool RegularExpression::compRegion (const char*& pAktRegExp,
                                    const char*& pCompare) {
   assert (pAktRegExp); assert (*pAktRegExp); assert (pCompare);

   return compActREPart (&RegularExpression::doCompRegion, pAktRegExp,
                         findEndOfRegion (pAktRegExp), pCompare);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the region
//Parameters: pAktPos: Reference to pointer to actual position in match
//            region: Region to compare
//Returns   : bool: Result (true: match)
//Requires  : pAktPos ASCIIZ-string; region not empty
/*--------------------------------------------------------------------------*/
bool RegularExpression::doCompRegion (const char*& pAktRegExp, const char* pEnd,
                                      const char*& pCompare) const {
   assert (pAktRegExp); assert (*pAktRegExp); assert (pCompare);
   assert (pEnd); assert (pEnd > pAktRegExp);

   ++pAktRegExp;                                          // Point to start and

   TRACE3 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) -> "
           << *pCompare << " in [" << std::string (pAktRegExp, pEnd - pAktRegExp) << ']');


   // Compares the actual file-char with the region
   bool fNeg (false);
   if (*pAktRegExp == NEGREGION) {                            // Values to invert?
      ++pAktRegExp;
      fNeg = true;
   } // endif

   char ch (*pAktRegExp);

   do {
      if (pAktRegExp[1] == RANGE) {
         char chUpper (pAktRegExp[2]);
         assert (chUpper != '\0');  assert (chUpper != REGIONEND);

         TRACE7 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - Check "
                 << *pCompare << " in [" << ch << '-' << chUpper << ']');
         if ((*pAktRegExp >= ch) && (*pCompare <= chUpper))
            break;

         pAktRegExp += 2;
      } // endif range found
      else {
         // Check for class of characters or ordinary char
         char* pEndClass = NULL;

         if ((ch == REGIONBEGIN) && (pAktRegExp[1] == REGIONCLASS)
             && ((pEndClass = strchr (pAktRegExp + 2, REGIONCLASS)) != NULL)
             && (pEndClass[1] == REGIONEND)) {
	    TRACE7 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - Check "
                    << *pCompare << " against region-class " << pAktRegExp + 2);

            int temp (0), len (pEndClass - pAktRegExp - 2);
            int val ((temp = isclass (alnum, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (temp = isclass (alpha, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (temp = isclass (digit, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (temp = isclass (space, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (temp = isclass (cntrl, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (temp = isclass (graph, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (temp = isclass (print, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (temp = isclass (punct, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (temp = isclass (upper, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (temp = isclass (lower, pAktRegExp + 2, len, *pCompare)) ? temp :
		     (isclass (xdigit, pAktRegExp + 2, len, *pCompare)));
            if (val == 2) {
               TRACE8 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - Check "
                       << *pCompare << " matches region-class " << pAktRegExp);
               break;
            } // endif class and input matches
            else {
               TRACE8 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - " 
                       << *pCompare << " doesn't match region-class " << pAktRegExp);
               if (val)
                  pAktRegExp = pEndClass + 1;
            } // end-else class found, but input doesn't match
         } // endif
         else {
            TRACE7 ("RegularExpression::doCompRegion (cont char*&, const char*, const char*&) - Check "
                    << *pCompare << " == " << ch);
            if (ch == *pCompare)
               break;
         } // end-else ordinary character
      } // end-else non-range

      ch = *++pAktRegExp;
   } while (pAktRegExp <= pEnd); // end-do

   if ((ch != '\0') == fNeg)
      return false;

   ++pCompare;
   return true;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the group
//Parameters: pAktRegExp: Reference to pointer to actual position in match
//            pCompare: Value to compare with
//Returns   : bool: Result (true: match)
//Requires  : pAktPos ASCIIZ-string; group not empty
/*--------------------------------------------------------------------------*/
bool RegularExpression::compGroup (const char*& pAktRegExp,
                                   const char*& pCompare) {
   assert (pAktRegExp); assert (*pAktRegExp); assert (pCompare);
   const char* pEnd (findEndOfGroup (++pAktRegExp));
   assert (pEnd); assert (pEnd > pAktRegExp);

   std::string group (pAktRegExp, pEnd - pAktRegExp - 1);

   TRACE3 ("RegularExpression::compGroup (const char*&, const char*&) -> "
           << pAktRegExp << " in \\(" << group << "\\)");

   int min, max;
   pEnd = getRepeatFactor (++pEnd, min, max);
   assert (min >= 0);
   assert (static_cast<unsigned int> (min) <= static_cast<unsigned int> (max));

   unsigned int i;
   for (i = 0; i <= min; ++i)
      if (!doCompGroup (pAktRegExp, pAktRegExp + group.size (),   // Match min
                        pCompare))
         return false;

   const char* pHelp = group.c_str ();
   const char* pSaveComp (pCompare);

   bool match;
   for (; i = static_cast<unsigned int> (max); ++i) {       // Check til. max.
      if (match = compareParts (pHelp, pCompare, true)) { // If grouppart fits
         if (*pCompare) {                                 // If fits with rest
            if (compareParts (pEnd, pCompare))         // Compare with rest-RE 
               return true;                                // Fits also: Found

            pHelp = findEndOfAlternative (pHelp);
            if (!*pHelp)
               return false;
         }
      }
      return false;
   } // endfor til max

   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the group
//Parameters: pAktRegExp: Reference to pointer to actual position in match
//            pEnd: Pointer to end of group
//            pCompare: Value to compare with
//Returns   : bool: Result (true: match)
//Requires  : pAktPos ASCIIZ-string; group not empty
/*--------------------------------------------------------------------------*/
bool RegularExpression::doCompGroup (const char*& pAktRegExp, const char* pEnd,
                                     const char*& pCompare) {
   assert (pAktRegExp); assert (*pAktRegExp); assert (pCompare);
   assert (pEnd); assert (pEnd > pAktRegExp);

   TRACE3 ("RegularExpression::doCompGroup (const char*&, const char*, const char*&) const -> "
           << pAktRegExp << " in (" << std::string (pAktRegExp, pEnd - pAktRegExp) << ")");
   return compareParts (pAktRegExp, pCompare);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the single character (incl.
//            handling of the repeat-factor)
//Parameters: pAktPos: Reference to pointer to actual position in match
//            pCompare: Value to compare with
//Returns   : bool: Result (true: match)
//Requires  : pAktPos ASCIIZ-string; region not empty
/*--------------------------------------------------------------------------*/
bool RegularExpression::compChar (const char*& pAktRegExp,
                                    const char*& pCompare) {
   assert (pAktRegExp); assert (*pAktRegExp); assert (pCompare);

   return compActREPart (&RegularExpression::doCompChar, pAktRegExp,
                         pAktRegExp + 1, pCompare);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the single character
//Parameters: pAktPos: Reference to pointer to actual position in match
//            pEnd: Pointer to repeat-factor (if any)
//            pCompare: Pointer to character to compare
//Returns   : bool: Result (true: match)
//Requires  : pAktPos ASCIIZ-string; ch has length of 1
/*--------------------------------------------------------------------------*/
bool RegularExpression::doCompChar (const char*& pAktRegExp, const char* pEnd,
                                    const char*& pCompare) const {

   assert (pAktRegExp); assert (*pAktRegExp); assert (pCompare); assert (pEnd);
   assert (pEnd > pAktRegExp);
   TRACE3 ("RegularExpression::doCompChar (const char*&, const char*&) -> "
           << *pAktRegExp << " == " << *pCompare);

   switch (*pCompare) {
   case SINGLEMATCH:
      break;

   case LINEBEGIN:
      assert (pAktRegExp >= pStartCompare);
      if ((pAktRegExp == pStartCompare) || (pAktRegExp[-1] == '\n'))
         break;
      return false;

   case LINEEND:
      if (!pAktRegExp[1] || (pAktRegExp[1] == '\n'))
         break;
      return false;

   default:
      if (*pCompare == *pAktRegExp)
         break;
      return false;
   } // end-switch 

   ++pAktRegExp;
   ++pCompare;
   return true;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the single character (incl.
//            handling of the repeat-factor)
//Parameters: pAktPos: Reference to pointer to actual position in match
//            region: Region to compare
//Returns   : bool: Result (true: match)
//Requires  : pAktPos ASCIIZ-string; region not empty
/*--------------------------------------------------------------------------*/
bool RegularExpression::compEscChar (const char*& pAktRegExp,
                                     const char*& pCompare) {
   assert (pAktRegExp); assert (*pAktRegExp); assert (pCompare);

   return compActREPart (&RegularExpression::doCompEscChar, pAktRegExp,
                         pAktRegExp + 1, pCompare);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the single (escaped) character
//Parameters: pAktPos: Reference to pointer to actual position in match
//            ch: Character to compare
//Returns   : bool: Result (true: match)
//Requires  : pAktPos ASCIIZ-string; ch has length of 1
/*--------------------------------------------------------------------------*/
bool RegularExpression::doCompEscChar (const char*& pAktRegExp, const char* pEnd,
                                       const char*& pCompare) const {

   assert (pAktRegExp); assert (*pAktRegExp); assert (pCompare); assert (pEnd);
   assert (pEnd > pAktRegExp);
   TRACE3 ("RegularExpression::doCompEscChar (const char*&, const char*&) const -> "
           << *pAktRegExp << " == " << *pCompare);

   switch (*pCompare) {
   case WORD:
      if (isalnum (*pAktRegExp))
         break;
      return false;

   case NOTWORD:
      if (!isalnum (*pAktRegExp))
         break;
      return false;

   case WORDBORDER:
      assert (pAktRegExp >= pStartCompare);
      return ((pAktRegExp == pStartCompare) || !pAktRegExp[1]
              || ((isalnum (*pAktRegExp)) != isalnum (pAktRegExp[1]))
              || ((isalnum (*pAktRegExp)) != isalnum (pAktRegExp[-1])));

   case NOTWORDBORDER:
      assert (pAktRegExp >= pStartCompare);
      return (pAktRegExp != pStartCompare) && pAktRegExp[1]
	&& isalnum (*pAktRegExp) == isalnum (pAktRegExp[1]) && isalnum (pAktRegExp[-1]);

   case WORDBEGIN:
      assert (pAktRegExp >= pStartCompare);
      return ((pAktRegExp == pStartCompare)
              || ((isalnum (*pAktRegExp)) && !isalnum (pAktRegExp[-1])));

   case WORDEND:
      assert (pAktRegExp >= pStartCompare);
      return ((pAktRegExp != pStartCompare)
              && !isalnum (*pAktRegExp) && isalnum (pAktRegExp[-1]));

   default:
      return doCompChar (pAktRegExp, pEnd, pCompare);
   } // end-switch

   assert (0);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Finds the end of a region
//Parameters: pRegExp: Pointer to the beginning of the region (*after* bracket)
//Returns   : const char*: Pointer to end of region (points to closing bracket)
//Requires  : pRegExp not NULL, pRegExp[-1] == REGIONBEGIN
/*--------------------------------------------------------------------------*/
const char* RegularExpression::findEndOfRegion (const char* pRegExp) const {
   assert (pRegExp); assert (pRegExp[-1] == REGIONBEGIN);

   if (*pRegExp == NEGREGION)           // Skip leading "special" characters
      ++pRegExp;
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

/*--------------------------------------------------------------------------*/
//Purpose   : Finds the end of a group
//Parameters: pRegExp: Pointer to the beginning of the group (*after* parent.)
//Returns   : const char*: Pointer to end of group (points to closing parent.)
//Requires  : pRegExp not NULL, pRegExp[-1] == GROUPBEGIN
/*--------------------------------------------------------------------------*/
const char* RegularExpression::findEndOfGroup (const char* pRegExp) const {
   assert (pRegExp); assert (pRegExp[-1] == GROUPBEGIN);

   int cGroups = 1;
   do {
      if (*pRegExp == GROUPEND)
         --cGroups;
      else if (*pRegExp++ == GROUPBEGIN)
         ++cGroups;
   } while (cGroups); // end-do 

   TRACE9 ("RegularExpression::findEndOfGroup (const char*) const - End: " << pRegExp + 1);
   return ++pRegExp;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Finds the end of an alternative
//Parameters: pRegExp: Pointer to the beginning of the regexp
//Returns   : const char*: Pointer to alternative-seperator
//Requires  : pRegExp not NULL, pRegExp >= 1 char
/*--------------------------------------------------------------------------*/
const char* RegularExpression::findEndOfAlternative (const char* pRegExp) const {
   assert (pRegExp); assert (*pRegExp);

   // Search for alternative with attention of regions
   while (*++pRegExp) {
      TRACE9 ("RegularExpression::findEndOfAlternative (const char*) const - Search for alternative: "
              << pRegExp);

      if (*pRegExp == REGIONBEGIN)
         pRegExp = findEndOfRegion (pRegExp + 1);
      else
 	 if ((*pRegExp == ALTERNATIVE) && (pRegExp[-1] != ESCAPE))
            break;
   } // end-while regexp not empty

   return pRegExp;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Compares the acutal RE-part according to the repeat-factor
//Parameters: TODO
//Returns   : bool: True, if matches; false, if not
//Requires  : Pointers not NULL
/*--------------------------------------------------------------------------*/
bool RegularExpression::compActREPart (MFCOMPARE fnCompare, const char*& pAktRegExp,
                                       const char* pEndRE, const char*& pCompare) {
   assert (fnCompare); assert (pAktRegExp); assert (pEndRE); 
   assert (pEndRE >= pAktRegExp); assert (pCompare);

   TRACE8 ("RegularExpression::compActREPart (MFCOMPARE, const char*&, const char*,"
           " const char*&) - Analyzing: '" << std::string (pAktRegExp, pEndRE - pAktRegExp)
           << '\'');

   int min, max;
   pEndRE = getRepeatFactor (pEndRE, min, max);     // Check, RE repeat-factor
   assert (static_cast<unsigned int> (min) <= static_cast<unsigned int> (max));
   assert (min >= 0);

   TRACE3 ("RegularExpression::compActREPart (MFCOMPARE, const char*&, const char*,"
           " const char*&) - Repeating: " << min << " - " << max);

   unsigned int i;
   for (i = 1; i <= min; ++i) {
      TRACE3 ("RegularExpression::compActREPart (MFCOMPARE, const char*&, const char*,"
              " const char*&) - Repeating: " << i << ". mandatory");

      if (!(this->*fnCompare) (pAktRegExp, pEndRE, pCompare))    // Match til min
         return false;
   }

   const char* pSaveRE (pAktRegExp);
   const char* pSaveEnd (pEndRE);
   const char* pSaveComp (pCompare);
   for (; i < static_cast <unsigned int> (max); ++i) {   // then (til max) try
      TRACE3 ("RegularExpression::compActREPart (MFCOMPARE, const char*&, const char*,"
              " const char*&) - Repeating: " << i << ". optional");


      if (doCompare (pSaveEnd, pCompare)) // if remaining parts match; if yes:
         return true;                                          // Return found

      pAktRegExp = pSaveRE;
      pEndRE = pSaveEnd;
      pCompare = pSaveComp;                        // Else: Check if act. part
      if ((this->*fnCompare) (pAktRegExp, pEndRE, pCompare)) {     // is valid
         pSaveRE = pAktRegExp;                             // and repeat proc.
         pSaveComp = pCompare;
      }
      else
         return false;
   }
   return false;
 }


/*--------------------------------------------------------------------------*/
//Purpose   : Checks how often the last regular expression must be repeated
//Parameters: pRE: Ptr. to regular expression
//            min: Minimal number, how often the previous part of the regular
//                 expression must be repeated 
//            max: Maximal number
//Returns   : const char*: End of repeat-factor (char behind)
//Requieres : pRE ASCIIZ-string; not NULL
/*--------------------------------------------------------------------------*/
const char* RegularExpression::getRepeatFactor (const char* pRE, int& min, int& max) const {
   assert (pRE);

   TRACE3 ("RegularExpression::getRepeatFactor (const char*, int&, int&) const - Checking: "
           << pRE);

   const char* pEnd (pRE);

   switch (*pRE) {
   case MULTIMATCHOPT:
      min = 0;
      max = -1;
      ++pEnd;
      break;

   case MULTIMATCHMAND:
      min = 1;
      max = -1;
      ++pEnd;
      break;

   case MULTIMATCH1:
      min = 0;
      max = 1;
      ++pEnd;
      break;

   case BOUNDBEG:
      min = isdigit (*pRE) ? strtoul (pRE + 1, &const_cast <char*> (pEnd), 10) : 1;
      max = min;
      if ((*pEnd == ',') && isdigit (pEnd--[1]))
         max = strtoul (pRE + 1, &const_cast <char*> (pEnd), 10);
      assert (*++pEnd == BOUNDEND);
      ++pEnd;
      break;

   default:
      min = max = 1;
      break;
   } // end-switch

   return pEnd;
}
#endif

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the consistency of the regular expression
//Returns   : int: Status; 0: OK; 1: No regular expressions
//Requires  : pFileRegExp is a valid reg. exp.
//Throws    : std::string describing error
/*--------------------------------------------------------------------------*/
int RegularExpression::checkIntegrity () const throw (std::string) {
#ifndef HAVE_REGEX_H
   const char* pRegExp = getExpression ();
   if (!pRegExp)
      return 1;

   const char* pPrevExpr = NULL;
   int cGroups = 0;

   while (*pRegExp) {
      switch (*pRegExp) {
      case  REGIONBEGIN:
         if (!*++pRegExp)
            throw (getError (RANGE_OPEN, pRegExp - getExpression ()));

         if (*pRegExp == NEGREGION)             // Skip leading range-inversion
            ++pRegExp;

         if (*pRegExp)        // Skip leading (or second) end-of-range charcter
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
         ++cGroups;
         break;

      case GROUPEND:
         --cGroups;
         break;

      case ESCAPE:
         if ((pRegExp[1] > '0') && (pRegExp[1] <= '9')
             && ((pRegExp[1] + '0') > cGroups)) {
            throw (getError (INV_DIGIT, pRegExp - getExpression ()));
         } // endif 
         break;

      case MULTIMATCHOPT:
      case MULTIMATCHMAND:
      case MULTIMATCH1:
         if (!pPrevExpr)
            throw (getError (NO_PREV_EXP, pRegExp - getExpression ()));
         pPrevExpr = NULL;
         break;

      case BOUNDBEG:
         if (isdigit (pRegExp[1])
             || (pRegExp[1] == ',')) {      // Check if bound or just '{' found
            if (pPrevExpr) 
               throw (getError (NO_PREV_EXP, pRegExp - getExpression ()));

	    char* pEnd;
            unsigned long min (strtoul (pRegExp + 1, &pEnd, 10));
            unsigned long max ((unsigned long)-1);
            if ((*pEnd == ',') && isdigit (pEnd[1]))
               max = strtoul (pRegExp + 1, &pEnd, 10);

            if (min > max)
               throw (getError (INV_BOUND, pRegExp - getExpression () + 1));

            pRegExp = pEnd;
	    if (*++pRegExp != BOUNDEND)
               throw (getError (INV_BOUND, pRegExp - getExpression ()));
         } // endif bound found
         break;

      default:
         pPrevExpr = pRegExp;
      } // end-switch

      ++pRegExp;
   } // end-while

   if (cGroups)
      throw (getError (GROUP_OPEN, 0));

   if (pRegExp[-1] == ESCAPE)
      throw (getError (ENDING_BACKSLASH, pRegExp - getExpression () - 1));

#endif
   return 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Builds the error-string for checkIntegrity ()
//Parameters: rc: Occured error
//            pos: Position of the error inside the regular expression
//Returns   : std::string: Text describing error in human-readable format
//Requires  : error is an ASCIIZ-string
/*--------------------------------------------------------------------------*/
std::string RegularExpression::getError (int rc, unsigned int pos) const {
#ifdef HAVE_REGEX_H
   char error[256];
   regerror (rc, &regexp, error, sizeof (error));
#else
   const char* error = NULL;

   switch (rc) {
   case REGION_OPEN: error = N_("Unmatched [ or [^"); break;
   case GROUP_OPEN: error = N_("Unmatched \\( or \\)"); break;
   case RANGE_OPEN: error = N_("Invalid range end"); break;
   case NO_PREV_EXP: error = N_("Repeating suffix without previous expression"); break;
   case INV_DIGIT: error = N_("Invalid group-number"); break;
   case INV_RANGE: error = N_("Invalid range (lower border larger than upper border)"); break;
   case INV_BOUND: error = N_("Invalid bound (lower border larger than upper border)"); break;
   case ENDING_BACKSLASH: error = N_("Regular expression ends with escape-character (\\)"); break;
   default: error = N_("Unknown error"); break;
   } // end-switch
#endif

   std::string err (getExpression ());
   err += _("`%1', position %2: %3");
   err += ANumeric::toString ((unsigned long)pos + 1);
   err += _(error);
}


#ifdef HAVE_REGEX_H
/*--------------------------------------------------------------------------*/
//Purpose   : Initializes the regular-expression
//Parameters: pRegExp: Pointer to ASCIIZ-string holding regexp
//Requires  : pRegExp is an ASCIIZ-string (not NULL)
/*--------------------------------------------------------------------------*/
void RegularExpression::init (const char* pRegExp) throw (std::string) {
   assert (pRegExp);

   int rc = regcomp (&regexp, pRegExp, REG_EXTENDED);
   if (rc)
      throw (getError (rc, 0));
   }
#endif
