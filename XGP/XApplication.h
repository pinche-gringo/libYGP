#ifndef XGP_XAPPLICATION_H
#define XGP_XAPPLICATION_H

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

#include <boost/scoped_ptr.hpp>

#include <gtkmm/window.h>

#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>

// Forward declarations
namespace Gtk {
class Box;
class Label;
class Image;
} // namespace Gtk

namespace XGP {

/**Baseclass for X-applications; it creates an application-window with a
   client.

   Furthermore there exists the posibility to add a help-menu consisting of:

      - An about box (to be implemented by the showAboutbox() method).
      - A content menu (accessible by pressing F1), which executes a browser
        (default: galeon) with a file determinted by the
        getHelpfile() method). The help must therefore be in HTML-format.
      - A menu to configure the browser to use.

   To get the last two entries, the getHelpfile() method must not return \c
   NULL.

   The following algorithm is used to determine the help file to display:

      -# If the file does not start with a single slash (<tt>/</tt>) and does
         not specify the file-protocoll (starting with <tt>%file://</tt>), just
         pass the filename to the browser.
      -# Else assume a local file to display and check the language
         settings: If the environment variable \c LANGUAGE is not empty,
         take that value, else the locale settings (\c LC_MESSAGES).
      -# Take every (colon-separated) entry from the value of step 2
         and append it to the name of the help-file.
      -# If step 3 does not specify a valid file name, cut the
         language specifier at the last underscore (_). (E.g. change
         \c de_AT to \c de).
      -# Repeat step 4 until either a file is found or the languge
         specifier is empty. If this still does not succeed, procceed
         with step 3.
      -# Nothing worked; search for the file "as is".
      -# As last ressort append a ".en" to the filename.
*/
class XApplication : public Gtk::Window {
  public:
    // Manager functions
    XApplication(const char* pTitle);
    ~XApplication() override; // No need to be virtual. There's only 1 instance

    static void initI18n();
    static void initI18n(const char* package, const char* dir);

  protected:
    using PVBox = boost::scoped_ptr<Gtk::Box>; ///< Smart pointer for a (vertical) box

    virtual void setIconProgram(const guint8* iconData, int lenData);

    /// Returns the client window
    Gtk::Box* getClient() const { return vboxClient.get(); }

    // Protected data
    PVBox vboxClient; ///< Client window of the application

    Glib::RefPtr<Gio::SimpleActionGroup> grpAction; ///< Actions of the application

    /// Appends a "Help" submenu (and its actions) to the passed menu
    void addHelpMenu(const Glib::RefPtr<Gio::Menu>& menu, bool withDynTrace = false);

    /// \name Help menu handling
    //@{
    virtual const char* getHelpfile();
    virtual void showAboutbox();
    void showHelp();
    void selectHelpBrowser();
    void showTraceObjects();
    //@}

    Glib::ustring helpBrowser; ///< Name of the browser to show the help.

  private:
    // Protected manager functions
    XApplication(const XApplication&) = delete;
    const XApplication& operator=(const XApplication&) = delete;
};

/**Baseclass for X-applications showing the program information inside the
   client.

   The top of the client contains 2 icons for program and programmer and two
   lines for program information and about the author.
*/
class XInfoApplication : public XApplication {
  public:
    XInfoApplication(const char* pTitle, const Glib::ustring& prgInfo, const Glib::ustring& copyright);
    ~XInfoApplication() override; // No need to be virtual. There�s only 1 instance

  protected:
    // Add information
    void setIconProgram(const guint8* iconData, int lenData) override;
    void setIconAuthor(const guint8* iconData, int lenData);

  private:
    // Protected manager functions
    XInfoApplication(const XInfoApplication&) = delete;
    const XInfoApplication& operator=(const XInfoApplication&) = delete;

    using PHBox = boost::scoped_ptr<Gtk::Box>;
    using PLabel = boost::scoped_ptr<Gtk::Label>;
    using PImage = boost::scoped_ptr<Gtk::Image>;

    PHBox hboxTitle;
    PVBox vboxPrgInfo;
    PLabel txtProgramm;
    PLabel txtCopyright;

    PImage iconPrg;
    PImage iconAuthor;
};

} // namespace XGP

#endif
