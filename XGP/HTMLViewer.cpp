//$Id: HTMLViewer.cpp,v 1.1 2003/10/17 06:33:20 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : HTMLViewer
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 16.10.2003
//COPYRIGHT   : Anticopyright (A) 2003

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


#include <Internal.h>

#ifdef HAVE_GTKHTML

#define CHECK 9
#define TRACELEVEL 9
#include <Check.h>
#include <Trace_.h>

#include "GTKViewer.h"
#include "HTMLViewer.h"


HTMLViewer* HTMLViewer::instance (NULL);


//----------------------------------------------------------------------------
/// Creates (or updates) a dialog displaying a HTML-document
/// \param file: File containing the HTML-document to display
/// \throw std::string in case of error
//----------------------------------------------------------------------------
void HTMLViewer::create (const std::string& file) throw (std::string) {
   TRACE9 ("HTMLViewer::create (const std::string&) - " << file);
   Check1 (file.size ());

   if (instance)
      instance->display (file);
   else
      instance = new HTMLViewer (file);
}


//----------------------------------------------------------------------------
/// Creates a dialog displaying a HTML-document
/// \param file: File containing the HTML-document to display
/// \throw \c std::string in case of error
//----------------------------------------------------------------------------
HTMLViewer::HTMLViewer (const std::string& file) throw (std::string)
    : XDialog (_("Help window"), XDialog::OK), htmlCtrl (NULL) {
   TRACE9 ("HTMLViewer::HTMLViewer (const std::string&) - " << file);
   Check1 (file.size ());

   htmlCtrl = gtkhtmlGetWidget ();
   if (htmlCtrl) {
      gtk_widget_show (htmlCtrl);
      get_vbox ()->pack_start (*Glib::wrap (htmlCtrl, false));

      display (file.c_str ());
      show ();
   }
   else {
      std::string err (_("Can't display the HTML control!\n\nReason: %1"));
      err.replace (err.find ("%1"), 2, gtkhtmlGetError ());
      throw (err);
   }
}

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
HTMLViewer::~HTMLViewer () {
   gtkhtmlDeleteWidget (htmlCtrl);
}


//----------------------------------------------------------------------------
/// Sets the HTML-document to display in the dialog
/// \param file: File containing the HTML-document to display
/// \throw std::string in case of error
//----------------------------------------------------------------------------
void HTMLViewer::display (const std::string& file) throw (std::string) {
   TRACE9 ("HTMLViewer::display (const std::string&) - " << file);
   Check1 (file.size ());

   std::string err;
   int rc (gtkhtmlDisplayFile (htmlCtrl, file.c_str ()));
   switch (rc) {
   case 1:
      err = _("File `%1' not found!");
      break;

   case -1:
      err = _("Can't display the HTML control!\n\nReason: %1");
      err.replace (err.find ("%1"), 2, gtkhtmlGetError ());
      break;

   case -2:
      err = _("Can't display the HTML document!\n\nReason: %1");
      err.replace (err.find ("%1"), 2, gtkhtmlGetError ());
      break;
   } // end-switch

   if (err.size ()) {
      throw err;
   }
}

#endif
