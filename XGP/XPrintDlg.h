#ifndef XGP_XPRINTDLG_H
#define XGP_XPRINTDLG_H

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

#include <cstdio>
#include <string>
#include <vector>

#include <glibmm/refptr.h>
#include <sigc++/signal.h>
#include <sigc++/trackable.h>

#include <gtkmm/printoperation.h>

namespace Gtk {
class PrintContext;
class Window;
} // namespace Gtk

namespace Pango {
class Layout;
}

namespace XGP {

/**Class to print plain text with the print dialog of the system.

   The text to print is collected (after the user confirmed the dialog) by
   emitting sigPrint with a (temporary) file the text should be written to.
   It is printed in a monospaced font; long lines are wrapped.

   The object deletes itself after printing (or canceling).
*/
class PrintDialog : public sigc::trackable {
  public:
    ~PrintDialog();

    static PrintDialog* create(Gtk::Window* parent = nullptr);

    /// Signal emitted, when printing has been confirmed; the text to print
    /// must be written to the passed file
    sigc::signal<void(FILE*)> sigPrint;

  private:
    explicit PrintDialog(Gtk::Window* parent);

    // Prohibited manager-functions
    PrintDialog(const PrintDialog&) = delete;
    const PrintDialog& operator=(const PrintDialog&) = delete;

    void run();
    std::string collectText();
    void beginPrint(const Glib::RefPtr<Gtk::PrintContext>& context);
    void drawPage(const Glib::RefPtr<Gtk::PrintContext>& context, int page);
    void done(Gtk::PrintOperation::Result result);

    Gtk::Window* parent;                       ///< Window the dialog belongs to (or NULL)
    Glib::RefPtr<Gtk::PrintOperation> operation; ///< Running print operation
    Glib::RefPtr<Pango::Layout> layout;         ///< Layout of the whole text
    std::vector<int> baselines;                 ///< Baseline of every line (in Pango units)
    std::vector<int> pages;                     ///< First line of every page
};

} // namespace XGP

#endif
