#ifndef XGP_XAPPLICATION_H
#define XGP_XAPPLICATION_H

//$Id: XApplication.h,v 1.32 2008/05/18 13:21:27 markus Rel $

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

#include <gtkmm/main.h>
#include <gtkmm/menu.h>
#include <gtkmm/window.h>
#include <gtkmm/uimanager.h>


// Forward declarations
namespace Gtk {
   class Main;
   class HBox;
   class VBox;
   class Label;
   class Image;
   class AccelGroup;
}

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
   XApplication (const char* pTitle);
   ~XApplication ();         // No need to be virtual. There's only 1 instance

   static void initI18n ();
   static void initI18n (const char* package, const char* dir);

 protected:
   typedef boost::scoped_ptr<Gtk::VBox>    PVBox;     ///< Smart pointer for a vbox

   virtual void setIconProgram (const guint8* iconData, int lenData);

   /// Returns the client window
   Gtk::VBox* getClient () const { return vboxClient.get (); }

   // Protected data
   PVBox    vboxClient;                   ///< Client window of the application

   Glib::RefPtr<Gtk::ActionGroup> grpAction;    ///< Actions of the application
   Glib::RefPtr<Gtk::UIManager>   mgrUI;                        ///< UI-manager

   // Help-menu
   void addHelpMenu (Glib::ustring& uiString, bool withDynTrace = false);

   /// \name Help menu handling
   //@{
   virtual const char* getHelpfile ();
   virtual void showAboutbox ();
   void showHelp ();
   void selectHelpBrowser ();
   void showTraceObjects ();
   //@}

   Glib::ustring helpBrowser;       ///< Name of the browser to show the help.

 private:
   // Protected manager functions
   XApplication (const XApplication&);
   const XApplication& operator= (const XApplication&);
};


/**Baseclass for X-applications showing the program information inside the
   client.

   The top of the client contains 2 icons for program and programmer and two
   lines for program information and about the author.
*/
class XInfoApplication : public XApplication {
 public:
   XInfoApplication (const char* pTitle, const Glib::ustring& prgInfo, const Glib::ustring& copyright);
   ~XInfoApplication ();     // No need to be virtual. There�s only 1 instance

 protected:
   // Add information
   virtual void setIconProgram (const guint8* iconData, int lenData);
   void setIconAuthor (const guint8* iconData, int lenData);

 private:
   // Protected manager functions
   XInfoApplication (const XInfoApplication&);
   const XInfoApplication& operator= (const XInfoApplication&);

   typedef boost::scoped_ptr<Gtk::HBox>  PHBox;
   typedef boost::scoped_ptr<Gtk::Label> PLabel;
   typedef boost::scoped_ptr<Gtk::Image> PImage;

   PHBox  hboxTitle;
   PVBox  vboxPrgInfo;
   PLabel txtProgramm;
   PLabel txtCopyright;

   PImage iconPrg;
   PImage iconAuthor;
};

}

#endif
