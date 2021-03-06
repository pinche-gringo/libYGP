//$Id: FileRExp.cpp,v 1.34 2008/06/11 17:52:57 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : FileRegularExpr
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.34 $
//AUTHOR      : Markus Schwab
//CREATED     : 29.7.1999
//COPYRIGHT   : Copyright (C) 1999 - 2004, 2006, 2008

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


#include <cctype>
#include <cstring>

#include "YGP/Internal.h"

#ifdef HAVE_FNMATCH
#  include <fnmatch.h>
#endif

#if SYSTEM == UNIX
#  define UPPER
#else
#  define UPPER        (char)toupper
#endif

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/ANumeric.h"
#include "YGP/FileRExp.h"


namespace YGP {

#define isclass(type,str,len,ch) (strncmp ((str), #type, len) ? 0 : (is##type (ch) ? 2 : 1))


const char MULTIMATCH = '*';
const char SINGLEMATCH = '?';
const char REGIONBEGIN = '[';
const char REGIONEND = ']';
const char RANGE = '-';
const char NEGREGION1 = '^';
const char REGIONCLASS = ':';
const char REGIONCOLLATE = '.';

#ifdef HAVE_EXCLMARK_AS_REGION_NEGATOR
const char NEGREGION2 = '!';

static inline bool isRegionNegator (const char ch) {
   return (ch == NEGREGION1) || (ch == NEGREGION2);
}
#else

static inline bool isRegionNegator (const char ch) {
   return ch == NEGREGION1;
}
#endif


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
FileRegularExpr::~FileRegularExpr () {
}


//-----------------------------------------------------------------------------
/// Checks if the passed text matches the passed regular expression. Both
/// passed parameter must not be NULL-pointers and the regular expression must
/// be in a valid syntax (this can be checked with the checkIntegrity()-method).
/// \param pAktRegExp Pointer to character array holding regular expression
/// \param pCompare Pointer to character array holding value to compare
/// \returns bool Result (true: match)
/// \pre \c pAktRegExp, \c pCompare must be ASCIIZ-strings
//-----------------------------------------------------------------------------
bool FileRegularExpr::compare (const char* pAktRegExp, const char* pCompare) {
   Check1 (pAktRegExp); Check1 (pCompare); Check1 (!checkIntegrity ());

#ifdef HAVE_FNMATCH
   // Use fnmatch if available and working
   return !fnmatch (pAktRegExp, pCompare, FNM_PATHNAME);
#else
   // Auswerten des Vergleichsstrings
   char ch;
   while (((ch = *pAktRegExp) != '\0') || *pCompare) {
      switch (ch = UPPER (ch)) {
      case MULTIMATCH:
         if (!*++pAktRegExp)
            return true;                       // Asterisk at end? Return match

         while (*pCompare)
            if (compare (pAktRegExp, pCompare++))
               return true;

         return false;

      case SINGLEMATCH:
         break;

      case REGIONBEGIN: {
         // Compares the actual file-char with the region
         bool fNeg (false);
         // Values to invert (either "!" or "^" at beginning)?
         if (isRegionNegator (pAktRegExp[1])) {
            ++pAktRegExp;
            fNeg = true;
         } // endif
         bool inClass (false);

         while ((ch = *++pAktRegExp) != REGIONEND) {
            if (pAktRegExp[1] == RANGE) {
               char chUpper (UPPER (pAktRegExp[2]));
               Check3 ((chUpper != '\0') && (chUpper != REGIONEND));
               char chAkt (UPPER (*pCompare));

               if ((chAkt >= UPPER (ch)) && (chAkt <= chUpper))
                  break;
               pAktRegExp += 2;
            }
            else {
               const char* pEndClass;
               if ((ch == REGIONBEGIN) && (pAktRegExp[1] == REGIONCLASS)
                   && ((pEndClass = strchr (pAktRegExp + 2, REGIONCLASS)) != NULL)
                   && (pEndClass[1] == REGIONEND)) {
                  TRACE7 ("Check " << *pCompare << " against region-class "
                          << pAktRegExp + 2);
                  inClass = true;

                  int temp, len (pEndClass - pAktRegExp - 2);
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
                     TRACE8 ("Check " << *pCompare << " matches region-class "
                             << pAktRegExp + 2);
                     break;
                  } // endif class and input matches
                  else {
                     TRACE8 ("Check " << *pCompare << " doesn't match region-class "
                             << pAktRegExp << " -> RC = " << val);
                     if (val)
                        pAktRegExp = pEndClass + 1;   // Point to end-of-region
                  } // end-else class found, but input doesn't match
               } // endif
               else {
                  TRACE7 ("Check " << *pCompare << " == " << ch);
                  if (UPPER (ch) == UPPER (*pCompare))
                     break;
                } // end-else ordinary character
            } // end-else no range
            inClass = false;
         } // end-while
         TRACE9 ("Match ended; remaining: " << pAktRegExp);

         if ((ch != REGIONEND) == fNeg)
            return false;

         // Search for end-of-region, with regard of region-classes ([:xxx:])
         while (ch != REGIONEND) {
            if (ch == REGIONCLASS) {
               if (pAktRegExp[-1] == REGIONBEGIN)
                  inClass = true;
               else
                  if (pAktRegExp[1] == REGIONEND)
                    if (!inClass)            // If no class left -> Exit anyway
                       break;
                    else
                       inClass = false;
               ++pAktRegExp;
            } // endif region-class found
            ch = *++pAktRegExp;
         } // end-while not region-end found
         TRACE8 ("End of region found; remaining: " << pAktRegExp);
         break;
         }

      default:
         if (UPPER (*pCompare) != ch)
            return false;
         break;
      } // end-switch reg.exp-character
      ++pAktRegExp;
      ++pCompare;
   } // end-while data available
   return !(*pCompare || *pAktRegExp);
#endif
}

