// $Id: Test.cpp,v 1.44 2001/01/19 14:38:48 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.44 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.7.1999
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


#include <ctype.h>
#include <assert.h>

#include <locale.h>

#include <fstream.h>
#include <iostream.h>

#include <gzo-cfg.h>

#if SYSTEM == UNIX || defined __GNUG__
#  include <strstream.h>
#else
#  include <strstrea.h>
#endif


#define ERROROUT(x) cout << "    -> Failed (" << x << "; line " << __LINE__ << ")\n";
#define CHECK(x) { if (!(x)) { rc++; ERROROUT (#x) } }

#define VERBOSE
#undef VERBOSE

#ifdef VERBOSE
#  define DEBUG 1
#  define check(x) { cout << "Checking: " #x "\n"; CHECK(x) }
#else
#  define DEBUG 0
#  define check(x) CHECK(x)
#endif


#include "Log.h"
#include "Parse.h"
#include "Trace_.h"
#include "CRegExp.h"
#include "Handle.h"
#include "ATStamp.h"
#include "XStrBuf.h"
#include "XStream.h"
#include "DirSrch.h"
#include "INIFile.h"
#include "ANumeric.h"
#include "IVIOAppl.h"
#include "PathSrch.h"
#include "Tokenize.h"
#include "FileRExp.h"
#include "StackTrc.h"
#include "PathDirSrch.h"


class test {
 public:
   const char* n;
   test (const char* name = "???") : n (name) {
      TRACE1 ("Construct " << n);
   }
   ~test () {
      TRACE1 ("Destruct " << n);
   }
};


class Application : public IVIOApplication {
 public:
   Application (const int argc, const char* argv[])
      : IVIOApplication (argc, argv, lo), cOptions (0), regexp ("")
      , strRE (""), rc (0) { }
  ~Application () { }

 protected:
   virtual bool handleOption (const char option);

   // Program-handling
   virtual int         perform (int argc, const char* argv[]);
   virtual const char* name () const { return "Test"; }
   virtual const char* description () const { return VERSION; }

   // Help-handling
   virtual void showHelp () const;

 private:
   // Prohobited manager functions
   Application ();
   Application (const Application&);
   const Application& operator= (const Application&);

   unsigned int cOptions;
   std::string strRE, strVal;
   RegularExpression regexp;;
   bool match;

   int rc;

   static const longOptions lo[];

   static int foundNumber (const char* pNumber) {
      TRACE1 ("Found number: " << pNumber);
      return ParseObject::PARSE_OK;
   }

   void removeEscapeChar (std::string& str, char esc = '\\') {
      int pos (0);
      while ((pos = str.find (esc, pos)) != -1)
	str.replace (pos++, 1, 0, '\0');
   }

   int foundRegExp (const char* pRegExp) {
      assert (pRegExp);
      TRACE1 ("Found regular expression: " << pRegExp);
      strRE = pRegExp;
      removeEscapeChar (strRE);
      regexp = strRE.c_str ();
      return ParseObject::PARSE_OK;
   }

   int foundValue (const char* pValue) {
      assert (pValue);
      check (!strRE.empty ());
      TRACE1 ("Found value: " << pValue);
      strVal = pValue;
      removeEscapeChar (strVal);
      match = regexp.matches (pValue);
      return ParseObject::PARSE_OK;
  }

   int foundResult (const char* pResult) {
      assert (pResult);
      if ((*pResult != '0') != match)
         ERROROUT ("RegExp (\"" << strRE.c_str () << "\").matches (\""
		   << strVal.c_str () << "\") = " << pResult);

      return ParseObject::PARSE_OK;
  }

   int foundAlpha (const char* pAlpha) {
      TRACE1 ("Found alpha: " << pAlpha);
      return ParseObject::PARSE_OK;
   }

   int dirSearchRecursive (const char* pFile) const;
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
      assert (pValue == getOptionValue ());
      if (option == 'A')
         check (pValue);

      TRACE1 ("Option param: " << pValue);
   } // endif special option

   if (option == 's') {
      Syslog log ("Test");

      cout << "Performing stacktrace\n";
      LOGDEBUG ("Test-Stacktrace:");
      dumpStack ();
   }

   return true;
}
void Application::showHelp () const {
  cout << "Usage: " << name () << " [<OPTION>] <ARGUMENT>\n\n"
          "  ARGUMENT ......... Argument (not starting with minus (-)\n"
          "  -a, --arg-opt .... Option with optional argument\n"
          "  -A, --arg-mand ... Option with mandatory argument\n\n"
          "The number of options and arguments are displayed\n";
}

