#ifndef YGP_IVIOAPPL_H
#define YGP_IVIOAPPL_H

//$Id: IVIOAppl.h,v 1.28 2008/05/18 13:21:27 markus Rel $

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


namespace YGP {

#define IVIOAPPL_HELP_OPTION    "help", 'h'

/**Class to handle the startup of a program, which includes reading the data
   stored in an INI file and (afterwards) the parsing of the parameters
   before the actual run of the program. Furthermore in case of a
   segmentation fault a stacktrace is dumped to the systemlog (or to stdout).

   The INI file is named according the name-method, with the following changes
   (depending on the operating system):
     -  UNIX: Prepend the name with the home directory of the user and make it
              "hidden", thus resulting in <tt>~/.name</tt>.
     - Windows: Prepend the name with the home directory (as specified by the
                environment variables HOMEDRIVE and HOMEPATH and append a .ini,
                thus resulting in <tt>%HOMEDRIVE%%HOMEPATH%name.ini</tt>.

   Parameters starting with a minus (-) (in Windoze also a with a slash (/))
   are treated as options and reported via the handleOption-method. Every
   option can also be represented in a verbose way starting with two
   minus-characters (--). What follows is only a short description; more
   information can be found in the getopt man page (which unfortunately can't
   be found on a Windows system, but such is life).

   The parameters are parsed from left to right. Each parameter is classified
   as a short option, a long option (both reported via the
   handleOption()-method), an argument to an option, or a non-option parameter.

   A simple short option is a minus (-) (or might be a slash (/) in Windoze)
   followed by a short option character. If the option has an argument, it may
   be written directly after the option character or as the next parameter
   (ie. separated by whitespace on the command line).

   It is possible to specify several short options after one minus (-), as
   long as all (except possibly the last) do not have arguments.

   A long option normally begins with `--' followed by the long option
   name. If the option has an argument, it may be written directly after the
   long option name, separated by an equal-sign (=), or as the next argument
   (ie. separated by whitespace on the command line). Long options may be
   abbreviated, as long as the abbreviation is not ambiguous.

   Every parameter which is not an option or an argument to an option or after
   a '--'-parameter is a non-option parameter and is passed to the
   perform()-method. Options are inspected before that method is called!

   \note: The options <tt>-h</tt> and <tt>-?</tt> causes the call of the
   showHelp()-method!

   <b>Programmers information</b>:

   Unlike in the UNIX getopt-function, optional paramters need not to be
   marked in a special way. Instead use the checkOptionValue()-method to
   retrieve the value of the possible argument, analyze it (like for having no
   leading minus (-)) and retrieve it with the getOptionValue()-method if OK.

   As a further feature a stackdump is logged in case of a protection fault.
*/
class IVIOApplication {
 public:
   /// Helper-structure to store long-options
   typedef struct {
      const char* longVal;        ///< Pointer to the value of the long option
      char  shortVal;  ///< Character representing the equivalent short option
   } longOptions;

   IVIOApplication (const int argc, const char* argv[],
                    const longOptions* pOpt = NULL);
   virtual ~IVIOApplication ();

   int run ();

   static void initI18n ();
   static void initI18n (const char* package, const char* dir);

 protected:
   // Option-handling
   /// Handle the options of the program. The current (parsed) option is passed.
   virtual bool handleOption (const char option) = 0;
   const char*  getOptionValue ();
   /// Returns the value to the current option, but without "consuming" it.
   /// This enables to check the value.
   const char*  checkOptionValue () const {
      return (pOptionParam && *pOptionParam) ? pOptionParam : ppArgs[startOpt + 1]; }
   void         setLongOptions (const longOptions* pLongOpts);
   void         setLongOptions (const longOptions* pLongOpts,
                                unsigned int numLongOpts);
   /// Parses some initialization data from a file specified by \c pFile.
   /// \param: Name of file to read
   virtual void readINIFile (const char*) { }

   /// \name Program-handling
   //@{
   /// Performs the job of the program.
   virtual int         perform (int argc, const char* argv[]) = 0;
   virtual const char* name () const;
   /// Returns a description to the program.
   virtual const char* description () const = 0;
   /// Returns the name of the program as passed by the operating system (argv[0]).
   const char* filename () const { return *ppArgs; }
   //@}

   /// \name Help-handling
   //@{
   /// Returns true, if a short programm information (name and release) should
   /// be displayed (default: Yes).
   virtual bool shallShowInfo () const { return true; }
   virtual void showHelp () const = 0;
   //@}

    unsigned int args;    ///< Number of arguments passed to the program (argc)
    const char** ppArgs;        ///< The arguments passed to the program (argv)

 private:
    // Prohobited manager functions
    IVIOApplication ();
    IVIOApplication (const IVIOApplication&);
    const IVIOApplication& operator= (const IVIOApplication&);

    char getOption ();
    void moveOption () const { moveOption (startOpt); }
    void moveOption (unsigned int numOpt) const;

    unsigned int startArg;
    unsigned int startOpt;
    const char*  pOptionParam;

    const longOptions* longOpt;
    unsigned int       numLongOpt;
};

}

#endif
