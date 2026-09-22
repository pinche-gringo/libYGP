// PROJECT     : MessageDialog
// SUBSYSTEM   : XAbout
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 14.9.1999
// COPYRIGHT   : Copyright (C) 1999 - 2005, 2008, 2009, 2024, 2026

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

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdkmm/pixbuf.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include "XGP/XAbout.h"

namespace XGP {

const int ICON_SIZE(48);

//-----------------------------------------------------------------------------
/// Scales the passed pixbuf to the standard icon-size (if not already that size)
/// \param pixbuf Pixbuf to scale
/// \returns Glib::RefPtr<Gdk::Pixbuf> Scaled pixbuf
//-----------------------------------------------------------------------------
static Glib::RefPtr<Gdk::Pixbuf> scaleIcon(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf) {
    if ((pixbuf->get_width() == ICON_SIZE) && (pixbuf->get_height() == ICON_SIZE))
        return pixbuf;
    return pixbuf->scale_simple(ICON_SIZE, ICON_SIZE, Gdk::InterpType::BILINEAR);
}

//-----------------------------------------------------------------------------
/// Constructor; adds all controls to the dialog and shows it.
/// \param author Author, copyright, ... of the program displayed in the client
/// \param program Name and version of the program; displaced in the title
//-----------------------------------------------------------------------------
XAbout::XAbout(const Glib::ustring& author, const Glib::ustring& program)
    : XDialog(CANCEL), writer(new Gtk::Label(author)),
      gpl(new Gtk::Label(_("Distributed under the terms of the GNU General Public License"))), pIconAuthor(), pIconProgramm(),
      client(new Gtk::Box) {
    Check1(gpl);
    Check1(writer);
    Check1(client);

    TRACE9("XAbout::XAbout - Show: " << author);
    TRACE9("XAbout::XAbout - Title: " << program);

    set_title(program);

    writer->set_justify(Gtk::Justification::CENTER);
    gpl->set_justify(Gtk::Justification::CENTER);

    writer->set_hexpand();
    writer->set_margin(5);
    client->append(*writer); // Put text into client
    get_content_area()->append(*client);

    gpl->set_margin(5);
    get_content_area()->append(*gpl);

    show();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XAbout::~XAbout() {
    TRACE9("XAbout::~XAbout");
    hide();
}

//-----------------------------------------------------------------------------
/// Sets the icon for the program (in inline format)
/// \param pIconData Image representing program
/// \param lenData: Length of image
//-----------------------------------------------------------------------------
void XAbout::setIconProgram(const guint8* pIconData, int lenData) {
    Check1(client);
    Check1(pIconData);

    pIconProgramm.reset(new Gtk::Image(scaleIcon(Glib::wrap(gdk_pixbuf_new_from_inline(lenData, pIconData, false, nullptr)))));

    pIconProgramm->set_pixel_size(ICON_SIZE);
    pIconProgramm->set_margin(5);
    client->prepend(*pIconProgramm);
}

//-----------------------------------------------------------------------------
/// Sets the icon for the programmer (in xpm-format)
/// \param pIconData Pointer to xpm-data for pixmap representing the programmer
/// \param lenData: Length of image
//-----------------------------------------------------------------------------
void XAbout::setIconAuthor(const guint8* pIconData, int lenData) {
    Check1(client);
    Check1(pIconData);

    pIconAuthor.reset(new Gtk::Image(scaleIcon(Glib::wrap(gdk_pixbuf_new_from_inline(lenData, pIconData, false, nullptr)))));

    pIconAuthor->set_pixel_size(ICON_SIZE);
    pIconAuthor->set_margin(5);
    client->append(*pIconAuthor);
}

//----------------------------------------------------------------------------
/// Creates a (modeless) about dialog and registers a handler to free it
/// after deleting
/// \returns IPrintDialog* Pointer to created dialog
//----------------------------------------------------------------------------
XAbout* XAbout::create(const Glib::ustring& author, const Glib::ustring& program) {
    auto* dlg(new XAbout(author, program));
    dlg->signal_response().connect(sigc::mem_fun(*dlg, &XAbout::free));
    return dlg;
}

} // namespace XGP