int Application::perform (int argc, const char* argv[]) {
   cout << "Testing IVIOApplication...\n";
   cout << "    " << cOptions << " options and " << argc
        << " arguments passed\n";
   check (!argv[argc]);

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
   xstr.open ("Test.Dat", ios::in | ios::nocreate);
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
         cerr << "Error parsing Test.Dat in line " << xstr.getLine () << " ("
              << xstr.getColumn () << "): " << e.c_str () << '\n';
      } // end-catch
   }

#ifndef WINDOWS
// BCC & MVC have no way to set the read-buffer of an istream -> Skip test
   cout << "Testing extStreambuf...\n";

   ifstream in ("Test.Dat", ios::in | ios::nocreate);
   check (in);
   if (in) {
      extStreambuf str (*in.rdbuf ());
      in.ios::rdbuf (&str);
      char c;

      char buffer[20], *pAct (buffer);

      int forAlpha (0), afterAlpha (0);

      while ((in.get (c)), in) {
         if (isalpha (c)) {
            check (str.getLine () == 3);
            check (str.getColumn () == 2);
            assert (pAct + 4 >= buffer);
            in.putback (c);
            in.putback (*--pAct);
            in.putback (*--pAct);
            in.putback (*--pAct);
            break;
         } // endif alpha found
         assert (buffer + sizeof (buffer) > pAct);
         *pAct++ = c;
         ++forAlpha;
      } // end-while 

      while ((in.get (c)), in)
         ++afterAlpha;

      check ((forAlpha == 16) && (afterAlpha == 12));
      check (str.getLine () == 5);
      check (str.getColumn () == 0);
   } 
