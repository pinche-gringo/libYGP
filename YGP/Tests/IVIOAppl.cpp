// $Id: IVIOAppl.cpp,v 1.6 2003/09/11 04:19:13 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/IVIOAppl
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Anticopyright (A) 2001 - 2003

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


#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif


#include <clocale>

#include <iostream>

#include <Log.h>
#include <Check.h>
#include <Trace_.h>
#include <IVIOAppl.h>
#include <StackTrc.h>

#include "Test.h"

class Application : public IVIOApplication {
 public:
   Application (const int argc, const char* argv[])
      : IVIOApplication (argc, argv, lo), cOptions (0), cErrors (0) { }
  ~Application () { }

 protected:
   virtual bool handleOption (const char option);

   // Program-handling
   virtual int         perform (int argc, const char* argv[]);
   virtual const char* name () const { return "Test/IVIOAppl"; }
   virtual const char* description () const { return VERSION; }

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

const IVIOApplication::longOptions Application::lo[] = {
   { "help", 'h' },
   { "stackdump", 'd' },
   { "arg-opt", 'a' },
   { "arg-mand", 'A' },
   { NULL, '\0' } };


bool Application::handleOption (const char option) {
   ++cOptions;

   if ((option == 'a') || (option == 'A')) {
      const char* pValue = checkOptionValue ();
      Check1 (pValue == getOptionValue ());
      if (option == 'A')
         check (pValue);

      TRACE1 ("Option param: " << pValue);
   } // endif special option

   if (option == 's') {
      std::cout << "Testing logging and stack-trace...\n";
      Syslog log ("Test");

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
