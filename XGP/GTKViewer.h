#ifndef GTKVIEWER_H
#define GTKVIEWER_H

//$Id: GTKViewer.h,v 1.3 2005/03/08 04:48:57 markus Exp $

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


#ifdef HAVE_GTKHTML

#include <gtk/gtkwidget.h>

#ifdef  __cplusplus
extern "C" {
#endif

    GtkWidget* gtkhtmlInitialize ();
    int        gtkhtmlDisplayFile (GtkWidget* ctrl, const char* file);

    const char* gtkhtmlGetError ();

#ifdef  __cplusplus
}
#endif


#endif // HAVE_GTKHTML

#endif
