//$Id: CRegExp.cpp,v 1.8 2000/05/23 22:56:48 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RegularExpression
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.8 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.5.2000
//COPYRIGHT   : Anticopyright (A) 2000

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


#define DEBUG 0
#include "Trace_.h"

#include "CRegExp.h"
#include "ANumeric.h"

#ifndef HAVE_REGEX_H
#  include <ctype.h>
#  define isclass(type,str,ch) (strncmp ((str), #type, sizeof (#type) - 1) ? 0 : (is##type (ch) ? 2 : 1))
#endif


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: pRegExp: Pointer to regular expression
/*--------------------------------------------------------------------------*/
RegularExpression::RegularExpression (const char* pRegExp)
   : IRegularExpression (pRegExp) {
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
//Require   : pAktRegExp, pCompare: ASCIIZ-string
/*--------------------------------------------------------------------------*/
bool RegularExpression::compare (const char* pAktRegExp, const char* pCompare) const {
   assert (pAktRegExp); assert (pCompare); assert (!checkIntegrity ());

   TRACE1 ("RegularExpression::compare -> " << pAktRegExp << " <-> " << pCompare);

#ifdef HAVE_REGEX_H
   // Use system-regular expressions if available
   return !regexec (&regexp, pCompare, 0, NULL, 0);
#else
   const_cast<RegularExpression*>(this)->pStartCompare = pCompare;

   std::string lastExpr;

   char ch;
   const char* pEnd = NULL;

   // Functionpointer for differing comparisons; those methods must take care
   // for increasing position-pointers!
   bool (RegularExpression::*fnCompare) (const char*&, const std::string&) const = NULL;

   // Check actual rex-exp for special characters (at the current position)
   // After this loop the the following conditions must be fullfilled:
   // - fnCompare points to the compare-function to use
   // - pEnd points to the end of the part of the reg-exp belonging to the
   //   current position (like end of group, ...)
   // - lastExpr contains the actual part of the regexp
   while ((ch = *pAktRegExp)) {
      switch (ch) {                                   // Get current expression
      case REGIONBEGIN:
         pEnd = pAktRegExp + 1;
         if (*pEnd == NEGREGION)           // Skip leading "special" characters
            ++pEnd;
         if (*pEnd == REGIONEND)
            ++pEnd;

         // Search for end-of-region, with regard of region-classes ([:xxx:])
         for (int cClass = 0; *pEnd != REGIONEND; ++pEnd) {
            TRACE9 ("Search for region-end: " << *pEnd);

            if (*pEnd == REGIONCLASS) {
               if (pEnd[-1] == REGIONBEGIN)
                  ++cClass;
               else
                  if (pEnd[1] == REGIONEND)
                     if (!cClass--)          // If no class left -> Exit anyway
                        break;
                     ++pEnd;
            } // endif region-class found
         } // end-for region-end found

         lastExpr.assign (pAktRegExp + 1, pEnd - pAktRegExp - 1);
         TRACE4 ("Found region: " << lastExpr.c_str ());
         fnCompare = &RegularExpression::compRegion;
         break;

      case ESCAPE:
         if (pAktRegExp[1] == GROUPBEGIN) {
            int cGroups = 1;
            pEnd = pAktRegExp + 1;
            do {
               pEnd = strchr (pEnd + 1, ESCAPE);
               if (pEnd[1] == GROUPEND)
                  --cGroups;
               else
                  if (pEnd[1] == GROUPBEGIN)
                     ++cGroups;
            } while (!cGroups); // end-do 
            ++pEnd;

            // TODO: ALTERNATIVE

            lastExpr.assign (pAktRegExp, pEnd - pAktRegExp);
            TRACE4 ("Found group: " << lastExpr.c_str ());
            fnCompare = &RegularExpression::compGroup;
         } // end GROUPBEGIN
         else {
            fnCompare = &RegularExpression::compEscChar;
            pEnd = pAktRegExp + 1;
            lastExpr = pAktRegExp[1];
         } // endif other escaped character
         break;

      default:
         assert (ch); assert (ch != MULTIMATCHMAND);
         assert (ch != MULTIMATCH1); assert (ch != MULTIMATCHOPT);

         fnCompare = &RegularExpression::compChar;
         lastExpr = ch;
         pEnd = pAktRegExp;
      } // end-switch 

      TRACE1 ("Actual expression: " << lastExpr.c_str ());
      assert (fnCompare); assert (pEnd);

      switch (pEnd[1]) {
      case MULTIMATCH1:
         TRACE6 ("Possible count = 0, 1");
         (this->*fnCompare) (pCompare, lastExpr);
         ++pEnd;
         break;

      case MULTIMATCHMAND:
         TRACE6 ("Possible count = 1, -1");
         if (!(this->*fnCompare) (pCompare, lastExpr))  // Must be min. 1 match
            return false;

      case MULTIMATCHOPT: {
         TRACE6 ("Possible count = 0, -1");

          // Hm, what's faster? To find the last position of the last (small)
         // expression and the search back for the last position matching the
         // whole regexp or search for the whole stuff til it doesn't match
         // anymore? Let's implement the first and care about the speed later.
         const char* pAktPos = pCompare;
         while (*pAktPos && (this->*fnCompare) (pAktPos, lastExpr)) ;

         while (pAktPos >= pCompare) {   // Try to find next smaller max. match
            if (compare (pEnd + 2, pAktPos))
               return true;
            --pAktPos;
         } // end-while
         ++pEnd;
         }
         break;

      default:
         TRACE6 ("Possible count = 1");
         if (!(*pCompare && (this->*fnCompare) (pCompare, lastExpr)))
            return false;
      } // end-switch

      pAktRegExp = pEnd + 1;
   } // end-while

   return ch ? false : !*pCompare;       // Match OK, if regexp and match empty
#endif
}

#ifndef HAVE_REGEX_H
/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the region
//Parameters: pAktPos: Reference to pointer to actual position in match
//            region: Region to compare
//Returns   : bool: Result (true: match)
//Require   : pAktPos ASCIIZ-string; region not empty
/*--------------------------------------------------------------------------*/
bool RegularExpression::compRegion (const char*& pAktPos,
                                    const std::string& region) const {
   assert (pAktPos); assert (!region.empty ());
   TRACE3 ("RegularExpression::compRegion -> " << *pAktPos << " in ["
           << region.c_str () << ']');

   const char* pRegion = region.c_str ();

   // Compares the actual file-char with the region
   bool fNeg (false);
   if (*pRegion == NEGREGION) {                            // Values to invert?
      ++pRegion;
      fNeg = true;
   } // endif

   char ch (*pRegion);

   do {
      if (pRegion[1] == RANGE) {
         char chUpper (pRegion[2]);
         assert (chUpper != '\0');  assert (chUpper != REGIONEND);

         TRACE7 ("Check " << *pAktPos << " in [" << ch << '-' << chUpper << ']');
         if ((*pAktPos >= ch) && (*pAktPos <= chUpper))
            break;

         pRegion += 2;
      } // endif range found
      else {
         // Check for class of characters or ordinary char
         char* pEndClass = NULL;

         if ((ch == REGIONBEGIN) && (pRegion[1] == REGIONCLASS)
              && ((pEndClass = strchr (pRegion + 2, REGIONCLASS)) != NULL)
              && (pEndClass[1] == REGIONEND)) {
	    TRACE7 ("Check " << *pAktPos << " against region-class " << pRegion + 2);

            int temp (0);
            int val ((temp = isclass (alnum, pRegion + 2, *pAktPos)) ? temp :
		     (temp = isclass (alpha, pRegion + 2, *pAktPos)) ? temp :
		     (temp = isclass (digit, pRegion + 2, *pAktPos)) ? temp :
		     (temp = isclass (space, pRegion + 2, *pAktPos)) ? temp :
		     (temp = isclass (cntrl, pRegion + 2, *pAktPos)) ? temp :
		     (temp = isclass (graph, pRegion + 2, *pAktPos)) ? temp :
		     (temp = isclass (print, pRegion + 2, *pAktPos)) ? temp :
		     (temp = isclass (punct, pRegion + 2, *pAktPos)) ? temp :
		     (temp = isclass (upper, pRegion + 2, *pAktPos)) ? temp :
		     (temp = isclass (lower, pRegion + 2, *pAktPos)) ? temp :
		     (isclass (xdigit, pRegion + 2, *pAktPos)));
            if (val == 2) {
               TRACE8 ("Check " << *pAktPos << " matches region-classes"
                        << setw (pRegion + 2 - pEndClass) << pRegion + 2);
               break;
            } // endif class and input matches
            else {
               TRACE8 ("Check " << *pAktPos << " doesn't match region-class "
                       << setw (pRegion + 2 - pEndClass) << pRegion);
               if (val)
                  pRegion = pEndClass + 1;
            } // end-else class found, but input doesn't match
         } // endif
         else {
            TRACE7 ("Check " << *pAktPos << " == " << ch);
            if (ch == *pAktPos)
               break;
         } // end-else ordinary character
      } // end-else non-range

      ch = *++pRegion;
   } while (ch != '\0'); // end-do

   if ((ch != '\0') == fNeg)
      return false;

   ++pAktPos;
   return true;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the group
//Parameters: pAktPos: Reference to pointer to actual position in match
//            region: Group to compare
//Returns   : bool: Result (true: match)
//Require   : pAktPos ASCIIZ-string; group not empty
/*--------------------------------------------------------------------------*/
bool RegularExpression::compGroup (const char*& pAktPos,
                                   const std::string& group) const {
   assert (pAktPos); assert (!group.empty ());
   TRACE3 ("RegularExpression::compGroup -> " << pAktPos << " in \\("
           << group.c_str () << "\\)");

   return compare (pAktPos, group.c_str ());
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the single character
//Parameters: pAktPos: Reference to pointer to actual position in match
//            ch: Character to compare
//Returns   : bool: Result (true: match)
//Require   : pAktPos ASCIIZ-string; ch has length of 1
/*--------------------------------------------------------------------------*/
bool RegularExpression::compChar (const char*& pAktPos,
                                  const std::string& ch) const {
   assert (pAktPos); assert (ch.length () == 1);
   TRACE3 ("RegularExpression::compChar -> " << *pAktPos << " == " << ch[0]);

   switch (ch[0]) {
   case SINGLEMATCH:
      break;

   case LINEBEGIN:
      assert (pAktPos >= pStartCompare);
      if (pAktPos == pStartCompare || (pAktPos[-1] == '\n'))
         break;
      return false;

   case LINEEND:
      if (!pAktPos[1] || (pAktPos[1] == '\n'))
         break;
      return false;

   default:
      if (ch[0] == *pAktPos)
         break;
      return false;
   } // end-switch 

   ++pAktPos;
   return true;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the group
//Parameters: pAktPos: Reference to pointer to actual position in match
//            ch: Character to compare
//Returns   : bool: Result (true: match)
//Require   : pAktPos ASCIIZ-string; ch has length of 1
/*--------------------------------------------------------------------------*/
bool RegularExpression::compEscChar (const char*& pAktPos,
                                     const std::string& ch) const {

   assert (pAktPos); assert (ch.length () == 1);
   TRACE3 ("RegularExpression::compEscChar -> " << *pAktPos << " == " << ch[0]);

   switch (ch[0]) {
   case WORD:
      if (isalnum (*pAktPos))
         break;
      return false;

   case NOTWORD:
      if (!isalnum (*pAktPos))
         break;
      return false;

   case WORDBORDER:
      assert (pAktPos >= pStartCompare);
      return ((pAktPos == pStartCompare) || !pAktPos[1]
              || ((isalnum (*pAktPos)) != isalpha (pAktPos[1]))
              || ((isalnum (*pAktPos)) != isalpha (pAktPos[-1])));

   case NOTWORDBORDER:
      assert (pAktPos >= pStartCompare);
      return (pAktPos != pStartCompare) && pAktPos[1]
	&& isalnum (*pAktPos) == isalnum (pAktPos[1]) && isalnum (pAktPos[-1]);

   case WORDBEGIN:
      assert (pAktPos >= pStartCompare);
      return ((pAktPos == pStartCompare)
              || ((isalnum (*pAktPos)) && !isalpha (pAktPos[1])));

   case WORDEND:
      assert (pAktPos >= pStartCompare);
      return ((pAktPos != pStartCompare)
              && !isalnum (*pAktPos) && isalnum (pAktPos[-1]));

   default:
      return compChar (pAktPos, ch);
   } // end-switch

   ++pAktPos;
   return true;
}

#endif


/*--------------------------------------------------------------------------*/
//Purpose   : Checks the consistency of the regular expression
//Returns   : int: Status; 0: OK; 1: No regular expressions
//Require   : pFileRegExp is a valid reg. exp.
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

      case ESCAPE:
         switch (pRegExp[1]) {
         case GROUPBEGIN:
            ++cGroups;
            break;

         case GROUPEND:
            --cGroups;
            break;

         default:
            if ((pRegExp[1] > '0') && (pRegExp[1] <= '9')
                && ((pRegExp[1] + '0') > cGroups)) {
              throw (getError (INV_DIGIT, pRegExp - getExpression ()));
            } // endif 
         } // end-switch 
         break;

      case MULTIMATCHOPT:
      case MULTIMATCHMAND:
      case MULTIMATCH1:
         if (!pPrevExpr)
            throw (getError (NO_PREV_EXP, 0));
         pPrevExpr = NULL;
         break;

      default:
         pPrevExpr = pRegExp;
      } // end-switch

      ++pRegExp;
   } // end-while

   if (cGroups)
      throw (getError (GROUP_OPEN, 0));
#endif
   return 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Builds the error-string for checkIntegrity ()
//Parameters: rc: Occured error
//            pos: Position of the error inside the regular expression
//Returns   : std::string: Text describing error in human-readable format
//Require   : error is an ASCIIZ-string
/*--------------------------------------------------------------------------*/
std::string RegularExpression::getError (int rc, unsigned int pos) const {
#ifdef HAVE_REGEX_H
   char error[256];
   regerror (rc, &regexp, error, sizeof (error));
#else
   const char* error = NULL;

   switch (rc) {
   case REGION_OPEN: error = "Unmatched [ or [^"; break;
   case GROUP_OPEN: error = "Unmatched \\( or \\)"; break;
   case RANGE_OPEN: error = "Invalid range end"; break;
   case NO_PREV_EXP: error = "Suffix without previous expression"; break;
   case INV_DIGIT: error = "Invalid group-number"; break;
   case INV_RANGE: error = "Invalid range (lower border larger than upper border"; break;
   default: error = "Unknown error"; break;
   } // end-switch
#endif

   return (std::string (getExpression ()) + std::string (", position ")
           + ANumeric::toString ((unsigned long)pos + 1) + std::string (": ")
           + std::string (error));
}
