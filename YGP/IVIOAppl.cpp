///$Id: IVIOAppl.cpp,v 1.28 2002/11/04 00:54:13 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : IVIOApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.28 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.6.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001,2002

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


#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <signal.h>
#include <locale.h>

#include <string>
#include <iostream>

#include "File.h"
#include "PathSrch.h"
#include "Internal.h"
#include "StackTrc.h"
#include "IVIOAppl.h"


using namespace std;

/*--------------------------------------------------------------------------*/
//Purpose   : Checks char if it starts an option
//Parameters: ch: Character to check
//Returns   : bool: Result (true: char starts option)
/*--------------------------------------------------------------------------*/
static inline bool isOptionChar (const char ch) {
#if SYSTEM == UNIX
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

   signal (SIGSEGV, handleSignal);
#ifdef HAVE_SIGBUS
   signal (SIGBUS, handleSignal);
#endif

   if (pOpt)
      setLongOptions (pOpt);        // Store the long-option-array (if passed)
}


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
IVIOApplication::~IVIOApplication () {
   signal (SIGSEGV, SIG_DFL);
#ifdef HAVE_SIGBUS
   signal (SIGBUS, SIG_DFL);
#endif
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
      assert (pLongOpts->shortVal != '\0');
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
   std::string inifile (PathSearch::expandNode (std::string (1, '~')));
   assert (inifile.size ());
   if (inifile[inifile.size () - 1] != File::DIRSEPARATOR)
      inifile += File::DIRSEPARATOR;
#if SYSTEM == UNIX
   inifile += '.';
#endif
   inifile += name ();
#if SYSTEM != UNIX
   inifile += ".ini";
#endif
   readINIFile (inifile.c_str ());

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
                  unsigned int found ((unsigned int)-1);
                  const char* pEqual = strchr (pOptionParam, '=');
                  unsigned int len (pEqual ? pEqual - pOptionParam
                                           : strlen (pOptionParam));

                  while (i--) {
                     assert (longOpt); assert (longOpt->longVal);
                     if (!strncmp (longOpt[i].longVal, pOptionParam, len)) {
                        if (found == (unsigned int)-1)
                           found = i;
                        else {
                           std::string error (_("-error: Option `%1' is ambiguous"));
                           error.replace (error.find ("%1"), 2, ppArgs[startOpt]);
                           cerr << name () << error.c_str () << '\n';
                           return '?';
                        } // end-else option ambigous
                     } // endif option matches
                  } // end-while

                  if (found == (unsigned int)-1) {     // No long-option found
                     std::string error (_("-error: Unrecognized option `%1'"));
                     error.replace (error.find ("%1"), 2, ppArgs[startOpt]);
                     cerr << name () << error.c_str () << '\n';
                     return '?';
                   } // endif no longopt found
                  else {
                     option = longOpt[found].shortVal;
                     pOptionParam += len;
                     if (*pOptionParam == '=')
                       ++pOptionParam;
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

/*--------------------------------------------------------------------------*/
//Purpose   : Returns the name of the program (without any path-information)
//Returns   : const char*: Name of programm
/*--------------------------------------------------------------------------*/
const char* IVIOApplication::name () const {
   const char* pEnd = strrchr (filename (), File::DIRSEPARATOR);
   return pEnd ? pEnd + 1 : filename ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Initializes program for internationalization (locale)
/*--------------------------------------------------------------------------*/
void IVIOApplication::initI18n () {
   setlocale (LC_ALL, "");                         // Activate current locale
}

/*--------------------------------------------------------------------------*/
//Purpose   : Initializes program for internationalization
//Parameters: package: Name of the message-catalog
//            dir: root-directory for message-catalogs
/*--------------------------------------------------------------------------*/
void IVIOApplication::initI18n (const char* package, const char* dir) {
   assert (package); assert (dir);
   initI18n ();

#ifdef HAVE_GETTEXT
   bindtextdomain (package, dir);
   textdomain (package);
#endif
}
