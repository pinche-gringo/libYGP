#ifndef IVIOAPPL_H
#define IVIOAPPL_H

//$Id: IVIOAppl.h,v 1.1 1999/07/31 00:15:08 Markus Exp $

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

// Class to handle the startup of a program
class IVIOApplication {
 public:
   // Manager functions
   IVIOApplication (const int argc, char* argv[]);
   virtual ~IVIOApplication () { }

   int run ();

 protected:
   typedef struct {
      char* longVal;
      char  shortVal;
   } longOptions;

   // Option-handling
   virtual bool handleOption (const char option) = 0;
   char*        getOptionValue ();
   void         setLongOptions (const longOptions* pLongOpts);
   void         setLongOptions (const longOptions* pLongOpts,
				unsigned int numLongOpts);

   // Program-handling
   virtual int         perform (int argc, char* argv[]) = 0;
   virtual const char* name () const = 0;
   virtual const char* description () const = 0;

   // Help-handling
   virtual bool shallShowInfo () const { return true; }
   virtual void showHelp () const = 0;

 private:
    // Prohobited manager functions
    IVIOApplication ();
    IVIOApplication (const IVIOApplication&);
    const IVIOApplication& operator= (const IVIOApplication&);

    char getOption ();

    unsigned int args;
    char**       ppArgs;

    unsigned int startArg;
    char*        pOptionParam;

    const longOptions* longOpt;
    unsigned int       numLongOpt;
};

#endif
