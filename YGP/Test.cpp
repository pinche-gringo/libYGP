// $Id: Test.cpp,v 1.16 1999/09/11 00:59:40 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.16 $
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

#include <fstream.h>
#include <iostream.h>

#ifdef UNIX
#  include <strstream.h>
#else
#  include <strstrea.h>
#endif

#include "Parse.h"
#include <Handle.h>
#include "XStrBuf.h"
#include "XStream.h"
#include "DirSrch.h"
#include "ANumeric.h"
#include "IVIOAppl.h"
#include "PathSrch.h"
#include "Tokenize.h"
#include "FileRExp.h"


#define CHECK(x) { if (!(x)) cout << "    -> Failed (" #x "; line " << __LINE__ << ")\n"; }

#define VERBOSE
#undef VERBOSE
#ifdef VERBOSE
#  define check(x) { cout << "Checking: " #x "\n"; CHECK(x) }
#else
#  define check(x) CHECK(x)
#endif

class test {
 public:
   const char* n;
   test (const char* name = "???") : n (name) {
#ifdef VERBOSE
      cout << "Construct " << n << '\n';
#endif
   }
   ~test () {
#ifdef VERBOSE
      cout << "Destruct " << n << '\n';
#endif
   }
};


class Application : public IVIOApplication {
 public:
   Application (const int argc, char* argv[])
     : IVIOApplication (argc, argv, lo), cOptions (0) { }
  ~Application () { }

 protected:
   virtual bool handleOption (const char option);

   // Program-handling
   virtual int         perform (int argc, char* argv[]);
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

   static const longOptions lo[];

   int dirSearchRecursive (const char* pFile) const;
};


const IVIOApplication::longOptions Application::lo[] = {
   { "help", 'h' },
   { "arg-opt", 'a' },
   { "arg-mand", 'A' },
   { NULL, '\0' } };


bool Application::handleOption (const char option) {
   ++cOptions;

   if ((option == 'a') || (option == 'A')) {
      const char* pValue (checkOptionValue ());
      assert (pValue == getOptionValue ());
      if (option == 'A')
         check (pValue);
   } // endif special option     
   return true;
}
void Application::showHelp () const {
  cout << "Usage: " << name () << " [<OPTION>] <ARGUMENT>\n\n"
          "  ARGUMENT ......... Argument (not starting with minus (-)\n"
          "  -a, --arg-opt .... Option with optional argument\n"
          "  -A, --arg-mand ... Option with mandatory argument\n\n"
          "The number of options and arguments are displayed\n";
}

int Application::perform (int argc, char* argv[]) {
   cout << "Testing IVIOApplication...\n";
   cout << "    " << cOptions << " options and " << argc
        << " arguments passed\n";
   check (!argv[argc]);

   cout << "Testing Parser...\n";
   ParseAttomic  nr ("\\9", "Number", NULL, 4, 2);
   ParseAttomic  alpha ("\\X", "Alphanum", NULL, 4, 2);
   ParseExact exact ("234", "234");
   ParseUpperExact upper ("9A42", "9A42");
   ParseObject* lstSeq[] = { &nr, &exact, NULL };
   ParseObject* lstSel[] = { &exact, &upper, NULL };
   ParseSequence seqANum (lstSeq, "Sequence-test");
   ParseSelection selANum (lstSel, "Selection-test");
   ParseText text ("2", "Text", 10);
   ParseTextEsc text2 ("34", "TextEsc", 10, 1, '2');

   Xifstream xstr;
   xstr.open ("Test.Dat");
   check (xstr);
   if (xstr) {
      xstr.init ();

      check (!(nr.parse ((Xistream&)xstr)));
      check (!(alpha.parse ((Xistream&)xstr)));
      check (!seqANum.parse ((Xistream&)xstr));
      check (!selANum.parse ((Xistream&)xstr));
      check (!text.parse ((Xistream&)xstr));
      check (!text2.parse ((Xistream&)xstr));
      check (xstr.getLine () == 3);
      check (xstr.getColumn () == 8);
   }

#ifndef WINDOWS
// BCC has no way to set the read-buffer of an istream -> Skip test
   cout << "Testing extStreambuf...\n";

   ifstream in ("Test.Dat");
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
   xin.open ("Test.Dat");
   check (xin);
   if (xin) {
      char c;
      check (!xin.eof ());

      xin.init ();

      xin >> c >> c;
      check (xin.getLine () == 1);
      check (xin.getColumn () == 2);
   }

   cout << "Testing FileRegularExpr...\n";
   FileRegularExpr regExp ("a*b");
   check (!regExp.checkIntegrity () && regExp.matches ("ab"));

   regExp = "a[a-d]b";
   check (!regExp.checkIntegrity () && regExp.matches ("adb") && !regExp.matches ("axb"));

   regExp = "a[^xa-dy]b";
   check (!regExp.checkIntegrity () && regExp.matches ("afb") && !regExp.matches ("axb"));

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

   char numBuf[10];
   ostrstream stream (numBuf, sizeof (numBuf));
   stream << num2 << ends;
   check (strlen (numBuf) >= 4);

   cout << "Testing DirectorySearch...\n";
   DirectorySearch ds;
   dirEntry file;
   check (!ds.find ("./T*", &file, FILE_NORMAL | FILE_READONLY));
   check (!ds.find ());

   check (!ds.find ("./.*", &file, FILE_DIRECTORY | FILE_HIDDEN));
   check (!ds.find ());
   check (!ds.find ());
   check (!ds.find ());
   check (ds.find ());

   check (!ds.find (".", &file, FILE_DIRECTORY | FILE_HIDDEN));
   check (!ds.find ("../General/", &file, FILE_DIRECTORY));
   check (!ds.find ("../General", &file, FILE_DIRECTORY));
   check (ds.find ());

   check (!dirSearchRecursive (NULL));

   cout << "Testing Tokenize...\n";
   Tokenize token ("/usr/include/std/");
   check (token.getActNode () == "/usr/include/std/");
   check (token.getNextNode ('/') == "usr");
   check (token.getNextNode ('/') == "include");
   check (token.getNextNode ('/') == "std");
 
   cout << "Testing PathSearch...\n";
   PathSearch ps (".:..:/::/usr/:/usr");
   check (ps.getActNode () == ".:..:/::/usr/:/usr");
   check (ps.getNextNode () == ".");
   check (ps.getNextNode () == "..");
   check (ps.getNextNode () == "/");
   check (ps.getNextNode () == "/usr/");
   check (ps.getNextNode () == "/usr");

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
   return 0;
}


int Application::dirSearchRecursive (const char* pFile) const {
   DirectorySearch ds (pFile ? "T*" : "D*");
   dirEntry file;

   if (ds.find (&file, FILE_NORMAL))
      return 1;

   return pFile ? dirSearchRecursive (NULL) : 0;
}


int main (int argc, char* argv[]) {
   Application appl (argc, argv);
   return appl.run ();
}
