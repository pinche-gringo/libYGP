#ifndef IVIOAPPL_H
#define IVIOAPPL_H

//$Id: IVIOAppl.h,v 1.19 2002/12/09 00:12:01 markus Rel $

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


// Class to handle the startup of a program, which includes reading the data
// stored in an INI file and (afterwards) the parsing of the parameters
// before the actual run of the program. Furthermore in case of a
// segmentation fault a stacktrace is dumped to the systemlog (or to stdout).
//
// Parameters starting with a minus (-) or (only in Windoze) with a slash
// (/) are treated as options and reported via the handleOption-method. Every
// option can also be represented in a verbose way starting with two
// minus-characters (--). What follows is only a short description; more
// information can be found in the getopt man page (which
// unfortunately can't be found on a Windows system, but such is life).
//
// The parameters are parsed from left to right. Each parameter is
// classified as a short option, a long option (both reported via the
// handleOption-method), an argument to an option, or a non-option
// parameter.
//
// >A simple short option is a minus (-) or (only in Windoze) a slash
// (/) followed by a short option character. If the option has an
// argument, it may be written directly after the option character or
// as the next parameter (ie. separated by whitespace on the command
// line).
//
// It is possible to specify several short options after one minus
// (-), as long as all (except possibly the last) do not have
// arguments.
//
// A long option normally begins with `--' followed by the long option
// name. If the option has an argument, it may be written directly
// after the long option name, separated by an equal-sign (=), or as
// the next argument (ie. separated by whitespace on the command
// line). Long options may be abbreviated, as long as the abbreviation
// is not ambiguous.
//
// Every parameter which is not an option or an argument to an option
// or after a '--'-parameter is a non-option parameter and are passed
// to the perform-method. Options are inspected before that method is called!
//
// Note: The options "-h" and "-?" causes the call of the showHelp-method!
//
// Programmers information:
//
// Unlike in the UNIX getopt-function, optional paramters need not to be
// marked in a special way. Instead use the checkOptionValue-method
// to retrieve the value of the possible argument, analyze it (like for having
// no leading minus (-)) and retrieve it with the getOptionValue-method if OK.


#define IVIOAPPL_HELP_OPTION    "help", 'h'

class IVIOApplication {
 public:
   // Helper-structure to store long-options
   typedef struct {
      char* longVal;
      char  shortVal;
   } longOptions;

   // Manager functions
   IVIOApplication (const int argc, const char* argv[],
                    const longOptions* pOpt = NULL);
   virtual ~IVIOApplication ();

   int run ();

   static void initI18n ();
   static void initI18n (const char* package, const char* dir);

 protected:
   // Option-handling
   virtual bool handleOption (const char option) = 0;
   const char*  getOptionValue ();
   const char*  checkOptionValue () const {
      return (pOptionParam && *pOptionParam) ? pOptionParam : ppArgs[startOpt + 1]; }
   void         setLongOptions (const longOptions* pLongOpts);
   void         setLongOptions (const longOptions* pLongOpts,
                                unsigned int numLongOpts);
   virtual void readINIFile (const char* pFile) { }

   // Program-handling
   virtual int         perform (int argc, const char* argv[]) = 0;
   virtual const char* name () const;
   virtual const char* description () const = 0;
   const char* filename () const { return *ppArgs; }

   // Help-handling
   virtual bool shallShowInfo () const { return true; }
   virtual void showHelp () const = 0;

 private:
    // Prohobited manager functions
    IVIOApplication ();
    IVIOApplication (const IVIOApplication&);
    const IVIOApplication& operator= (const IVIOApplication&);

    char getOption ();
    void moveOption () const { moveOption (startOpt); }
    void moveOption (unsigned int numOpt) const;

    unsigned int args;
    const char** ppArgs;

    unsigned int startArg;
    unsigned int startOpt;
    const char*  pOptionParam;

    const longOptions* longOpt;
    unsigned int       numLongOpt;
};

#endif
