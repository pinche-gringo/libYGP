#ifndef HTMLVIEWER_H
#define HTMLVIEWER_H

//$Id: HTMLViewer.h,v 1.2 2003/10/19 00:02:46 markus Exp $

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
#define HAVE_VIEWER

namespace Gtk {
    class ScrolledWindow;
}

#include <XDialog.h>


/**Helper class to display a HTML document in a dialog.
 *
 * Uses GTKHTML to display the HTML code.
 */
class HTMLViewer : public XDialog {
 public:
    static HTMLViewer* create (const std::string& file) throw (std::string);

 private:
    HTMLViewer (const std::string& file) throw (std::string);
    ~HTMLViewer ();

    void display (const std::string& file) throw (std::string);
    void x (int);

    void*                htmlCtrl;
    Gtk::ScrolledWindow* scrl;
};

#endif // HAVE_GTKHTML

#endif