//$Id: FileRExp.cpp,v 1.4 1999/09/11 01:07:57 Markus Rel $

//PROJECT     : General
//SUBSYSTEM   : FileRegularExpr
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
//AUTHOR      : Markus Schwab
//CREATED     : 29.7.1999
//COPYRIGHT   : Anticopyright (A) 1999

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


#ifdef UNIX
#include <fnmatch.h>

#define upper
#else
#include <ctype.h>
#define upper        (char)toupper
#endif

#include "FileRExp.h"


const char FileRegularExpr::MULTIMATCH  = '*';
const char FileRegularExpr::SINGLEMATCH = '?';
const char FileRegularExpr::REGIONBEGIN = '[';
const char FileRegularExpr::REGIONEND   = ']';
const char FileRegularExpr::REGION      = '-';
const char FileRegularExpr::NEGREGION   = '^';


#include <iostream.h>

/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the regular expression
//Parameters: pAktRegExp: Pointer to regular expression
//            pCompare: String to compare
//Returns   : bool: Result (true: match)
//Require   : pAktRegExp, pCompare: ASCIIZ-string
/*--------------------------------------------------------------------------*/
bool FileRegularExpr::matches (const char* pAktRegExp, const char* pCompare) const {
   assert (pAktRegExp); assert (pCompare); assert (!checkIntegrity ());

#ifdef HAVE_FNMATCH
   // Use fnmatch if available and working
   return !fnmatch (pAktRegExp, pCompare, FNM_PATHNAME);
#else
   // Auswerten des Vergleichsstrings
   char ch;
   while (((ch = *pAktRegExp) != '\0') || *pCompare) {
      switch (ch = upper (ch)) {
      case MULTIMATCH:
         if (!*++pAktRegExp)
            return true;                       // Asterisk at end? Return match

         while (*pCompare)
            if (matches (pAktRegExp, pCompare++))
               return true;

         return false;

      case SINGLEMATCH:
         break;

      case REGIONBEGIN: {
         // Compares the actual file-char with the region
         bool fNeg (false);
         if (pAktRegExp[1] == NEGREGION) {                // Values to invert?
            ++pAktRegExp;
            fNeg = true;
         } // endif

         while ((ch = *++pAktRegExp) != REGIONEND) {
            if (pAktRegExp[1] == REGION) {
               char chUpper (upper (pAktRegExp[2]));
               assert ((chUpper != '\0') && (chUpper != REGIONEND));
               char chAkt (upper (*pCompare));

               if ((chAkt >= upper (ch)) && (chAkt <= chUpper))
                  break;
               pAktRegExp += 2;
            }
            else
               if (upper (ch) == upper (*pCompare))
                  break;
         } // end-while

         if ((ch != REGIONEND) == fNeg)
            return false;

         while (ch && (ch != REGIONEND))
            ch = *++pAktRegExp;
         break;
         }

      default:
         if (upper (*pCompare) != ch)
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
//Purpose   : Checks the consistency of the regular expression
//Returns   : int: Status; 0: OK; 1: Region not closed; 2: Open region-area
//            3: Empty region
//Require   : pFileRegExp is a valid reg. exp.
/*--------------------------------------------------------------------------*/
int FileRegularExpr::checkIntegrity () const {
   assert (pFileRegExp);

   const char* pRegExp (pFileRegExp);

   while (*pRegExp) {
      if (*pRegExp == REGIONBEGIN) {
         if (!*++pRegExp)
            return 1;
         if (*pRegExp == REGIONEND)
            return 3;

         while (*++pRegExp != REGIONEND) {
            switch (*pRegExp) {
            case REGION:
               if (!pRegExp[1])
                  return 2;
               if (pRegExp[1] == REGIONEND)
                  return 1;
               break;
            case '\0':
               return 1;
            } // endif
         } // end-while
      } // endif
      ++pRegExp;
   } // end-while
   return 0;
}
