//$Id: FileRExp.cpp,v 1.21 2003/02/13 07:17:13 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : FileRegularExpr
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.21 $
//AUTHOR      : Markus Schwab
//CREATED     : 29.7.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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


#include <ctype.h>
#include <string.h>

#include "Internal.h"

#ifdef HAVE_FNMATCH
#  include <fnmatch.h>
#endif

#if SYSTEM == UNIX
#  define UPPER
#else
#  define UPPER        (char)toupper
#endif

#include "Check.h"
#include "Trace_.h"
#include "ANumeric.h"
#include "FileRExp.h"


#define isclass(type,str,len,ch) (strncmp ((str), #type, len) ? 0 : (is##type (ch) ? 2 : 1))


#define MULTIMATCH '*'
#define SINGLEMATCH '?'
#define REGIONBEGIN '['
#define REGIONEND ']'
#define RANGE '-'
#define NEGREGION1 '^'
#define NEGREGION2 '!'
#define REGIONCLASS ':'


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
FileRegularExpr::~FileRegularExpr () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed text matches the passed regular expression.
//
//            Both passed parameter must not be NULL-pointers and the regular
//            expression must be in a valid syntax (this can be checked with
//            the checkIntegrity-method).
//Parameters: Pointer to character array holding regular expression
//            pAktRegExp: Pointer to character array holding value to compare
//Returns   : bool: Result (true: match)
//Requires  : pAktRegExp, pCompare: ASCIIZ-strings
/*--------------------------------------------------------------------------*/
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
         if ((pAktRegExp[1] == NEGREGION1) || (pAktRegExp[1] == NEGREGION2)) {
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

/*--------------------------------------------------------------------------*/
//Purpose   : Checks the syntax of the regular expression. If everything is OK,
//            0 is returned; 1 if there is no regular expression at all. In
//            case of any other error an exception is thrown.
//Returns   : int: Status; 0: OK
//Requires  : pFileRegExp is a valid regexp
//Throws    : std::string: In case of an invalid regexp a describing text
/*--------------------------------------------------------------------------*/
int FileRegularExpr::checkIntegrity () const throw (std::string) {
   const char* pRegExp = getExpression ();
   if (!(pRegExp || *pRegExp))
      return 1;

   while (*pRegExp) {
      if (*pRegExp == REGIONBEGIN) {
         if (!*++pRegExp)
            throw (getError (N_("Open region"), pRegExp - getExpression ()));

         // Skip leading range-inversion
         if ((*pRegExp == NEGREGION1) || (*pRegExp == NEGREGION2))
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

/*--------------------------------------------------------------------------*/
//Purpose   : Builds the error-string for checkIntegrity ()
//Parameters: error: Text describing error
//            pos: Position of the error inside the regular expression
//Returns   : std::string: Text describing error in human-readable format
//Requires  : error is an ASCIIZ-string
/*--------------------------------------------------------------------------*/
std::string FileRegularExpr::getError (const char* error, unsigned int pos) const {
   Check1 (error);
   std::string err (_("`%1', position %2: %3"));
   err.replace (err.find ("%1"), 2, getExpression ());
   err.replace (err.find ("%2"), 2, ANumeric::toString ((unsigned long)pos));
   err.replace (err.find ("%3"), 2, _(error));
   return err;
}
