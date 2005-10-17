#ifndef GTKMOZVIEWER_H
#define GTKMOZVIEWER_H

//$Id: GtkMozViewer.h,v 1.1 2005/10/17 03:50:40 markus Rel $

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
