//$Id: PathSrch.cpp,v 1.7 2003/02/13 07:17:13 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : PathSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.7 $
//AUTHOR      : Markus Schwab
//CREATED     : 9.5.2002
//COPYRIGHT   : Anticopyright (A) 2002

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


#include <gzo-cfg.h>

#if SYSTEM == UNIX
#include <pwd.h>
#endif
#include <ctype.h>
#include <stdlib.h>

#include "Trace_.h"

#include "File.h"
#include "PathSrch.h"

#if SYSTEM == UNIX
const char PathSearch::PATHSEPARATOR = ':';
#elif SYSTEM == WINDOWS
const char PathSearch::PATHSEPARATOR = ';';
#else
#  error Unsupported plattform!
#endif


/*--------------------------------------------------------------------------*/
//Purpose   : Performs tilde-expansion on the input.
//
//            The expansion is only done under the following conditions:
//              - The string starts with a tilde
//              - None of the characters up to the next whitespace or slash (/)
//                is quoted
//              - The following characters (if any) specify a user-name
//
//            If those conditions apply, a single tilde is replaced with the
//            contents of the HOME-environment variable (UNIX) or the combination
//            of HOMEDRIVE and HOMEPATH (Windows). A tilde with following name
//            returns the HOME-path of the user with that name (or the input, if
//            such a user does not exist
//Parameters: input: String to expand
//Returns   : std::string: Expanded string or input (if not expandable)
/*--------------------------------------------------------------------------*/
std::string PathSearch::expandNode (const std::string& input) {
   TRACE9 ("PathSearch::expandNode (const std::string&) - " << input);

   if (input.empty () || input[0] != '~')
      return input;

   unsigned int i (1);
   for (; i < input.length (); ++i) {
      if (isspace (input[i]) || (input[i] == File::DIRSEPARATOR))
         break;

#if SYSTEM == UNIX
      if (input[i] == '\\')                     // Don't expand quoted strings
         return input;
#endif
   } // endfor

   std::string ret (input);
   if (i == 1) {                // No name after
#if SYSTEM == UNIX
      const char* user = getenv ("HOME");
      if (user)
         ret.replace (0, 1, user);
#else
      const char* env = getenv ("HOMEDRIVE");
      if (env) {
         ret.replace (0, 1, env);
         i = strlen (env);
      }
      else
         ret.replace (0, 1, i = 0, '\0');

      env = getenv ("HOMEPATH");
      if (env)
         ret.replace (i, 0, env);
#endif
   }
#if SYSTEM == UNIX
   else {
      std::string user (input.substr (1, i));
      TRACE5 ("PathSearch::expandNode (const std::string&) - Expanding user "
              << user);

      struct passwd* entry (getpwnam (user.c_str ()));
      if (entry)
         ret.replace (0, i, entry->pw_dir);
   }
#endif
   TRACE1 ("PathSearch::expandNode (const std::string&) - Expanded user "
           << ret);
   return ret;
}
