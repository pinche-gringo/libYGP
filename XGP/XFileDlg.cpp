// PROJECT     : libXGP
// SUBSYSTEM   : XFileDlg
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 14.11.1999
// COPYRIGHT   : Copyright (C) 1999 - 2004, 2006, 2008, 2026

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

#include <sys/stat.h>

#include <string>

#include <glibmm/main.h>

#include <giomm/file.h>
#include <giomm/listmodel.h>

#include <gtkmm/messagedialog.h>

#include "YGP/Check.h"
#include "YGP/Trace.h"

#include "XGP/XDialog.h"

#define CONVERT_TO_UTF8
#include "YGP/Internal.h"

#include "XGP/XFileDlg.h"

namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; Creates a (modeless) dialog to select a file
/// \param title Text to display in the title of the dialog
/// \param action Action which is caused by clicking a file
/// \param dlgOption Checks to perform after selecting OK
//-----------------------------------------------------------------------------
FileDialog::FileDialog(const Glib::ustring& title, Gtk::FileChooser::Action action, unsigned int dlgOption)
    : Gtk::FileChooserDialog(title, action), sigSelected(), opt(dlgOption) {
    TRACE9("FileDialog::FileDialog(const Glib::ustring&, Gtk::FileChooser::Action, unsigned int)");

    add_button("_Cancel", static_cast<int>(Gtk::ResponseType::CANCEL));
    add_button((action == Gtk::FileChooser::Action::SAVE) ? "_Save" : "_Open", static_cast<int>(Gtk::ResponseType::OK));

    set_select_multiple(dlgOption & MULTIPLE);
    opt = opt & ~MULTIPLE;
    show();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
FileDialog::~FileDialog() {
    TRACE9("FileDialog::~FileDialog");
    hide();
}

//-----------------------------------------------------------------------------
/// Callback on user-action (file selected or dialog closed)
/// \param cmd ID of selected action
/// \remarks - Depending on the option, the file must either exist or it is
///     checked if it should be overwritten
//-----------------------------------------------------------------------------
void FileDialog::on_response(int cmd) {
    TRACE9("FileDialog::on_response(int) - " << cmd);

    auto handleFile = [this](std::string filename) {
        TRACE8("FileDialog::on_response(int) - File selected: " << filename);

        if (opt != NONE) {
            struct stat fileInfo;
            int rc(stat(filename.c_str(), &fileInfo)); // Get fileinfo

            if (opt & MUST_EXIST)
                if (rc) { // File does not exist: Show msg and exit
                    Glib::ustring err(_("File `%1' does not exist!"));
                    err.replace(err.find("%1"), 2, Glib::filename_to_utf8(filename));
                    Gtk::MessageDialog dlg(err, false, Gtk::MessageType::ERROR);
                    dlg.set_transient_for(*this);
                    XGP::runModal(dlg);
                    return;
                }

            if (opt & ASK_OVERWRITE)
                if (!rc) {
                    Glib::ustring msg(_("File `%1' exists! Overwrite?"));
                    msg.replace(msg.find("%1"), 2, Glib::filename_to_utf8(filename));
                    Gtk::MessageDialog dlg(msg, false, Gtk::MessageType::QUESTION, Gtk::ButtonsType::YES_NO);
                    dlg.set_transient_for(*this);
                    if (XGP::runModal(dlg) != static_cast<int>(Gtk::ResponseType::YES))
                        return;
                }
        } // endif option set

        sigSelected.emit(filename);
    };

    switch (cmd) {
    case Gtk::ResponseType::OK: {

        if (get_select_multiple()) {
            Glib::RefPtr<Gio::ListModel> files(get_files());
            guint count(files->get_n_items());
            for (guint i(0); i < count; ++i) {
                auto file(std::dynamic_pointer_cast<Gio::File>(files->get_object(i)));
                if (file)
                    handleFile(file->get_path());
            }
        }
        else {
            Glib::RefPtr<Gio::File> file(get_file());
            if (file)
                handleFile(file->get_path());
        }
    } // Missing break is intentional

    case Gtk::ResponseType::CANCEL:
        if (modal) {
            modal = false;
            pLoop->quit();
        }
        break;
    } // end-switch command-id

    TRACE9("FileDialog::on_response(int) - Default handler");
    Gtk::FileChooserDialog::on_response(cmd);
}

//-----------------------------------------------------------------------------
/// Performs the dialog modaly
/// \returns std::string The selected file
/// \remarks When executed modally, only the first file is returned
//-----------------------------------------------------------------------------
std::string FileDialog::execModal() {
    Check2(!(opt % MULTIPLE));
    set_modal(modal = true);
    pLoop = Glib::MainLoop::create();
    show();
    pLoop->run();

    Glib::RefPtr<Gio::File> file(modal ? get_file() : Glib::RefPtr<Gio::File>());
    std::string result(file ? file->get_path() : "");
    delete this;
    return result;
}

//----------------------------------------------------------------------------
/// Creates a (modeless) dialog and registers a handler to free it after
/// deleting.
/// \param title Title of the dialog
/// \param action Action which is caused by clicking a file
/// \param dlgOption Options for the dialog
//  \returns FileDialog* Pointer to created dialog
//----------------------------------------------------------------------------
FileDialog* FileDialog::create(const Glib::ustring& title, Gtk::FileChooser::Action action, unsigned int dlgOption) {
    auto* dlg(new FileDialog(title, action, dlgOption));
    dlg->signal_response().connect(sigc::mem_fun(*dlg, &FileDialog::free));
    return dlg;
}

//-----------------------------------------------------------------------------
/// Frees the dialog.
/// \remarks Call only if the dialog was created with new
//-----------------------------------------------------------------------------
void FileDialog::free(int) { delete this; }

} // namespace XGP
