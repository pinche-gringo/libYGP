//$Id: XApplication.cpp,v 1.42 2005/01/24 17:12:18 markus Exp $

//PROJECT     : libXGP
//SUBSYSTEM   : XApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.42 $
//AUTHOR      : Markus Schwab
//CREATED     : 4.9.1999
//COPYRIGHT   : Copyright (C) 1999 - 2005

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


#include <cstdlib>
#include <csignal>
#include <clocale>

#include <sys/stat.h>

#include <gdkmm/pixbuf.h>

#include <gtkmm/box.h>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/messagedialog.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/Process.h>
#include <YGP/StackTrc.h>

#include <YGP/Tokenize.h>

#include "XGP/HTMLViewer.h"
#include "XGP/BrowserDlg.h"

#include "XGP/XApplication.h"

using namespace Gtk::Menu_Helpers;


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; creates a program-window showing only an empty menu and an
/// area (a GTK::VBox) for the client. Furthermore \c SIGSEGV and \c SIGBUS
/// signals are trapped to produce a stackdump in the log file.
/// \param pTitle: Pointer to title of the application
//-----------------------------------------------------------------------------
XApplication::XApplication (const char* pTitle)
   : vboxClient (new Gtk::VBox ())
     , grpAction (Gtk::ActionGroup::create ())
     , mgrUI (Gtk::UIManager::create ())
     , helpBrowser (BrowserDlg::getDefaultBrowser ()) {
   TRACE9 ("XApplication::XApplication (const char*) - " << pTitle);
   signal (SIGSEGV, handleSignal);
#ifdef HAVE_SIGBUS
   signal (SIGBUS, handleSignal);
#endif

   Check3 (pTitle);
   set_title (pTitle);

   Check3 (vboxClient);
   vboxClient->show ();
   add (*vboxClient);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XApplication::~XApplication () {
   TRACE9 ("XApplication::~XApplication () - start");

   signal (SIGSEGV, SIG_DFL);
#ifdef HAVE_SIGBUS
   signal (SIGBUS, SIG_DFL);
#endif
}



//-----------------------------------------------------------------------------
/// Initializes the program for internationalization by setting the locale and
/// loading the messagefile.
/// \param package: Name of the message-catalog
/// \param dir: Root-directory for message-catalogs
//-----------------------------------------------------------------------------
void XApplication::initI18n (const char* package, const char* dir) {
   Check1 (package); Check1 (dir);

   initI18n ();
   bindtextdomain (package, dir);
   textdomain (package);
}

//-----------------------------------------------------------------------------
/// Initializes the program for localization by setting the locale and loading
/// the messagefile.
//-----------------------------------------------------------------------------
void XApplication::initI18n () {
   setlocale (LC_ALL, "");                         // Activate current locale
}

//-----------------------------------------------------------------------------
/// Adds a help-menu at the end of the menu. This menu consists of an
/// "About"-entry and - if the method getHelpfile() does return a value -
/// entries to display a help-file and to configure the help browser.
/// \param uiString: String, to which the menu-structure is appended
//-----------------------------------------------------------------------------
void XApplication::addHelpMenu (Glib::ustring& uiString) {
   TRACE9 ("XApplication::addHelpMenu ()");

   uiString += "<menu action='Help'>";

   grpAction->add (Gtk::Action::create ("Help", _("_Help")));
   if (getHelpfile ()) {
      grpAction->add (Gtk::Action::create ("HlpContent", Gtk::Stock::HELP),
		      Gtk::AccelKey (_("F1")),
		      mem_fun (*this, &XApplication::showHelp));
      grpAction->add (Gtk::Action::create ("HlpSetBrowser", Gtk::Stock::PROPERTIES,
					   _("Set help-_browser"),
					   _("Enables selecting which browser to use")),
		      mem_fun (*this, &XApplication::selectHelpBrowser));

      uiString += ("<menuitem action='HlpContent'/>"
		   "<menuitem action='HlpSetBrowser'/><separator/>");
   }
   grpAction->add (Gtk::Action::create ("HlpAbout", _("_About")),
		   mem_fun (*this, &XApplication::showAboutbox));

   uiString += "<menuitem action='HlpAbout'/></menu>";
 }

//-----------------------------------------------------------------------------
/// Shows the help to the program
//-----------------------------------------------------------------------------
void XApplication::showHelp () {
   Check3 (getHelpfile ());
   std::string file (getHelpfile ());
   TRACE9 ("XApplication::command (int) - Show help " << file);
   TRACE9 ("XApplication::command (int) - Protocoll: " << file.substr (0, 7));

   // Test if file-protocoll or no protocoll at all
   if (((file[0] == '/') && (file[1] != '/'))
       || (file.substr (0, 7) == "file://")) {
      if (file[0] != '/')
	 file.replace (0, 7, 0, '\0');

      // If so: Check which language to use; either using the LANGUAGE
      // environment variable or the locale settings
      const char* pLang (getenv ("LANGUAGE"));
#ifdef HAVE_LC_MESSAGES
      YGP::Tokenize ext (pLang ? pLang : setlocale (LC_MESSAGES, NULL));
#else
      YGP::Tokenize ext (pLang ? pLang : getenv ("LANG"));
#endif

      // Check every language-entry (while removing trailing specifiers)
      std::string extension;
      struct stat sfile;
      while ((extension = ext.getNextNode (':')).size ()) {
	 std::string search;
	 do {
	    search = file + std::string (1, '.') + extension;

	    TRACE9 ("XApplication::command (int) - Checking for help-file "
		    << search);
	    if (!::stat (search.c_str (), &sfile) && (sfile.st_mode & S_IFREG))
	       break;

	    unsigned int pos (extension.rfind ('_'));
	    if (pos == std::string::npos)
	       pos = 0;
	    extension.replace (pos, extension.length (), 0, '\0');
	 } while (extension.size ());

	 if (extension.size ()) {
	    file += '.';
	    file += extension;
	    break;
	 }
      } // end-while

      // Nothing worked: Check if file exists directly; if not try english
      if (::stat (file.c_str (), &sfile) || !(sfile.st_mode & S_IFREG))
	 file += ".en";
   } // endif file-protocoll
   TRACE5 ("XApplication::command (int) - Starting browser with " << file);

   try {
#ifdef HAVE_VIEWER
      if (helpBrowser == "GTKHTML")
	 HTMLViewer::create (file);
      else {
#endif
	 file = "file://" + file;
	 const char* const args[] = { helpBrowser.c_str (), file.c_str (), NULL };
	 YGP::Process::execAsync (helpBrowser.c_str (), args);
#ifdef HAVE_VIEWER
      }
#endif
   }
   catch (std::string& error) {
      if (error.size ()) {
	 Gtk::MessageDialog msg (Glib::locale_to_utf8 (error),
				 Gtk::MESSAGE_ERROR);
	 msg.run ();
      }
   }
}

//----------------------------------------------------------------------------
/// Shows the dialog to select the browser to display the help
//----------------------------------------------------------------------------
void XApplication::selectHelpBrowser () {
   Check3 (getHelpfile ());
   BrowserDlg::create (helpBrowser);
}

//----------------------------------------------------------------------------
/// Returns the name of the help file to display.
/// \returns const char*: Name of help-file (NULL: none)
//----------------------------------------------------------------------------
const char* XApplication::getHelpfile () {
   return NULL;
}

//----------------------------------------------------------------------------
/// Displays the about box. See also XAbout for a dialog implementing one.
//----------------------------------------------------------------------------
void XApplication::showAboutbox () {
}

//----------------------------------------------------------------------------
/// Sets the program icon which is used by some window managers when minimizing
/// the program.
/// \param pIconData Array of character pointers describing the icon (xpm-format)
/// \pre pIconData must be a valid pointer to xpm-data
/// \remarks: Some window managers might also display the icon on other occassions.
//----------------------------------------------------------------------------
void XApplication::setIconProgram (const char* const* pIconData) {
   TRACE9 ("XApplication::setIconProgram");
   Check1 (pIconData);

   set_icon (Gdk::Pixbuf::create_from_xpm_data (pIconData));
}


//-----------------------------------------------------------------------------
/// Constructor for programs with program information in the client.
/// \param pTitle: Pointer to title
/// \param pPrgInfo: Pointer to text describing the application
/// \param pCopyright: Pointer to copyright-information
//-----------------------------------------------------------------------------
XInfoApplication::XInfoApplication (const char* pTitle, const char* pPrgInfo,
                                    const char* pCopyright)
   : XApplication (pTitle), hboxTitle (new Gtk::HBox)
     , vboxPrgInfo (new Gtk::VBox), txtProgramm (new Gtk::Label (pPrgInfo))
     , txtCopyright (new Gtk::Label (pCopyright)), iconPrg (NULL)
     , iconAuthor (NULL) {
   TRACE9 ("XInfoApplication::XInfoApplication ()");
   Check1 (pPrgInfo); Check1 (pCopyright);

   hboxTitle->show ();
   vboxClient->pack_start (*hboxTitle, false, false, 5);

   vboxPrgInfo->show ();
   hboxTitle->pack_end (*vboxPrgInfo);

   txtProgramm->show ();
   vboxPrgInfo->pack_start (*txtProgramm);

   txtCopyright->show ();
   vboxPrgInfo->pack_start (*txtCopyright);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XInfoApplication::~XInfoApplication () {
   TRACE9 ("XInfoApplication::~XInfoApplication ()");

   txtCopyright->hide ();
   txtProgramm->hide ();
   vboxPrgInfo->hide ();
   hboxTitle->hide ();
}


//-----------------------------------------------------------------------------
/// Sets an icon for the program
/// \param pIconData: Pointer to xpm-data for pixmap
//-----------------------------------------------------------------------------
void XInfoApplication::setIconProgram (const char* const* pIconData) {
   TRACE9 ("XInfoApplication::setIconProgram");
   Check1 (pIconData);
   Check3 (hboxTitle);

   iconPrg = new Gtk::Image
      (Gdk::Pixbuf::create_from_xpm_data (pIconData));
   Check3 (iconPrg);

   iconPrg->show ();
   hboxTitle->pack_start (*iconPrg, false, false, 5);

   XApplication::setIconProgram (pIconData);
}

//-----------------------------------------------------------------------------
/// Sets pixmap for the programmer
/// \param pIconData: Pointer to xpm-data for pixmap
//-----------------------------------------------------------------------------
void XInfoApplication::setIconAuthor (const char* const* pIconData) {
   TRACE9 ("XInfoApplication::setIconAuthor");
   Check1 (pIconData);
   Check3 (hboxTitle); Check3 (vboxPrgInfo);

   iconAuthor = new Gtk::Image
      (Gdk::Pixbuf::create_from_xpm_data (pIconData));
   Check3 (iconAuthor);

   iconAuthor->show ();
   hboxTitle->pack_end (*iconAuthor, false, false, 5);
   hboxTitle->reorder_child (*vboxPrgInfo, 3);
}

}
