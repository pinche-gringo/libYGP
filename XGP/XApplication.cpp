//$Id: XApplication.cpp,v 1.28 2003/07/25 00:24:52 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.28 $
//AUTHOR      : Markus Schwab
//CREATED     : 4.9.1999
//COPYRIGHT   : Anticopyright (A) 1999 - 2003

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
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/menubar.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/radiomenuitem.h>

#include <Internal.h>

#include <Check.h>
#include <Trace_.h>
#include <Process_.h>
#include <StackTrc.h>

#include <Tokenize.h>

#include "BrowserDlg.h"

#include "XApplication.h"

using namespace Gtk::Menu_Helpers;


#undef HAVE_GTKHTML
#ifdef HAVE_GTKHTML
#  include <dlfcn.h>

extern "C" {
#  include <libgtkhtml/gtkhtml.h>
}

#  include <XDialog.h>


// Helper class to display a HTML document in a dialog
class HTMLViewer : public XDialog {
 public:
    static void perform (const std::string& file) throw (std::string);

 private:
    HTMLViewer (const std::string& file) throw (std::string);
    ~HTMLViewer ();

    void display (const std::string& file) throw (std::string);

    static HTMLViewer* instance;
    GtkWidget* htmlCtrl;
    void* hDLL;
};


//----------------------------------------------------------------------------
/// Creates (or updates) a dialog displaying a HTML-document
/// \param file: File containing the HTML-document to display
/// \throw std::string in case of error
//----------------------------------------------------------------------------
void HTMLViewer::perform (const std::string& file) throw (std::string) {
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
   Check1 (file.size ());
   typedef GtkWidget* (*PFNNEWHTMLVIEW)(void);

   hDLL = dlopen ("libgtkhtml-2.so", 0x00001);
   if (hDLL) {
      PFNNEWHTMLVIEW pFnc ((PFNNEWHTMLVIEW)dlsym (hDLL, "html_view_new"));
      if (pFnc) {
         htmlCtrl = pFnc ();
         gtk_widget_show (htmlCtrl);
         get_vbox ()->pack_start (*Glib::wrap (htmlCtrl, false));

         display (file);
         show ();
      }
      dlclose (hDLL);
      return;
   }

   std::string err (_("Can't display the HTML control!\n\nReason: %1"));
   err.replace (err.find ("%1"), 2, dlerror ());
   throw (err);
}

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
HTMLViewer::~HTMLViewer () {
   delete htmlCtrl;
}


//----------------------------------------------------------------------------
/// Sets the HTML-document to display in the dialog
/// \param file: File containing the HTML-document to display
/// \throw std::string in case of error
//----------------------------------------------------------------------------
void HTMLViewer::display (const std::string& file) throw (std::string) {
   Check1 (hDLL);
   Check1 (file.size ());

   typedef HtmlDocument* (*PFNNEWDOCUMENT)();
   typedef gboolean (*PFNDOCUMENTOPEN)(HtmlDocument*, const gchar);
   typedef void (*PFNSETDOCUMENT)(HtmlView*, HtmlDocument*);

   PFNNEWDOCUMENT pfnNewDoc ((PFNNEWDOCUMENT)dlsym (hDLL, "html_document_new"));
   PFNDOCUMENTOPEN pfnOpenDoc ((PFNDOCUMENTOPEN)dlsym (hDLL, "html_document_open_stream"));
   PFNSETDOCUMENT pfnSetDoc ((PFNSETDOCUMENT)dlsym (hDLL, "html_view_set_document"));

   if (!(pfnNewDoc && pfnOpenDoc && pfnSetDoc)) {
      std::string err (_("Can't display the HTML control!\n\nReason: %1"));
      err.replace (err.find ("%1"), 2, dlerror ());
      throw (err);
      return;
   }

   HtmlDocument* doc (pfnNewDoc ());
   if (!pfnOpenDoc (doc, file.c_str ())) {
      std::string err (_("Can't display the HTML document!\n\nReason: %1"));
      err.replace (err.find ("%1"), 2, dlerror ());
      throw (err);
   }
   else
      pfnSetDoc (htmlCtrl, doc);
}
#endif


