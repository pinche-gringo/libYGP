// PROJECT     : libXGP
// SUBSYSTEM   : PrintDialog
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 14.11.1999
// COPYRIGHT   : Copyright (C) 1999 - 2004, 2006, 2008, 2026

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

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <memory>
#include <string>

#include <glibmm/convert.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>

#include <gtkmm/messagedialog.h>
#include <gtkmm/printcontext.h>
#include <gtkmm/printsettings.h>
#include <gtkmm/window.h>

#include <pangomm/layout.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include "XGP/XDialog.h"
#include "XGP/XPrintDlg.h"

namespace XGP {

namespace {

/// Font to print the text with
constexpr const char* FONT("Monospace 10");

/// Closes a file
struct FileCloser {
    void operator()(FILE* file) const { fclose(file); }
};

/// Settings of the last print operation (to be used for the next one)
Glib::RefPtr<Gtk::PrintSettings> lastSettings;

/// Displays an error message
/// \param parent Window the message belongs to (or NULL)
/// \param message Message to display
void showError(Gtk::Window* parent, const Glib::ustring& message) {
    Gtk::MessageDialog msg(message, false, Gtk::MessageType::ERROR);
    if (parent)
        msg.set_transient_for(*parent);
    runModal(msg);
}

} // namespace

//-----------------------------------------------------------------------------
/// Constructor
/// \param parent Window the print dialog belongs to (or NULL)
//-----------------------------------------------------------------------------
PrintDialog::PrintDialog(Gtk::Window* parent)
    : sigPrint(), parent(parent), operation(Gtk::PrintOperation::create()), layout(), baselines(), pages() {
    TRACE9("PrintDialog::PrintDialog(Gtk::Window*)");

    if (lastSettings)
        operation->set_print_settings(lastSettings);
    operation->set_allow_async();
    operation->set_embed_page_setup();
    operation->signal_begin_print().connect(sigc::mem_fun(*this, &PrintDialog::beginPrint));
    operation->signal_draw_page().connect(sigc::mem_fun(*this, &PrintDialog::drawPage));
    operation->signal_done().connect(sigc::mem_fun(*this, &PrintDialog::done));
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
PrintDialog::~PrintDialog() { TRACE9("PrintDialog::~PrintDialog()"); }

//-----------------------------------------------------------------------------
/// Creates a print dialog; it is shown as soon as the main loop is idle (so
/// sigPrint can be connected first) and deletes itself afterwards.
/// \param parent Window the print dialog belongs to (or NULL)
/// \returns PrintDialog* Pointer to created dialog
//-----------------------------------------------------------------------------
PrintDialog* PrintDialog::create(Gtk::Window* parent) {
    auto* dlg(new PrintDialog(parent));
    Glib::signal_idle().connect_once(sigc::mem_fun(*dlg, &PrintDialog::run));
    return dlg;
}

//-----------------------------------------------------------------------------
/// Shows the print dialog
//-----------------------------------------------------------------------------
void PrintDialog::run() {
    TRACE9("PrintDialog::run()");
    try {
        if (parent)
            operation->run(Gtk::PrintOperation::Action::PRINT_DIALOG, *parent);
        else
            operation->run(Gtk::PrintOperation::Action::PRINT_DIALOG);
    }
    catch (Glib::Error& error) {
        showError(parent, error.what());
        delete this;
    }
}

//-----------------------------------------------------------------------------
/// Collects the text to print by emitting sigPrint
/// \returns std::string Text to print (in UTF-8)
/// \throws std::runtime_error If the temporary file can't be created
//-----------------------------------------------------------------------------
std::string PrintDialog::collectText() {
    std::unique_ptr<FILE, FileCloser> file(tmpfile());
    if (!file) {
        std::string err(_("Can't create temporary file!\n\nReason: %1"));
        err.replace(err.find("%1"), 2, strerror(errno));
        throw std::runtime_error(err);
    }

    sigPrint.emit(file.get());
    fflush(file.get());
    rewind(file.get());

    std::string text;
    char buffer[4096];
    std::size_t read;
    while ((read = fread(buffer, 1, sizeof(buffer), file.get())) > 0)
        text.append(buffer, read);

    // The text might be in the encoding of the locale
    if (!g_utf8_validate(text.data(), static_cast<gssize>(text.size()), nullptr)) {
        try {
            text = Glib::locale_to_utf8(text);
        }
        catch (Glib::ConvertError&) {
            text = Glib::ustring(text).make_valid().raw();
        }
    }
    return text;
}

//-----------------------------------------------------------------------------
/// Prepares printing; collects the text and splits it into pages
/// \param context Context to print into
//-----------------------------------------------------------------------------
void PrintDialog::beginPrint(const Glib::RefPtr<Gtk::PrintContext>& context) {
    TRACE9("PrintDialog::beginPrint(const Glib::RefPtr<Gtk::PrintContext>&)");

    std::string text;
    try {
        text = collectText();
    }
    catch (std::exception& error) {
        showError(parent, error.what());
        operation->cancel();
        return;
    }

    layout = context->create_pango_layout();
    layout->set_font_description(Pango::FontDescription(FONT));
    layout->set_width(static_cast<int>(context->get_width() * PANGO_SCALE));
    layout->set_wrap(Pango::WrapMode::WORD_CHAR);
    layout->set_text(text);

    // Split the lines into pages
    const int pageHeight(static_cast<int>(context->get_height() * PANGO_SCALE));
    baselines.clear();
    pages.assign(1, 0);
    int pageTop(0);
    auto iter(layout->get_iter());
    do {
        Pango::Rectangle ink, logical;
        iter.get_line_extents(ink, logical);
        if (((logical.get_y() + logical.get_height() - pageTop) > pageHeight) &&
            (baselines.size() > static_cast<std::size_t>(pages.back()))) {
            pages.push_back(static_cast<int>(baselines.size()));
            pageTop = logical.get_y();
        }
        baselines.push_back(iter.get_baseline());
    }
    while (iter.next_line());

    TRACE5("PrintDialog::beginPrint(const Glib::RefPtr<Gtk::PrintContext>&) - Lines: " << baselines.size()
                                                                                     << "; Pages: " << pages.size());
    operation->set_n_pages(static_cast<int>(pages.size()));
}

//-----------------------------------------------------------------------------
/// Draws the lines of one page
/// \param context Context to print into
/// \param page Number of page to draw (starting with 0)
//-----------------------------------------------------------------------------
void PrintDialog::drawPage(const Glib::RefPtr<Gtk::PrintContext>& context, int page) {
    TRACE9("PrintDialog::drawPage(const Glib::RefPtr<Gtk::PrintContext>&, int) - " << page);
    Check1(layout);
    Check1((page >= 0) && (static_cast<std::size_t>(page) < pages.size()));

    const int first(pages[page]);
    const int last((static_cast<std::size_t>(page + 1) < pages.size()) ? pages[page + 1]
                                                                       : static_cast<int>(baselines.size()));

    // The top of the page is the top of its first line
    auto iter(layout->get_iter());
    for (int i(0); i < first; ++i)
        iter.next_line();
    Pango::Rectangle ink, logical;
    iter.get_line_extents(ink, logical);
    const int top(logical.get_y());

    auto cairo(context->get_cairo_context());
    cairo->set_source_rgb(0, 0, 0);
    for (int i(first); i < last; ++i) {
        cairo->move_to(0, static_cast<double>(baselines[i] - top) / PANGO_SCALE);
        layout->get_line(i)->show_in_cairo_context(cairo);
    }
}

//-----------------------------------------------------------------------------
/// Finishes printing; remembers the settings and deletes the object
/// \param result Result of the print operation
//-----------------------------------------------------------------------------
void PrintDialog::done(Gtk::PrintOperation::Result result) {
    TRACE9("PrintDialog::done(Gtk::PrintOperation::Result) - " << static_cast<int>(result));
    if (result == Gtk::PrintOperation::Result::APPLY)
        lastSettings = operation->get_print_settings();
    else if (result == Gtk::PrintOperation::Result::ERROR)
        showError(parent, _("Printing failed!"));

    // Don't delete the operation while it is emitting its signal
    Glib::signal_idle().connect_once([this] { delete this; });
}

} // namespace XGP
