//$Id: CRegExp.cpp,v 1.4 2000/05/18 17:44:44 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RegularExpression
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
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


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: pRegExp: Pointer to regular expression
/*--------------------------------------------------------------------------*/
RegularExpression::RegularExpression (const char* pRegExp)
   : IRegularExpression (pRegExp) {
#ifdef HAVE_REGEX_H
   int rc = regcomp (&regexp, pRegExp, REG_NOSUB);
   if (rc)
      throw (getError (rc, 0));
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
   int rc = regcomp (&regexp, pRegExp, REG_NOSUB);
   if (rc)
      throw (getError (rc, 0));
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
   std::string lastExpr;

   char ch;
   const char* pEnd = NULL;

   // Functionpointer for differing comparisons; those methods must take care
   // for increasing position-pointers!
   bool (RegularExpression::*fnCompare) (const char*&, const std::string&) const = NULL;

   while ((ch = *pAktRegExp)) {
      switch (ch) {                                   // Get current expression
      case REGIONBEGIN:
         pEnd = pAktRegExp + 1;
         if (*pEnd == NEGREGION)           // Skip leading "special" characters
            ++pEnd;
         if (*pEnd == REGIONEND)
            ++pEnd;

         pEnd = strchr (pEnd, REGIONEND); assert (pEnd);
         lastExpr.assign (pAktRegExp + 1, pEnd - pAktRegExp - 1);
         TRACE5 ("Found region: " << lastExpr.c_str ());
         fnCompare = &RegularExpression::compRegion;
         break;

      case QUOTE:
         if (pAktRegExp[1] == GROUPBEGIN) {
            int cGroups = 1;
            pEnd = pAktRegExp + 1;
            do {
               pEnd = strchr (pEnd + 1, QUOTE);
               if (pEnd[1] == GROUPEND)
                  --cGroups;
	       else
                  if (pEnd[1] == GROUPBEGIN)
                     ++cGroups;
            } while (!cGroups); // end-do 
            ++pEnd;

            lastExpr.assign (pAktRegExp, pEnd - pAktRegExp);
            TRACE5 ("Found group: " << lastExpr.c_str ());
            fnCompare = &RegularExpression::compGroup;
         } // endif 
         break;

      default:
         assert (ch != MULTIMATCH1); assert (ch != MULTIMATCHOPT);
         assert (ch != MULTIMATCHMAND); assert (ch != SINGLEMATCH);
         assert (ch);

         fnCompare = &RegularExpression::compChar;
         lastExpr = ch;
         pEnd = pAktRegExp;
      } // end-switch 

      TRACE1 ("Actual expressions: " << lastExpr.c_str ());
      assert (fnCompare); assert (pEnd);

      switch (pEnd[1]) {
      case MULTIMATCH1:
         TRACE7 ("Possible count = 0, 1");
         (this->*fnCompare) (pCompare, lastExpr);
         break;

      case MULTIMATCHMAND:
         TRACE7 ("Possible count = 1, -1");
         if (!(this->*fnCompare) (pCompare, lastExpr))  // Must be min. 1 match
            return false;

      case MULTIMATCHOPT: {
         TRACE7 ("Possible count = 0, -1");

          // Hm, what's faster? To find the last position of the last (small)
         // expression and the search back for the last position matching the
         // whole regexp or search for the whole stuff til it doesn't match
         // anymore? Let's implement the first and care about the speed later.
         const char* pAktPos = pCompare;
         while ((this->*fnCompare) (pAktPos, lastExpr)) ; // Find maximal match

         while (pAktPos >= pCompare) {   // Try to find next smaller max. match
            if (compare (pEnd + 2, pAktPos))
	       return true;
            --pAktPos;
         } // end-while
         pEnd = pAktPos + 1;
         }
         break;

      default:
         TRACE7 ("Possible count = 1");
         if (!(this->*fnCompare) (pCompare, lastExpr))
            return false;
      } // end-switch

      pAktRegExp = pEnd + 1;
   } // end-while

   return true;          // Match OK, even if string to compare is not finished
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

   char ch = *pRegion;

   do {
      if (pRegion[1] == RANGE) {
         char chUpper (pRegion[2]);
         assert (chUpper != '\0');  assert (chUpper != REGIONEND);

         TRACE9 ("Check " << *pAktPos << " in [" << ch << '-' << chUpper << ']');
         if ((*pAktPos >= ch) && (*pAktPos <= chUpper))
            break;
         pRegion += 2;
      }
      else {
         TRACE9 ("Check " << *pAktPos << " == " << ch);
         if (ch == *pAktPos)
            break;
      } // endif

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

   return true;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the group
//Parameters: pAktPos: Reference to pointer to actual position in match
//            ch: Character to compare
//Returns   : bool: Result (true: match)
//Require   : pAktPos ASCIIZ-string; ch has length of 1
/*--------------------------------------------------------------------------*/
bool RegularExpression::compChar (const char*& pAktPos,
                                  const std::string& ch) const {
   assert (pAktPos); assert (ch.length () == 1);
   TRACE3 ("RegularExpression::compChar -> " << *pAktPos << " == " << ch[0]);

   if ((ch[0] == SINGLEMATCH) || (ch[0] == *pAktPos)) {
      ++pAktPos;
      return true;
   } // endif

   return false;
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

      case QUOTE:
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
//Parameters: error: Text describing error
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
