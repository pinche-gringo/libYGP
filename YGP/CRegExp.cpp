//$Id: CRegExp.cpp,v 1.2 2000/05/15 21:58:05 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : RegularExpression
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
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


#define DEBUG 9
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

#ifdef HAVE_REGEX_H
   // Use system-regular expressions if available
   return !regexec (&regexp, pCompare, 0, NULL, 0);
#else
   std::string lastExpr;

   char ch = *pAktRegExp;
   const char* pEnd = NULL;
   bool (RegularExpression::*fnCompare) (const char*&, const std::string&) const = NULL;

   switch (ch) {                                     // Get current expression
   case REGIONBEGIN:
      pEnd = strchr (pAktRegExp + 2, REGIONEND); assert (pEnd);
      lastExpr.assign (pAktRegExp, pEnd - pAktRegExp - 1);
      TRACE7 ("Found region: " << lastExpr.c_str ());
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

         lastExpr.assign (pAktRegExp, pEnd - pAktRegExp - 1);
         TRACE7 ("Found group: " << lastExpr.c_str ());
         fnCompare = &RegularExpression::compGroup;
      } // endif 
      break;


   default:
      assert (ch != MULTIMATCH1); assert (ch != MULTIMATCHOPT);
      assert (ch != MULTIMATCHMAND); assert (ch != SINGLEMATCH);

      fnCompare = &RegularExpression::compChar;
      lastExpr = ch;
   } // end-switch 

   TRACE1 ("Actual expressions: " << lastExpr.c_str ());
   assert (fnCompare);

   return (this->*fnCompare) (pCompare, lastExpr);

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
         if (*pRegExp)     // End-of-range-char at beginning doesn't end range
            ++pRegExp;

         while (*++pRegExp != REGIONEND) {
            switch (*pRegExp) {
            case RANGE:
               if (!pRegExp[1] || (pRegExp[1] == REGIONEND))
                  throw (getError (REGION_OPEN, pRegExp - getExpression ()));
               break;

            case '\0':
               throw (getError (REGION_OPEN, pRegExp - getExpression ()));
            } // endif
         } // end-while

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
   default: error = "Unknown error"; break;
   } // end-switch
#endif

   return (std::string (getExpression ()) + std::string (", position ")
           + ANumeric::toString ((unsigned long)pos + 1) + std::string (": ")
           + std::string (error));
}
