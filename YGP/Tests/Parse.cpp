// $Id: Parse.cpp,v 1.2 2002/04/27 19:05:32 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : Test/Parse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Anticopyright (A) 2001

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
#include <locale.h>

#include <Parse.h>
#include <Trace_.h>
#include <IVIOAppl.h>

#include <Internal.h>
#include "Test.h"


class Application : public IVIOApplication {
 public:
   Application (const int argc, const char* argv[])
      : IVIOApplication (argc, argv, lo), cErrors (0) { }
  ~Application () { }

 protected:
   virtual bool handleOption (const char option);

   // Program-handling
   virtual int         perform (int argc, const char* argv[]);
   virtual const char* name () const { return "Test/Parse"; }
   virtual const char* description () const { return VERSION; }

   // Help-handling
   virtual void showHelp () const;

 private:
   // Prohobited manager functions
   Application ();
   Application (const Application&);
   const Application& operator= (const Application&);

   unsigned int cErrors;

   static const longOptions lo[];

   static int foundNumber (const char* pNumber) {
      TRACE1 ("Found number: " << pNumber);
      return ParseObject::PARSE_OK;
   }

   int foundAlpha (const char* pAlpha) {
      TRACE1 ("Found alpha: " << pAlpha);
      return ParseObject::PARSE_OK;
   }

};

const IVIOApplication::longOptions Application::lo[] = {
   { "help", 'h' },
   { NULL, '\0' } };


bool Application::handleOption (const char option) {
   return true;
}
void Application::showHelp () const {
   cout << "Usage: " << name () << " [--help]\n\n";
}

int Application::perform (int argc, const char* argv[]) {
   cout << "Testing Parser...\n";
   CBParseAttomic nr ("\\9", "Number", foundNumber, 4, 2);
   OFParseAttomic<Application> alpha ("\\X", "Alphanum", *this,
                                      &Application::foundAlpha, 4, 2);
   ParseExact exact ("234", "234");
   ParseUpperExact upper ("9A42", "9A42");
   ParseObject* lstSeq[] = { &nr, &exact, NULL };
   ParseObject* lstSel[] = { &exact, &upper, NULL };
   ParseSequence seqANum (lstSeq, "Sequence-test");
   ParseSelection selANum (lstSel, "Selection-test");
   ParseText text ("2", "Text", 10);
   ParseTextEsc text2 ("34", "TextEsc", 10, 1, '2');

   Xifstream xstr;
   xstr.open ("Parsertest.dat", ios::in | ios::nocreate);
   check (xstr);
   if (xstr) {
      xstr.init ();

      try {
         check (!(nr.parse ((Xistream&)xstr)));
         check (!(alpha.parse ((Xistream&)xstr)));
         check (!seqANum.parse ((Xistream&)xstr));
         check (!selANum.parse ((Xistream&)xstr));
         check (!text.parse ((Xistream&)xstr));
         check (!text2.parse ((Xistream&)xstr));
         check (xstr.getLine () == 3);
         check (xstr.getColumn () == 8);
      } // end-try
      catch (std::string e) {
         cerr << "Error parsing Parsertest.dat in line " << xstr.getLine () << " ("
              << xstr.getColumn () << "): " << e.c_str () << '\n';
      } // end-catch
   }

   if (cErrors)
      cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}

int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);     // Specify messagefile for gettext
   textdomain (PACKAGE);

   setlocale (LC_ALL, "");                         // Activate current locale

   Application appl (argc, const_cast<const char**> (argv));
   return appl.run ();
}
