// $Id: Parse.cpp,v 1.12 2003/11/16 19:25:55 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test/Parse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.12 $
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

#include <ygp-cfg.h>

#include <clocale>

#include <YGP/Parse.h>
#include <YGP/Trace_.h>
#include <YGP/IVIOAppl.h>
#include <YGP/Internal.h>

#define VERBOSE
#undef  VERBOSE
#include "Test.h"

#if SYSTEM == UNIX
#  define PATH
#else
#  define PATH "..\\Common\\Tests\\"
#endif


static unsigned int cErrors (0);


class Application : public YGP::IVIOApplication {
 public:
   Application (const int argc, const char* argv[])
      : YGP::IVIOApplication (argc, argv, lo) { }
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

   static const longOptions lo[];

   static int foundNumber (const char* pNumber, unsigned int) {
      TRACE1 ("Found number: " << pNumber);
      return YGP::ParseObject::PARSE_OK;
   }

   static int foundQuotedText (const char* pText, unsigned int) {
      TRACE1 ("Found quoted text: " << pText);
      check (!strcmp (pText, "ab<cd>ef"));
      return YGP::ParseObject::PARSE_OK;
   }

   int foundAlpha (const char* pAlpha, unsigned int) {
      TRACE1 ("Found alpha: " << pAlpha);
      check (!strcmp (pAlpha, "5678"));
      return YGP::ParseObject::PARSE_OK;
   }

};

const YGP::IVIOApplication::longOptions Application::lo[] = {
   { "help", 'h' },
   { NULL, '\0' } };


bool Application::handleOption (const char option) {
   return true;
}
void Application::showHelp () const {
   std::cout << "Usage: " << name () << " [--help]\n\n";
}

int Application::perform (int argc, const char* argv[]) {
   std::cout << "Testing Parser...\n";
   YGP::CBParseAttomic nr ("\\9", "Number", foundNumber, 4, 2);
   YGP::OFParseAttomic<Application> alpha ("\\X", "Alphanum", *this,
                                      &Application::foundAlpha, 4, 2);
   YGP::ParseExact exact ("234", "234");
   YGP::ParseUpperExact upper ("9A42", "9A42");
   YGP::ParseObject* lstSeq[] = { &nr, &exact, NULL };
   YGP::ParseObject* lstSel[] = { &exact, &upper, NULL };
   YGP::ParseSequence seqANum (lstSeq, "Sequence-test");
   YGP::ParseSelection selANum (lstSel, "Selection-test");
   YGP::ParseText text ("2", "Text", 10);
   YGP::ParseTextEsc text2 ("34", "TextEsc", 10, 1, '2');
   YGP::ParseSkip skip (2);
   YGP::ParseQuoted qText ('"', "Quoted", 10, 1);
   YGP::CBParseQuotedEsc qEText ('<', "QuotedEsc", foundQuotedText, 10, 1);

   YGP::Xifstream xstr;
   xstr.open (PATH "Parser.test", std::ios::in);
   check (xstr);
   if (xstr) {
      xstr.init ();

      try {
         check (!(nr.parse ((YGP::Xistream&)xstr)));
         check (!(alpha.parse ((YGP::Xistream&)xstr)));
         check (!seqANum.parse ((YGP::Xistream&)xstr));
         check (!selANum.parse ((YGP::Xistream&)xstr));
         check (!text.parse ((YGP::Xistream&)xstr));
         check (!text2.parse ((YGP::Xistream&)xstr));
         check (xstr.getLine () == 3);
         check (xstr.getColumn () == 8);
         check (!skip.parse ((YGP::Xistream&)xstr));
         check (!qText.parse ((YGP::Xistream&)xstr));
         check (!qEText.parse ((YGP::Xistream&)xstr));
         check (xstr.getColumn () == 0);
	 // Don't check line number, is undefined after skipping
      } // end-try
      catch (std::string e) {
         std::cerr << "Error parsing Parser.test in line " << xstr.getLine () << " ("
                   << xstr.getColumn () << "): " << e.c_str () << '\n';
      } // end-catch
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
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
