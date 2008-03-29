//$Id: PathSrch.cpp,v 1.16 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : PathSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.16 $
//AUTHOR      : Markus Schwab
//CREATED     : 9.5.2002
//COPYRIGHT   : Copyright (C) 2002 - 2004, 2008

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

#if (SYSTEM == UNIX) && defined (HAVE_PWD_H)
#  include <pwd.h>
#endif
#include <cctype>
#include <cstdlib>

#include "YGP/Trace.h"

#include "YGP/File.h"
#include "YGP/PathSrch.h"


namespace YGP {

#if SYSTEM == UNIX
const char PathSearch::PATHSEPARATOR = ':';
#elif SYSTEM == WINDOWS
const char PathSearch::PATHSEPARATOR = ';';
#else
#  error Unsupported plattform!
#endif


//-----------------------------------------------------------------------------
/// Performs tilde-expansion on the input. The expansion is only done under
/// the following conditions:
///    - The string starts with a tilde
///    - None of the characters up to the next whitespace or slash (/) is quoted
///    - The following characters (if any) specify a user-name
/// If those conditions apply, a single tilde is replaced with the contents of
/// the <tt>HOME</tt>-environment variable (UNIX) or the combination of
/// <tt>HOMEDRIVE</tt> and <tt>HOMEPATH</tt> (Windows). A tilde with following
/// name returns the <tt>HOME</tt>-path of the user with that name (or the
/// input, if such a user does not exist
/// \param input: String to expand
/// \returns \c std::string: Expanded string or input (if not expandable)
//-----------------------------------------------------------------------------
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
#if (SYSTEM == UNIX) && defined (HAVE_PWD_H)
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

}
