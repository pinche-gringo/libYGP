#ifndef IVIOAPPL_H
#define IVIOAPPL_H

//$Id: IVIOAppl.h,v 1.13 2001/02/18 23:20:31 Markus Exp $

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


// Class to handle the startup of a program
//
// The main purpose/advantage of this class is its parameter-handling.
// Parameters starting with a minus (-) or (only in Windoze) with a slash
// (/) are treated as options and reported via the handleOption-method. Every
// option can also represented in a verbose way starting with two
// minus-characters (--).
//
// Options can be passed before or after the other parameters and are
// handled before the "real" program starts to run. To end further
// option-handling two minus-characters (--) without following text can be
// used.
//
// Two ore more options can be put together after a single option-character.
// e.g.: "-d -v" is equal to "-dv" (and also to "--debug --verbose" if those
// long-values are associated with the options).
//
// Note: The option "-h" causes the call of the showHelp-method!


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

 protected:
   // Option-handling
   virtual bool handleOption (const char option) = 0;
   const char*  getOptionValue ();
   const char*  checkOptionValue () const {
      return (pOptionParam && *pOptionParam) ? pOptionParam : ppArgs[startOpt + 1]; }
   void         setLongOptions (const longOptions* pLongOpts);
   void         setLongOptions (const longOptions* pLongOpts,
                                unsigned int numLongOpts);

   // Program-handling
   virtual int         perform (int argc, const char* argv[]) = 0;
   virtual const char* name () const { return filename (); }
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
