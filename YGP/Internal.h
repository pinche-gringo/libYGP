#ifndef INTERNAL_H
#define INTERNAL_H

//$Id: Internal.h,v 1.6 2003/11/16 19:25:55 markus Rel $

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


#include <ygp-cfg.h>

#undef _
#undef N_
#undef textdomain
#undef binddomain
#undef bindtextdomain

#if defined (HAVE_GETTEXT) && defined (ENABLE_NLS)
#  include <libintl.h>
#  define _(String)                           dgettext (PACKAGE, String)
#  define N_(String)                          (String)
#else
#  define _(String)                           (String)
#  define N_(String)                          (String)
#  define textdomain(domain)
#  define binddomain(Domain)
#  define bindtextdomain(Package, Directory)
#endif

#endif
