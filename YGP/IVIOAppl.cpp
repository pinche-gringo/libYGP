//$Id: IVIOAppl.cpp,v 1.1 1999/07/31 00:15:08 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : IVIOApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
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
//                                   __FILE__ " $Revision: 1.1 $@§$%";


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

   if (pOptionParam) {
       pHelp = pOptionParam;
       pOptionParam = NULL;
   }
   else
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
   unsigned int i (startArg);
   char option (0);

   while (i < args) {
      assert (ppArgs[i]); assert (*ppArgs[i]);
      if (isOptionChar (*ppArgs[i])) {                        // Option passed
	 unsigned int j (i);

	 if (!pOptionParam)
	    pOptionParam = (ppArgs[i] + 1);

	 option = *pOptionParam++;
	 if (!*pOptionParam)
	    pOptionParam = NULL;

	 // If option has been passed after parameters -> Move opt. to begin.
	 if (j != startArg) {
	    char* pHelp (ppArgs[i]);

	    while (j > startArg) {
	       assert (ppArgs[j - 1]); assert (!isOptionChar (*ppArgs[j - 1]));
	       assert (ppArgs[j]);
	       ppArgs[j] = ppArgs[j - 1];
	       j--;
	    }
	    ppArgs[j] = pHelp;
	 }
	 if (!pOptionParam)
	    startArg++;
	 break;
      }
      i++;
   }

   if (isOptionChar (option))                       // Specialhandling of "--"
      if (*pOptionParam) {          // Is text behind --? Treat as long option
	 unsigned int i (numLongOpt);

	 while (i--) {
	    assert (longOpt);
	    assert (longOpt->longVal);
	    if (!strcmp (longOpt[i].longVal, pOptionParam))
	       break;
	 } // end-while

	 if (i == (unsigned int)-1) {
	    option = '?';
	    cerr << name () << "-Error: Unrecognized option '"
		 << pOptionParam << "'\n";
	 }
	 else {
	    option = longOpt[i].shortVal;
	    getOptionValue ();
	 } // endif
      }
      else                                   // Option --? Means end of option
	 option = '\0';

   return option;
}
