///$Id: IVIOAppl.cpp,v 1.14 2000/02/02 22:10:24 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : IVIOApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.14 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.6.1999
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


#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <iostream.h>

#include "IVIOAppl.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Checks char if it starts an option
//Parameters: ch: Character to check
//Returns   : bool: Result (true: char starts option)
/*--------------------------------------------------------------------------*/
static inline bool isOptionChar (const char ch) {
#ifdef UNIX
   return ch == '-';
#else
   return (ch == '-') || (ch == '/');
#endif
}


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: argc: Number of arguments
//            argv: Array of pointers to argumetns
//            pOpt: Pointer to long-option-table
/*--------------------------------------------------------------------------*/
IVIOApplication::IVIOApplication (const int argc, const char* argv[],
                                  const longOptions* pOpt)
   : args (argc), ppArgs (argv), startArg (1), pOptionParam (NULL)
   , longOpt (NULL), startOpt (1)
   , numLongOpt (0) {
   assert (args > 0);
   assert (ppArgs);
   if (pOpt)
      setLongOptions (pOpt);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Sets the long-options table
//Rarameters: pLongOptions: Ponter to longOptions; terminated with a
//                          longOption with NULL as longVal
//Requires  : pLongOpts != NULL; with an { NULL, '\0' }-entry at the end
/*--------------------------------------------------------------------------*/
void IVIOApplication::setLongOptions (const longOptions* pLongOpts) {
   assert (pLongOpts);
   assert (pLongOpts->longVal);         // At least one valid entry must exist

   longOpt = pLongOpts;
   while (pLongOpts->longVal != NULL) {
      ++numLongOpt;
      pLongOpts++;
   } // end-while

   assert (pLongOpts->shortVal == '\0'); // Last entry mustn't have a shortval
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the long-options table
//Rarameters: pLongOptions: Pointer to longOptions
//Requires  : pLongOpts != NULL; all long-entries != NULL
/*--------------------------------------------------------------------------*/
void IVIOApplication::setLongOptions (const longOptions* pLongOpts,
				      unsigned int numLongOpts) {
   assert (numLongOpts);
   assert (pLongOpts);
   assert (pLongOpts->longVal);         // At least one valid entry must exist

   numLongOpt = numLongOpts;
   longOpt = pLongOpts;

#ifndef NDEBUG
   while (numLongOpts--) {
      assert (pLongOpts->shortVal != '\0');
      assert (pLongOpts++->longVal);
   } // end-while
#endif
}


/*--------------------------------------------------------------------------*/
//Purpose   : Runs the application; first the options are checked and then
//            the job is performed
//Returns   : int: Status
/*--------------------------------------------------------------------------*/
int IVIOApplication::run () {
   char ch;
   bool showHlp (false);

   while ((ch = getOption ()) != '\0')
      if ((ch == '?') || (ch == 'h') || !handleOption (ch)) {
         showHlp = true;
          break;
      }

   if (shallShowInfo ())
      cout << name () << " V" << description () << "\n\n";

   if (showHlp) {
      showHelp ();
      return -1;
   }

   return perform (args - startArg, &ppArgs[startArg]);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the value-parameter for the option
//Returns   : char*: Parameter for the option
/*--------------------------------------------------------------------------*/
const char* IVIOApplication::getOptionValue () {
   if (startOpt > startArg)
      moveOption ();

   const char* pHelp;

   if (pOptionParam && *pOptionParam)
      pHelp = pOptionParam;
   else {
      ++startArg;
      if (++startOpt == args)
         return NULL;

      pHelp = ppArgs[startOpt];
      moveOption ();
   }

   ++startArg;
   ++startOpt;
   pOptionParam = NULL;
   return pHelp;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads the options. It handles options in the form:
//              -<Option>[...]
//              -[<Option>[...]]<Option><Value>
//              -[<Option>[...]]<Option> <Value>
//              --
//              --<LongOption>
//              --<LongOption><Value>
//              --<LongOption> <Value>
//Returns   : char: Next option ('\0' at last option)
//Notes     : In non-UNIX-systems slash (/) is also an option-char
/*--------------------------------------------------------------------------*/
char IVIOApplication::getOption () {
   char option ('\0');

   while (startOpt < args) {
      assert (ppArgs[startOpt]); assert (*ppArgs[startOpt]);

      // Check parameters: Option start with - and are longer than 1 char
      if (isOptionChar (*ppArgs[startOpt]) && ppArgs[startOpt][1]) {
         if (!pOptionParam) {
            pOptionParam = ppArgs[startOpt] + 1;
            assert (*pOptionParam);
         } // endif init option-params

         option = *pOptionParam++;
         if (!option) {
            assert (startOpt >= startArg);
            moveOption ();                     // Move option before arguments

            ++startOpt;
            ++startArg;
            pOptionParam = NULL;
            continue;
         } // endif actual option finished

         if (isOptionChar (option)) {               // Specialhandling of "--"
            if (pOptionParam && *pOptionParam) {   // Text behind --? Long opt
               if (longOpt) {                    // Are long-options specified
                  unsigned int i (numLongOpt);

                  while (--i) {
                     assert (longOpt); assert (longOpt->longVal);
                     if (!strcmp (longOpt[i].longVal, pOptionParam))
                        break;
                  } // end-while

                  if (i == (unsigned int)-1) {         // No long-option found
                     option = '?';
                     cerr << name () << "-Error: Unrecognized option '"
                          << pOptionParam << "'\n";
                   } // endif no longopt found
                  else {
        	     option = longOpt[i].shortVal;
        	     pOptionParam += strlen (pOptionParam);
                  } // end-else long-option found
               } // endif no long-options defined
            }
            else {                           // Option --? Means end of option
               moveOption ();
               ++startArg;
               option = '\0';
            }
         } // endif longoption found
         break;
      } // endif option found
      else
         ++startOpt;
   } // end-while arguments

   return option;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Moves the option (or option-value) in parameter numOpt before
//            the arguments (indicated with startArg)
//Parameters: numOpt: Option (argument) to move
/*--------------------------------------------------------------------------*/
void IVIOApplication::moveOption (unsigned int numOpt) const {
   if (numOpt == startArg)
      return;

   assert (numOpt > startArg); assert (numOpt < args);

   const char* pHelp = ppArgs[numOpt];

   while (numOpt > startArg) {
      assert (ppArgs[numOpt - 1]); assert (ppArgs[numOpt]);

      ppArgs[numOpt] = ppArgs[numOpt - 1];
      --numOpt;
   } // end-while option before params
   ppArgs[numOpt] = pHelp;
}
