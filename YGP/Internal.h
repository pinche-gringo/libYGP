#ifndef INTERNAL_H
#define INTERNAL_H

//$Id: Internal.h,v 1.1 2002/04/09 07:16:28 markus Exp $

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

#ifdef HAVE_GETTEXT
#  include <libintl.h>
#  define _(String)                           dgettext (PACKAGE, String)
#  define N_(String)                          gettext_noop (String)
#else
#  define _(String)                           (String)
#  define N_(String)                          gettext_noop (String)
#  define gettext_noop(String)                (String)
#  define binddomain(Domain)
#  define bindtextdomain(Package, Directory)
#endif

#endif