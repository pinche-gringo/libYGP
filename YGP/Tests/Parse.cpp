// $Id: Parse.cpp,v 1.22 2008/06/11 17:53:40 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : Test/Parse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.22 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2008

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


#include <ygp-cfg.h>

#include <cstring>
#include <clocale>

#include <YGP/Parse.h>
#include <YGP/Trace.h>
#include <YGP/IVIOAppl.h>
#include <YGP/Internal.h>

#define VERBOSE
#undef  VERBOSE
#include "Test.h"

#if SYSTEM == UNIX
#  define PATH
#else
#  define PATH "..\\YGP\\Tests\\"
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
   virtual const char* description () const { return PACKAGE_VERSION; }

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
   YGP::ParseToText endMark ("**END**", "End-marker");
   YGP::ParseEOF eof;

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
	 // Don't check line number anymore, its undefined after skipping
         check (xstr.getColumn () == 0);

	 // Test ParseToText and ParseEOF
         check (!endMark.parse ((YGP::Xistream&)xstr));
         check (!eof.parse ((YGP::Xistream&)xstr));
      } // end-try
      catch (YGP::ParseError& e) {
         std::cerr << "Error parsing Parser.test in line " << xstr.getLine () << " ("
                   << xstr.getColumn () << "): " << e.what () << '\n';
      } // end-catch
   }

   if (cErrors)
      std::cout << "Failures: " << cErrors << '\n';
   return cErrors ? 1 : 0;
}

int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");
   bindtextdomain (LIBYGP_NAME, LOCALEDIR);   // Specify msg-file for gettext
   textdomain (LIBYGP_NAME);

   setlocale (LC_ALL, "");                         // Activate current locale

   Application appl (argc, const_cast<const char**> (argv));
   return appl.run ();
}
