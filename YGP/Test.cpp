// $Id: Test.cpp,v 1.11 1999/08/24 23:46:11 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : Test
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.11 $
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
#include <strstream.h>

#include "Parse.h"
#include "XStrBuf.h"
#include "XStream.h"
#include "DirSrch.h"
#include "ANumeric.h"
#include "IVIOAppl.h"
#include "PathSrch.h"
#include "Tokenize.h"
#include "FileRExp.h"


#define check(x) { if (!(x)) cout << "    -> Failed (" #x "; line " << __LINE__ << ")\n"; }


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
   ParseAttomic nr ("\\9", "Number", 5);
   ParseAttomic alpha ("\\X", "Alphanum", 5);

   Xifstream xstr;
   xstr.open ("Test.Dat");
   xstr.init ();

   check (!(nr.parse ((Xistream&)xstr)));
   check (!(alpha.parse ((Xistream&)xstr)));
   check (xstr.getLine () == 2);
   check (xstr.getColumn () == 0);

   cout << "Testing extStreambuf...\n";

   ifstream in ("Test.Dat");
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
      }
      assert (buffer + sizeof (buffer) > pAct);
      *pAct++ = c;
      ++forAlpha;
   } // end-while

   while ((in.get (c)), in)
      ++afterAlpha;

   check ((forAlpha == 16) && (afterAlpha == 12));
   check (str.getLine () == 5);
   check (str.getColumn () == 0);

   cout << "Testing XStream...\n";
   
   Xifstream xin;
   xin.open ("Test.Dat");
   check (!xin.eof ());

   xin.init ();

   xin >> c >> c;
   check (xin.getLine () == 1);
   check (xin.getColumn () == 2);

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

   ANumeric   num2 (2000);
   check (num2.isDefined ());
   num = num2;
   check (num.isDefined ());

   char       numBuf[10];
   ostrstream stream (numBuf, sizeof (numBuf));
   stream << num2 << ends;
   check (strlen (numBuf) <= 4);

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

   return 0;
}

int main (int argc, char* argv[]) {
   Application appl (argc, argv);
   return appl.run ();
}