//-----------------------------------------------------------------------------
/// Checks the syntax of the regular expression. If everything is OK, 0 is
/// returned; 1 if there is no regular expression at all. In case of any other
/// error an exception is thrown.
/// \returns int Status; 0: OK
/// \throw std::invalid_argument In case of an invalid regexp a describing text
/// \pre pFileRegExp is a valid regexp
//-----------------------------------------------------------------------------
int FileRegularExpr::checkIntegrity () const throw (std::invalid_argument) {
   const char* pRegExp = getExpression ();
   if (!(pRegExp || *pRegExp))
      return 1;

   while (*pRegExp) {
      if (*pRegExp == REGIONBEGIN) {
         if (!*++pRegExp)
            throw (getError (N_("Open region"), pRegExp - getExpression ()));

         // Skip leading range-inversion
         if (isRegionNegator (*pRegExp))
            ++pRegExp;

         if (*pRegExp == REGIONEND)
            ++pRegExp;

         while (*pRegExp != REGIONEND) {
            switch (*pRegExp) {
            case RANGE:
               if (!pRegExp[1] || (pRegExp[1] == REGIONEND))
                  throw (getError (N_("Empty range"), pRegExp - getExpression ()));
               break;

            case '\0':
               throw (getError (N_("Empty region"), pRegExp - getExpression ()));
            } // endif

            ++pRegExp;
         } // end-while
      } // endif
      ++pRegExp;
   } // end-while
   return 0;
}

//-----------------------------------------------------------------------------
/// Builds the error-string for checkIntegrity ()
/// \param error Text describing error
/// \param pos Position of the error inside the regular expression
/// \returns std::invalid_argument With a text describing error in human-readable format
/// \pre error is an ASCIIZ-string
//-----------------------------------------------------------------------------
std::invalid_argument FileRegularExpr::getError (const char* error, unsigned int pos) const {
   Check1 (error);
   std::string err (_("`%1', position %2: %3"));
   err.replace (err.find ("%1"), 2, getExpression ());
   err.replace (err.find ("%2"), 2, ANumeric::toString ((unsigned long)pos));
   err.replace (err.find ("%3"), 2, _(error));
   return std::invalid_argument (err);
}

}
