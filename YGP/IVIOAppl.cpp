///$Id: IVIOAppl.cpp,v 1.3 1999/08/11 16:19:54 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : IVIOApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.6.1999
//COPYRIGHT   : Anticopyright (A) 1999

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <stdio.h>
#include <assert.h>

#include <iostream.h>

#include "IVIOAppl.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Checks char if it starts an option
//Parameters: ch: Character to check
//Returns   : bool: Result (true: char starts option)
/*--------------------------------------------------------------------------*/
inline bool isOptionChar (const char ch) {
#ifdef UNIX
   return ch == '-';
#else
   return (ch == '-') || (ch == '/');
#endif
}


//static const char* const Version = "@§$%" PACKAGE " V" VERSION "." MICRO_VERSION "\n"
//                                   __FILE__ " $Revision: 1.3 $@§$%";


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
/*--------------------------------------------------------------------------*/
IVIOApplication::IVIOApplication (int argc, char* argv[]) : args (argc)
   , ppArgs (argv), startArg (1), pOptionParam (NULL), longOpt (NULL)
   , numLongOpt (0) {
   assert (args > 0);
   assert (ppArgs);
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
char* IVIOApplication::getOptionValue () {
   char* pHelp;
   if (pOptionParam && *pOptionParam) {
      pHelp = pOptionParam;
      pOptionParam = NULL;
   } else
      pHelp = ppArgs[startArg];

   ++startArg;
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
   unsigned int i (startArg - 1);
   char option ('\0');

   while (++i < args) {
      assert (ppArgs[i]); assert (*ppArgs[i]);

      if (isOptionChar (*ppArgs[i])) {                        // Option passed
	 if (!pOptionParam) {
	    pOptionParam = ppArgs[i] + 1;
	    if (!*pOptionParam)
	       continue;
	 } // endif init option-params

         assert (pOptionParam);
         option = *pOptionParam++;
         if (!option) {
            if (i != startArg)
               moveOption (i);                 // Move option before arguments

            ++startArg;
            pOptionParam = NULL;
            continue;
         } // endif actual option finished

         if (isOptionChar (option)) {               // Specialhandling of "--"
            if (pOptionParam && *pOptionParam) {   // Text behind --? Long opt
               unsigned int i (numLongOpt);

	       while (i--) {
                  assert (longOpt); assert (longOpt->longVal);
                  if (!strcmp (longOpt[i].longVal, pOptionParam))
                     break;
               } // end-while

               if (i == (unsigned int)-1) {
        	  option = '?';
        	  cerr << name () << "-Error: Unrecognized option '"
                       << pOptionParam << "'\n";
               } // endif no longopt found
               else {
        	  option = longOpt[i].shortVal;
        	  getOptionValue ();
               } // endif
            }
            else                             // Option --? Means end of option
               option = '\0';
	 } // endif longoption found
	 break;
      } // endif option found
   } // end-while arguments

   return option;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Moves the option in parameter numOpt before the arguments
//            (indicated with startArg)
//Parameters: numOpt: Option (argument) to move
/*--------------------------------------------------------------------------*/
void IVIOApplication::moveOption (unsigned int numOpt) const {
   assert (numOpt > startArg); assert (numOpt < args);
   assert (isOptionChar (*ppArgs[numOpt]));

   char* pHelp (ppArgs[numOpt]);

   while (numOpt > startArg) {
      assert (ppArgs[numOpt - 1]); assert (ppArgs[numOpt]);
      assert (!isOptionChar (*ppArgs[numOpt - 1]));

      ppArgs[numOpt] = ppArgs[numOpt - 1];
      --numOpt;
   } // end-while option before params
   ppArgs[numOpt] = pHelp;
}
