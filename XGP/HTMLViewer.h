#ifndef HTMLVIEWER_H
#define HTMLVIEWER_H

//$Id: HTMLViewer.h,v 1.14 2008/03/30 13:39:17 markus Rel $

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


#if defined HAVE_GTKHTML || defined HAVE_GTKMOZEMBED
#define HAVE_VIEWER

#include <string>
#include <stdexcept>


namespace Gtk {
    class ScrolledWindow;
}

#include <XGP/XDialog.h>


namespace XGP {

/**Helper class to display a HTML document in a dialog.
 *
 * Uses GTKHTML or GtkMozEmbed to display the HTML code.
 */
class HTMLViewer : public XDialog {
 public:
   class CreateError : public std::runtime_error {
    public:
      CreateError (const std::string& what) : std::runtime_error (what) { }
   };

   /// Supported widgets to display HTML
   typedef enum { GTKHTML, GTKMOZEMBED, LAST } widgetTypes;

   virtual ~HTMLViewer ();

   static HTMLViewer* create (const std::string& file, const Glib::ustring& title,
			      widgetTypes type) throw (CreateError);
   void display (const std::string& file);

 protected:
   HTMLViewer (const std::string& file, const Glib::ustring& title,
	       widgetTypes type) throw (CreateError);

 private:
   HTMLViewer (const HTMLViewer&);
   const HTMLViewer& operator= (const HTMLViewer&);

   int _display (const std::string file);

   GtkWidget*  htmlCtrl;
   widgetTypes _type;
};

}

#endif // HAVE_GTKHTML || HAVE_GTKMOZEMBED

#endif
