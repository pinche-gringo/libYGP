// $Id: IVIOAppl.cpp,v 1.13 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/IVIOAppl
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.13 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2005, 2008, 2009

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


#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif


#include <clocale>

#include <iostream>

#include <YGP/Log.h>
#include <YGP/Trace.h>
#include <YGP/IVIOAppl.h>
#include <YGP/StackTrc.h>

#include "Test.h"

class Application : public YGP::IVIOApplication {
 public:
   Application (const int argc, const char* argv[])
      : YGP::IVIOApplication (argc, argv, lo), cOptions (0), cErrors (0) { }
  ~Application () { }

 protected:
   virtual bool handleOption (const char option);

   // Program-handling
   virtual int         perform (int argc, const char* argv[]);
   virtual const char* name () const { return "Test/IVIOAppl"; }
   virtual const char* description () const { return PACKAGE_VERSION; }

   // Help-handling
   virtual void showHelp () const;

 private:
   // Prohobited manager functions
   Application ();
   Application (const Application&);
   const Application& operator= (const Application&);

   unsigned int cOptions;
   unsigned int cErrors;

   static const longOptions lo[];
};

const YGP::IVIOApplication::longOptions Application::lo[] = {
   { "help", 'h' },
   { "stackdump", 'd' },
   { "arg-opt", 'a' },
   { "arg-mand", 'A' },
   { NULL, '\0' } };


bool Application::handleOption (const char option) {
   ++cOptions;

   if ((option == 'a') || (option == 'A')) {
      const char* pValue = checkOptionValue ();
      check (pValue == getOptionValue ());
      if (option == 'A')
         check (pValue);

      TRACE1 ("Option param: " << pValue);
   } // endif special option

   if (option == 's') {
      std::cout << "Testing logging and stack-trace...\n";
      YGP::Syslog log ("Test");

      LOGDEBUG ("Test-Stacktrace:");
      dumpStack ();
   }

   return true;
}
void Application::showHelp () const {
  std::cout << "Usage: " << name () << " [<OPTION>] <ARGUMENT>\n\n"
               "  ARGUMENT ......... Argument (not starting with minus (-)\n"
               "  -a, --arg-opt .... Option with optional argument\n"
               "  -A, --arg-mand ... Option with mandatory argument\n\n"
               "The number of options and arguments are displayed\n";
}

int Application::perform (int argc, const char* argv[]) {
   std::cout << "Testing IVIOApplication...\n";
   std::cout << "    " << cOptions << " options and " << argc
             << " arguments passed\n";
   check (!argv[argc]);

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}

int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");                         // Activate current locale

   Application appl (argc, const_cast<const char**> (argv));
   return appl.run ();
}
