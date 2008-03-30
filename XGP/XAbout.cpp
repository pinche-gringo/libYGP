//$Id: XAbout.cpp,v 1.30 2008/03/30 13:39:17 markus Rel $

//PROJECT     : MessageDialog
//SUBSYSTEM   : XAbout
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.30 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.9.1999
//COPYRIGHT   : Copyright (C) 1999 - 2005, 2008

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


#include <gdkmm/pixbuf.h>

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include "XGP/XAbout.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; adds all controls to the dialog and shows it.
/// \param author: Author, copyright, ... of the program displayed in the client
/// \param program: Name and version of the program; displaced in the title
//-----------------------------------------------------------------------------
XAbout::XAbout (const Glib::ustring& author, const Glib::ustring& program)
   : XDialog (CANCEL), writer (new Gtk::Label (author)),
     gpl (new Gtk::Label (_("Distributed under the terms of the GNU General Public License"))),
     pIconAuthor (), pIconProgramm (),
     client (new Gtk::HBox) {
   Check1 (gpl); Check1 (writer); Check1 (client);

   TRACE9 ("XAbout::XAbout - Show: " << author);
   TRACE9 ("XAbout::XAbout - Title: " << program);

   set_title (program);

   writer->set_justify (Gtk::JUSTIFY_CENTER);
   gpl->set_justify (Gtk::JUSTIFY_CENTER);

   writer->show ();
   client->pack_end (*writer, true, false, 5);         // Put text into client
   client->show ();
   get_vbox ()->pack_start (*client, true, false, 5);

   gpl->show ();
   get_vbox ()->pack_start (*gpl, true, true, 5);

   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XAbout::~XAbout () {
   TRACE9 ("XAbout::~XAbout");
   hide ();
}

//-----------------------------------------------------------------------------
/// Sets the icon for the program (in xpm-format)
/// \param pIconData: Image representing program
//-----------------------------------------------------------------------------
void XAbout::setIconProgram (const char* const* pIconData) {
   Check1 (client); Check1 (pIconData);

   pIconProgramm = new Gtk::Image
      (Gdk::Pixbuf::create_from_xpm_data (pIconData));

   pIconProgramm->show ();
   client->pack_start (*pIconProgramm, false, false, 5);
}

//-----------------------------------------------------------------------------
/// Sets the icon for the programmer (in xpm-format)
/// \param pIconData: Pointer to xpm-data for pixmap representing the programmer
//-----------------------------------------------------------------------------
void XAbout::setIconAuthor (const char* const* pIconData) {
   Check1 (client); Check1 (pIconData);

   pIconAuthor = new Gtk::Image
      (Gdk::Pixbuf::create_from_xpm_data (pIconData));

   pIconAuthor->show ();
   client->pack_end (*pIconAuthor, false, false, 5);

   Check3 (writer);
   client->reorder_child (*writer, 3);
}

//----------------------------------------------------------------------------
/// Creates a (modeless) about dialog and registers a handler to free it
/// after deleting
/// \returns IPrintDialog*: Pointer to created dialog
//----------------------------------------------------------------------------
XAbout* XAbout::create (const Glib::ustring& author, const Glib::ustring& program) {
    XAbout* dlg (new XAbout (author, program));
    dlg->signal_response ().connect (mem_fun (*dlg, &XAbout::free));
    return dlg;
}

}
