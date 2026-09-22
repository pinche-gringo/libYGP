// PROJECT     : libXGP
// SUBSYSTEM   : XDialog
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 04.01.2003
// COPYRIGHT   : Copyright (C) 2003, 2004, 2006, 2008, 2011, 2026

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

#include <type_traits>

#include <glibmm/main.h>

#include <gtkmm/button.h>
// #include <gtkmm/accelgroup.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "XGP/XDialog.h"

namespace XGP {

//-----------------------------------------------------------------------------
/// Runs the passed dialog modally and returns the response-ID it was closed with
/// \param dlg Dialog to run
/// \returns int Response-ID the dialog was closed with
/// \remarks Replaces the blocking Gtk::Dialog::run(), which GTK4 removed
//-----------------------------------------------------------------------------
int runModal(Gtk::Dialog& dlg) {
    Glib::RefPtr<Glib::MainLoop> loop(Glib::MainLoop::create());
    int response(static_cast<int>(Gtk::ResponseType::NONE));

    sigc::connection conn(dlg.signal_response().connect([&](int r) {
        response = r;
        loop->quit();
    }));

    dlg.set_modal(true);
    dlg.show();
    loop->run();
    conn.disconnect();

    return response;
}

//-----------------------------------------------------------------------------
/// Constructor; creates the dialog with the specified buttons
/// \param buttons Bitfield for buttons to display
/// \param modal Flag, if the dialog is modal
//-----------------------------------------------------------------------------
XDialog::XDialog(unsigned int buttons, bool modal) : Gtk::Dialog(Glib::ustring(), modal), ok(nullptr), cancel(nullptr) {
    init(buttons);
}

//-----------------------------------------------------------------------------
/// Constructor; creates the dialog with the specified buttons
/// \param buttons Bitfield for buttons to display
/// \param title Title to display
/// \param modal Flag, if the dialog is modal
//-----------------------------------------------------------------------------
XDialog::XDialog(const Glib::ustring& title, unsigned int buttons, bool modal)
    : Gtk::Dialog(title, modal), ok(nullptr), cancel(nullptr) {
    init(buttons);
}

//-----------------------------------------------------------------------------
/// (Default-)Constructor; creates the dialog with the specified buttons
/// \param buttons Bitfield for buttons to display
/// \param title Title to display
/// \param parent Parent of the dialog
/// \param modal Flag, if the dialog is modal
//-----------------------------------------------------------------------------
XDialog::XDialog(const Glib::ustring& title, Gtk::Window& parent, unsigned int buttons, bool modal)
    : Gtk::Dialog(title, parent, modal), ok(nullptr), cancel(nullptr) {
    init(buttons);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XDialog::~XDialog() { TRACE9("XDialog::~XDialog()"); }

//-----------------------------------------------------------------------------
/// Adds the specified buttons to the dialog
/// \param buttons Bitfield for buttons to display
//-----------------------------------------------------------------------------
void XDialog::init(unsigned int buttons) {
    TRACE9("XDialog::init()");
    ok = (buttons & OK) ? add_button("_OK", static_cast<int>(Gtk::ResponseType::OK)) : nullptr;
    cancel = (buttons & CANCEL) ? add_button((buttons & OK) ? "_Cancel" : "_Close", static_cast<int>(Gtk::ResponseType::CANCEL))
                                : nullptr;

    if (ok)
        set_default_widget(*ok);
    else if (cancel)
        set_default_widget(*cancel);
}

//-----------------------------------------------------------------------------
/// Callback after button-events
/// \param cmd ID of pressed button
//-----------------------------------------------------------------------------
void XDialog::on_response(int cmd) {
    TRACE9("XDialog::on_response(int) " << cmd);
    if (cmd == static_cast<int>(Gtk::ResponseType::OK)) {
        if (!isDataOK())
            return;
        okEvent();
    }

    if (cmd == static_cast<int>(Gtk::ResponseType::CANCEL))
        cancelEvent();
    else
        command(cmd);
}

//-----------------------------------------------------------------------------
/// Callback after pressing the OK button; the user can override that to implemnt
/// own behaviour
//-----------------------------------------------------------------------------
void XDialog::okEvent() { TRACE9("XDialog::okEvent()"); }

//-----------------------------------------------------------------------------
/// Callback after pressing the Cancel button
//-----------------------------------------------------------------------------
void XDialog::cancelEvent() { TRACE9("XDialog::cancelEvent()"); }

//-----------------------------------------------------------------------------
/// Checks if the data entered in the dialog is OK
/// \returns bool True, if dialog can be left by selecting OK
//-----------------------------------------------------------------------------
bool XDialog::isDataOK() {
    TRACE9("XDialog::isDataOK()");
    return true;
}

//-----------------------------------------------------------------------------
/// Command handler of the dialog
/// \param action Selected action
/// \remarks Must not be called
//-----------------------------------------------------------------------------
void XDialog::command(int action) { TRACE9("XDialog::command(int) - " << action); }

//-----------------------------------------------------------------------------
/// Frees the dialog.
/// \remarks Call only if the dialog was created with new
//-----------------------------------------------------------------------------
void XDialog::free(int) {
    TRACE9("XDialog::free(int)");
    delete this;
}

} // namespace XGP