#endif

   cout << "Testing XStream...\n";
   
   Xifstream xin;
   xin.open ("Test.Dat", ios::in | ios::nocreate);
   check (xin);
   if (xin) {
      char c;
      check (!xin.eof ());

      xin.init ();

      xin >> c >> c;
      check (xin.getLine () == 1);
      check (xin.getColumn () == 2);
   }

   cout << "Testing RegularExpression...\n";
   Xifstream frexexp;
   frexexp.open ("Test.rex", ios::in | ios::nocreate);
   check (frexexp);
   if (frexexp) {
      frexexp.init ();

      try {
         typedef OFParseTextEsc<Application> OMParseTextEsc;
         typedef OFParseAttomic<Application> OMParseAttomic;

         ParseObject*   _RegExpTest[3];
         ParseSelection RegExpTest (_RegExpTest, "Regular expression-file", -1, 0);
         ParseObject*   _RegExpHeader[4];
         ParseSequence  RegExpHeader (_RegExpHeader, "Regexp-header", 1, 1);
         ParseObject*   _values[4];
         ParseSequence  values (_values, "Values", 1, 1);
         OMParseTextEsc RegExp ("]", "Regular expression", *this,
                                &Application::foundRegExp, 512, 1, '\\', false);
         OMParseTextEsc value ("=\n\r", "Value", *this, &Application::foundValue,
                               512, 1, false);
         OMParseAttomic result ("\\9", "Result", *this,
                                &Application::foundResult, 1, 1);
         ParseExact RegExpBegin ("[", "Begin of regexp ([)", false);
         ParseExact RegExpEnd ("]", "End of regexp (])");
         ParseExact equals ("=", "Equal-sign (=)", false);

         _RegExpTest[0] = &RegExpHeader; _RegExpTest[1] = &values;
         _RegExpTest[2] = NULL;
         _RegExpHeader[0] = &RegExpBegin; _RegExpHeader[1] = &RegExp;
         _RegExpHeader[2] = &RegExpEnd; _RegExpHeader[3] = NULL;
         _values[0] = &value; _values[1] = &equals; _values[2] = &result;
	 _values[3] = NULL;

         RegExpTest.parse ((Xistream&)frexexp);
      }
      catch (std::string& e) {
	 cerr << e.c_str () << "\nActual position: Test.rex - "
              << ANumeric (frexexp.getLine ()).toString ().c_str () << '/'
              << ANumeric (frexexp.getColumn ()).toString ().c_str () << '\n';
      }
   } // endif

   cout << "Testing FileRegularExpr...\n";
   FileRegularExpr regExp ("a*b");
   check (!regExp.checkIntegrity () && regExp.matches ("ab"));

   regExp = "a[a-d]b";
   check (!regExp.checkIntegrity () && regExp.matches ("adb") && !regExp.matches ("axb"));

   regExp = "a[^xa-dy]b";
   check (!regExp.checkIntegrity () && regExp.matches ("afb") && !regExp.matches ("axb"));

   regExp = "[[:alpha:]]";
   check (!regExp.checkIntegrity ());
   check (regExp.matches ("x"));
   check (!regExp.matches (":"));

   cout << "Testing ADate...\n";
   ADate today;
   check (!today.isDefined ());
   ADate past (false);
   check (past.isDefined ());
   check (today < past);

   today = ADate::today ();
   check (today.isDefined ());
   check (today > past);

   today.sub (0, 1);
   check (today < ADate::today ());
   today.add (0, 1);
   check (today == ADate::today ());
   today.sub (0, 1);
   today -= ADate::today ();                 // Results in 0.11.-1 => 31.10.-1

   today.setMonth (1);
   today.sub (0, 2);
   check (today.getMonth () == 10);

   today.add (0, 3);
   check (today.getMonth () == 1);

   today.setYear (2000);
   check (today.isLeapYear ());
   today.add (0, 0, 1);
   check (!today.isLeapYear ());

   check (!past.isLeapYear ());
   past.add (0, 0, 4);
   check (past.isLeapYear ());
   
   cout << "Testing ATime...\n";
   ATime act;
   check (!act.isDefined ());
   ATimestamp first (false);
   check (first.isDefined ());
   check (act < first);

   act = ATimestamp::now ();
   check (act.isDefined ());
   check (act > first);
 
   cout << "Testing ATimestamp...\n";
   ATimestamp now;
   check (!now.isDefined ());
   ATimestamp early (false);
   check (early.isDefined ());
   check (now < early);

   now = ATimestamp::now ();
   check (now.isDefined ());
   check (now > early);

   cout << "Testing ANumeric...\n";
   ANumeric num;
   check (!num.isDefined ());

   ANumeric num2 (-2000);
   check (num2.isDefined ());
   check (num2 < num);
   num2 = 0;
   check (num2 > num);
   num2 += 2000;
   ANumeric num3 (num2 + num);
   check (num3 == num2);
   num3 = num2 / num;
   check (num3 == num2);
   num = num2;
   check (num.isDefined ());
   check (num == num2);

   std::string numStr (num2.toString ());
   check (numStr.length () >= 4);

   cout << "Testing Tokenize...\n";
   Tokenize token ("/usr/include/std/");
   check (token.getActNode () == "/usr/include/std/");
   check (token.getNextNode ('/') == "usr");
   check (token.getNextNode ('/') == "include");
   check (token.getNextNode ('/') == "std");
 
   cout << "Testing DirectorySearch...\n";
   DirectorySearch ds;
   dirEntry file;
   check (!ds.find ("T*", file, DirectorySearch::FILE_NORMAL
                                  | DirectorySearch::FILE_READONLY));
   check (!ds.find ());

   check (!ds.find (".*", file, DirectorySearch::FILE_DIRECTORY
                                  | DirectorySearch::FILE_HIDDEN));
   check (!ds.find ());
   check (!ds.find ());
   check (!ds.find ());
   check (ds.find ());

   check (!ds.find (".", file, DirectorySearch::FILE_DIRECTORY
                               | DirectorySearch::FILE_HIDDEN));
   std::string temp ("..");
   temp += DirectorySearch::getSplitChar ();
   temp += "Common";
   temp += DirectorySearch::getSplitChar ();
   check (!ds.find (temp.c_str (), file, DirectorySearch::FILE_DIRECTORY));
   temp = "..";
   temp += DirectorySearch::getSplitChar ();
   temp += "X-windows";
   check (!ds.find (temp.c_str (), file, DirectorySearch::FILE_DIRECTORY));
   check (ds.find ());

   check (!dirSearchRecursive (NULL));

   check (ds.find ("CVS", file, DirectorySearch::FILE_NORMAL));

   cout << "Testing PathDirectorySearch...\n";
