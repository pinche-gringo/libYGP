//$Id: FileRExp.cpp,v 1.8 2000/05/15 21:57:26 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : FileRegularExpr
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.8 $
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


#include <assert.h>

#ifdef HAVE_FNMATCH
#  include <fnmatch.h>
#endif

#ifdef UNIX
#  define upper
#else
#  include <ctype.h>
#  define upper        (char)toupper
#endif

#include "ANumeric.h"
#include "FileRExp.h"

#include <iostream.h>


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
FileRegularExpr::~FileRegularExpr () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Checks if the passed string matches the regular expression
//Parameters: pAktRegExp: Pointer to regular expression
//            pCompare: String to compare
//Returns   : bool: Result (true: match)
//Require   : pAktRegExp, pCompare: ASCIIZ-string
/*--------------------------------------------------------------------------*/
bool FileRegularExpr::compare (const char* pAktRegExp, const char* pCompare) const {
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
            if (compare (pAktRegExp, pCompare++))
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
            if (pAktRegExp[1] == RANGE) {
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
//Returns   : int: Status; 0: OK; 1: No regulare expression
//Require   : pFileRegExp is a valid reg. exp.
//Throws    : std::string: In case of error a describing text
/*--------------------------------------------------------------------------*/
int FileRegularExpr::checkIntegrity () const throw (std::string) {
   const char* pRegExp = getExpression ();
   if (!(pRegExp || *pRegExp))
      return 1;

   while (*pRegExp) {
      if (*pRegExp == REGIONBEGIN) {
         if (!*++pRegExp)
            throw (getError ("Open region", pRegExp - getExpression ()));
         if (*pRegExp == REGIONEND)
            throw (getError ("Empty region", pRegExp - getExpression ()));

         while (*++pRegExp != REGIONEND) {
            switch (*pRegExp) {
            case RANGE:
               if (!pRegExp[1] || (pRegExp[1] == REGIONEND))
                  throw (getError ("Empty range", pRegExp - getExpression ()));
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

/*--------------------------------------------------------------------------*/
//Purpose   : Builds the error-string for checkIntegrity ()
//Parameters: error: Text describing error
//            pos: Position of the error inside the regular expression
//Returns   : std::string: Text describing error in human-readable format
//Require   : error is an ASCIIZ-string
/*--------------------------------------------------------------------------*/
std::string FileRegularExpr::getError (const char* error, unsigned int pos) const {
   assert (error);
   return (std::string (getExpression ()) + std::string (", position ")
           + ANumeric::toString ((unsigned long)pos) + std::string (": ")
           + std::string (error));
}
