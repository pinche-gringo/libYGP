#ifndef GZO_CFG_H
#define GZO_CFG_H

//$Id: acconfig.h,v 1.4 2002/04/11 18:48:00 markus Rel $

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

// Detect common platforms
#define WINDOWS 0
#define UNIX    1
#if defined(_WIN32)
#  if defined(__CYGWIN__)
#     define SYSTEM UNIX
#  else
#     define SYSTEM WINDOWS
#  endif
#else
#  define SYSTEM UNIX
#endif /* _WIN32 */

/* Define the name of the package */
#ifndef PACKAGE
#undef PACKAGE
#endif

/* Define the version number */
#ifndef VERSION
#undef VERSION
#endif

/* Define the micro-version */
#ifndef MICRO_VERSION
#undef MICRO_VERSION
#endif

@TOP@

/* acconfig.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */

/* Define for working regex.h */
#undef HAVE_REGEX_H


/* Define for working regexp.h */
#undef HAVE_REGEXP_H


/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */


@BOTTOM@

#endif