//-----------------------------------------------------------------------------
/// Constructor; creates a program-window showing only an empty menu and an
/// area (a GTK::VBox) for the client. Furthermore \c SIGSEGV and \c SIGBUS
/// signals are trapped to produce a stackdump in the log file.
/// \param pTitle: Pointer to title of the application
//-----------------------------------------------------------------------------
XApplication::XApplication (const char* pTitle)
   : pMenu (new Gtk::MenuBar ()), vboxClient (new Gtk::VBox ())
     , helpBrowser (BrowserDlg::getDefaultBrowser ()), aLastMenus (5) {
   TRACE9 ("XApplication::XApplication (const char*) - " << pTitle);
   signal (SIGSEGV, handleSignal);
   signal (SIGBUS, handleSignal);

   Check3 (pTitle);
   set_title (pTitle);

   Check3 (vboxClient);
   vboxClient->show ();
   add (*vboxClient);

   Check3 (pMenu);
   pMenu->show ();
   vboxClient->pack_start (*pMenu, Gtk::PACK_SHRINK);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XApplication::~XApplication () {
   TRACE9 ("XApplication::~XApplication () - start");

   signal (SIGSEGV, SIG_DFL);
   signal (SIGBUS, SIG_DFL);
}


//-----------------------------------------------------------------------------
/// Adds a single menuentry to the menu. Items and submenus are added to the
/// last defined menu; menus are added to the menubar.
/// \param menuEntry: Menuentry to add
/// \returns \c Widget*: Pointer to the newly added menu or menuitem
/// \remarks Radioitems can't be added with that method!
//-----------------------------------------------------------------------------
Gtk::Widget& XApplication::addMenu (const MenuEntry& menuEntry) {
   TRACE1 ("XApplication::addMenu (const MenuEntry&) - " << menuEntry.name);
   Check3 (pMenu);

   TRACE3 ("XApplication::addMenu (const MenuEntry&) -> Type (" << menuEntry.name << ") = "
           << menuEntry.type);

   TRACE9 ("XApplication::addMenu (const MenuEntry&) - Levels: " << aLastMenus.size ());
   Gtk::Menu* pLastMenu (aLastMenus.size () ? aLastMenus.back () : NULL);

   switch (menuEntry.type) {
   case ITEM:
      Check3 (pLastMenu);
      pLastMenu->items ().push_back (MenuElem (menuEntry.name, AccelKey (menuEntry.accel),
                                               bind (slot (*this, &XApplication::command),
                                                     menuEntry.id)));
      break;

   case CHECKITEM:
      Check3 (pLastMenu);
      pLastMenu->items ().push_back (CheckMenuElem (menuEntry.name, AccelKey (menuEntry.accel),
                                                    bind (slot (*this, &XApplication::command),
                                                          menuEntry.id)));
      break;

   case SEPARATOR:
      Check3 (pLastMenu);
      pLastMenu->items ().push_back (SeparatorElem ());
      break;

   case BRANCH:
   case LASTBRANCH:
      aLastMenus.clear ();
      pLastMenu = manage (new Gtk::Menu ()); Check3 (pLastMenu);
      aLastMenus.insert (aLastMenus.end (), pLastMenu);

      pMenu->items ().push_back (MenuElem (menuEntry.name, AccelKey (menuEntry.accel), *pLastMenu));
      if (menuEntry.type == LASTBRANCH)
         pMenu->items ().back ().set_right_justified ();

      return *pLastMenu;

   case SUBMENU:
      Check3 (pLastMenu);
      pLastMenu = new Gtk::Menu ();

      aLastMenus.back ()->items ().push_back (MenuElem (menuEntry.name, AccelKey (menuEntry.accel), *pLastMenu));
      aLastMenus.push_back (pLastMenu);
      break;

   case SUBMENUEND:
      Check3 (pLastMenu);
      aLastMenus.pop_back ();
      break;

   default:
      Check (0);
   } // end-switch type of menu

   if (menuEntry.id) {
      Check (apMenus.find (menuEntry.id) == apMenus.end ());
      TRACE9 ("XApplication::addMenu (const MenuEntry&) Adding menu: " << menuEntry.id);
      apMenus[menuEntry.id] = &pLastMenu->items ().back ();
   }
      
   return pLastMenu->items ().back ();
}

//-----------------------------------------------------------------------------
/// Adds a whole bunch of menuentries to the menu. Items and submenus are
/// added to the last defined menu; menus are added to the menubar.
/// \param menuEntries: Pointer to array of MenuEntries
/// \param cMenus: Number of elements in the array
/// \pre \c menuEntries not NULL
//-----------------------------------------------------------------------------
void XApplication::addMenus (const MenuEntry menuEntries[], int cMenus) {
   TRACE9 ("XApplication::addMenus (const MenuEntry[], int) - " << cMenus);

   Check3 (menuEntries);
   Check3 (pMenu);

   while (cMenus) {
      if ((menuEntries->type == RADIOITEM)
          || (menuEntries->type == LASTRADIOITEM)) {
         Check3 (aLastMenus.size ());
         Gtk::Menu* pLastMenu (aLastMenus.back ());
         
         Gtk::RadioMenuItem::Group radioGroup;
         do {
            cMenus--;
            pLastMenu->items ().push_back (RadioMenuElem (radioGroup, menuEntries->name,
                                                          AccelKey (menuEntries->accel),
                                                          bind (slot (*this, &XApplication::command),
                                                                menuEntries->id)));
            if (menuEntries->id) {
               Check (apMenus.find (menuEntries->id) == apMenus.end ());
               TRACE9 ("XApplication::addMenus (const MenuEntry[]) - Adding menu: "
                       << menuEntries->id);
               apMenus[menuEntries->id] = &pLastMenu->items ().back ();
            }
         } while ((menuEntries++)->type != LASTRADIOITEM);
      }
      else {
         cMenus--;
         addMenu (*menuEntries++);
      }
   }
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
//-----------------------------------------------------------------------------
void XApplication::showHelpMenu () {
   MenuEntry menuItems[] = {
      { Glib::locale_to_utf8 (_("_Help")),                  _("<alt>H"), 0,                LASTBRANCH },
      { Glib::locale_to_utf8 (_("_Content...")),            _("F1"),     CONTENT,          ITEM },
      { Glib::locale_to_utf8 (_("Configure _browser ...")), "",          CONFIGUREBROWSER, ITEM },
      { "",                                                 "",          0,                SEPARATOR },
      { Glib::locale_to_utf8 (_("_About...")),              "",          ABOUT,            ITEM } };

   addMenu (menuItems[0]);
   if (getHelpfile ())
      addMenus (&menuItems[1], 4);
   else
      addMenu (menuItems[4]);
}

//-----------------------------------------------------------------------------
/// Command-handler; handles the help menu entries.
/// \param menu: ID of command (menu)
//-----------------------------------------------------------------------------
void XApplication::command (int menu) {
   switch (menu) {
   case ABOUT:
      showAboutbox ();
      break;

   case CONTENT: {
      Check3 (getHelpfile ());
      std::string file (getHelpfile ());
      TRACE9 ("XApplication::command (int) - Show help " << file);
      TRACE9 ("XApplication::command (int) - Protocoll: " << file.substr (0, 7));

      // Test if file-protocoll or no protocoll at all
      if (((file[0] == '/') && (file[1] != '/'))
          || (file.substr (0, 7) == "file://")) {
         if (file[0] != '/')
            file.replace (0, 6, 0, '\0');

         // If so: Check which language to use
         Tokenize ext (getenv ("LANGUAGE"));
         if (ext.getActNode ().empty ())
            ext = setlocale (LC_MESSAGES, NULL);

         // Check every language-entry (while removing trailing specifiers)
         std::string extension;
         struct stat sfile;
         while ((extension = ext.getNextNode (',')).size ()) {
            std::string search;
            do {
               search = file + std::string (1, '.') + extension;
               if (search.substr (0, 7) == "file://")
                  search.replace (0, 6, 0, '\0');

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
#ifdef HAVE_GTKHTML
         if (helpBrowser == "GTKHTML")
            HTMLDialog::perform (file);
         else {
#endif
            const char* const args[] = { helpBrowser.c_str (), file.c_str (), NULL };
            Process::execAsync (helpBrowser.c_str (), args);
#ifdef HAVE_GTKHTML
         }
#endif
      }
      catch (std::string& error) {
         Gtk::MessageDialog msg (error, Gtk::MESSAGE_ERROR);
         msg.run ();
      }
      break; }

   case CONFIGUREBROWSER:
      Check3 (getHelpfile ());
      BrowserDlg::create (helpBrowser);
      break;

   default:
      Check3 (0);
   }
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
   Check3 (hboxTitle);

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
