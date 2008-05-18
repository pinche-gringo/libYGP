#ifndef XGP_GTKMOZVIEWER_H
#define XGP_GTKMOZVIEWER_H

//$Id: GtkMozViewer.h,v 1.3 2008/05/18 13:21:27 markus Rel $

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

#ifdef HAVE_GTKMOZEMBED

#include <gtk/gtkwidget.h>

#ifdef  __cplusplus
extern "C" {
#endif

    GtkWidget* gtkMozEmbedInitialize ();
    void       gtkMozEmbedDisplayURL (GtkWidget* ctrl, const char* url);

    const char* gtkMozEmbedGetError ();

#ifdef  __cplusplus
}
#endif


#endif // HAVE_GTMOZEMBED

#endif
