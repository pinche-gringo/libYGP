#ifndef YGP_INTERNAL_H
#define YGP_INTERNAL_H

//$Id: Internal.h,v 1.11 2008/05/18 13:21:27 markus Rel $

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


#if defined (HAVE_GETTEXT) && defined (ENABLE_NLS)
#  include <libintl.h>
#  ifdef CONVERT_TO_UTF8
#     define _(String)                        Glib::locale_to_utf8 (dgettext (LIBYGP_NAME, String))
#  else
#     define _(String)                        dgettext (LIBYGP_NAME, String)
#  endif
#  define N_(String)                          (String)
#else
#  define _(String)                           (String)
#  define N_(String)                          (String)
#  define textdomain(domain)
#  define binddomain(Domain)
#  define bindtextdomain(Package, Directory)
#endif

#endif