#if SYSTEM == UNIX
   PathDirectorySearch pds (".:../X-windows", "Makefile.*");
#else
   PathDirectorySearch pds (".;..\\X-windows", "Makefile.*");
#endif
   check (!pds.find (file, DirectorySearch::FILE_NORMAL
                           | DirectorySearch::FILE_READONLY));
   check (!pds.find ());
   check (!pds.find ());
   check (!pds.find ());
   check (pds.find ());

   cout << "Testing PathSearch...\n";
#if SYSTEM == UNIX
   PathSearch ps (".:..:/::/usr/:/usr");
   check (ps.getActNode () == ".:..:/::/usr/:/usr");
   check (ps.getNextNode () == ".");
   check (ps.getNextNode () == "..");
   check (ps.getNextNode () == "/");
   check (ps.getNextNode () == "/usr/");
   check (ps.getNextNode () == "/usr");
#else
   PathSearch ps (".;..;\\;;\\usr\\;\\usr");
   check (ps.getActNode () == ".;..;\\;;\\usr\\;\\usr");
   check (ps.getNextNode () == ".");
   check (ps.getNextNode () == "..");
   check (ps.getNextNode () == "\\");
   check (ps.getNextNode () == "\\usr\\");
   check (ps.getNextNode () == "\\usr");
#endif
   // This test should be the last (to see result of cleanup close to result
   // of create). If you dont want that put a block around this context.
   cout << "Testing SmartPtr & Handle...\n";
   definePtr (test);
   Ptest pTest (new test ("SmartPtr"));
   defineHndl (test);
   Htest hHandle;
   check (!hHandle.isDefined ());

   hHandle.define ();
   check (hHandle.isDefined ());
   hHandle->n = "Handle";
   check (hHandle.isDefined ());
   hHandle.undefine ();
   check (!hHandle.isDefined ());

   Htest hHandle2 (new test ("Handle2"));
   check (hHandle2.isDefined ());
   hHandle2 = hHandle;
   check (!hHandle2.isDefined ());

   cout << "Testing INI-file parser...\n";
   int Attr1;
   std::string Attr2;
   ANumeric Attr3;
   ADate Attr4;
   Xifstream inifile;
   ATime Attr5;
   ATimestamp Attr6;
   try {
      INISection global ("Global");
      INIAttribute<int> attr1 ("Attr1", Attr1);
      INIAttribute<std::string> attr2 ("Attr2", Attr2);
      INIAttribute<ANumeric> attr3 ("Attr3", Attr3);
      INIAttribute<ADate> attr4 ("Attr4", Attr4);
      global.addAttribute (attr1);
      global.addAttribute (attr2);
      global.addAttribute (attr3);
      global.addAttribute (attr4);

      inifile.open ("Test.ini", ios::in | ios::nocreate);
      check (inifile);

      if (inifile) {
         INIFILE ("Test.ini");
         INISECTION (Special);
         INIATTR (Special, ATime, Attr5);
         INIATTR (Special, ATimestamp, Attr6);

         INISECTION (Local);
         INIATTR (Local, int, Attr1);
         std::string attr2_;
         INIATTR2 (Local, std::string, attr2_, Attr2);
         INIATTR (Local, ANumeric, Attr3);
         INIATTR (Local, ADate, Attr4);

         _inifile_.addSection (global);

         inifile.init ();
         try {
            int rc = global.readFromStream ((Xistream&)inifile);
            check (!rc);
         } // end-try
         catch (std::string& e) {
            throw (std::string ("Error parsing Test.ini in line ")
                   + ANumeric (inifile.getLine ()).toString ()
                   + std::string (" (") + ANumeric (inifile.getColumn ()).toString ()
                   + std::string ("): ") + e);
         } // end-catch

         int rc = INIFILE_READ ();
         check (!rc);
      } // endif
   } // end-try
   catch (std::string& e) {
      cerr << e.c_str () << '\n';
   }

   return rc;
}


int Application::dirSearchRecursive (const char* pFile) const {
   DirectorySearch ds (pFile ? "T*" : "D*");
   dirEntry file;

   if (ds.find (file, DirectorySearch::FILE_NORMAL))
      return 1;

   return pFile ? dirSearchRecursive (NULL) : 0;
}


int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");                         // Activate current locale

   Application appl (argc, const_cast<const char**> (argv));
   return appl.run ();
}
