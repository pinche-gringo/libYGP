//$Id: XApplication.cpp,v 1.17 2003/01/31 23:47:17 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.17 $
//AUTHOR      : Markus Schwab
//CREATED     : 4.9.1999
//COPYRIGHT   : Anticopyright (A) 1999

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

#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <locale.h>
#include <unistd.h>

#include <sys/stat.h>

#include "Check.h"
#include "StackTrc.h"

#include <gtk--/box.h>
#include <gtk--/label.h>
#include <gtk--/pixmap.h>
#include <gtk--/accelgroup.h>
#include <gtk--/radiomenuitem.h>

#include <gtk--/menubar.h>

#include <Internal.h>

#include <Trace_.h>
#include <Tokenize.h>

#include "BrowserDlg.h"

#include "XApplication.h"

using namespace Menu_Helpers;


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; creates a program-window showing only an empty menu
//            and an area (a GTK::VBox) for the client.
//
//            Furthermore SIGSEGV and SIGBUS signals are trapped to produce
//            a stackdump in the log file.
//Parameters: pTitle: Pointer to title of the application
/*--------------------------------------------------------------------------*/
XApplication::XApplication (const char* pTitle)
   : vboxClient (new VBox ()), aLastMenus (5), pMenu (new MenuBar ())
     , helpBrowser (BrowserDlg::getDefaultBrowser ())
{
   TRACE9 ("XApplication::XApplication ()");

   signal (SIGSEGV, handleSignal);
   signal (SIGBUS, handleSignal);

   //Check3 (accels);
   Check3 (vboxClient);

   Check3 (pTitle);
   set_title (pTitle);

   vboxClient->show ();
   add (*vboxClient);

   Check3 (pMenu);
   pMenu->show ();
   vboxClient->pack_start (*pMenu, false);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XApplication::~XApplication () {
   TRACE9 ("XApplication::~XApplication () - start");

   vboxClient->hide ();
   pMenu->hide ();
   hide ();
   signal (SIGSEGV, SIG_DFL);
   signal (SIGBUS, SIG_DFL);

   TRACE9 ("XApplication::~XApplication () - end");
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds a single menuentry to the menu. Items and submenus are
//            added to the last defined menu; menus are added to the menubar.
//Parameters: menuEntry: Menuentry to add
//Returns   : Widget*: Pointer to the newly added menu or menuitem
//Remarks   : Radioitems can't be added with that method!
/*--------------------------------------------------------------------------*/
Widget* XApplication::addMenu (const MenuEntry& menuEntry) {
   TRACE1 ("XApplication::addMenu (const MenuEntry&) - " << menuEntry.name);
   Check3 (pMenu);

   TRACE3 ("XApplication::addMenu (const MenuEntry&) -> Type (" << menuEntry.name << ") = "
           << menuEntry.type);

   TRACE9 ("XApplication::addMenu (const MenuEntry&) - Levels: " << aLastMenus.size ());
   Menu* pLastMenu (aLastMenus.size () ? aLastMenus.back () : NULL);

   switch (menuEntry.type) {
   case ITEM:
      Check3 (pLastMenu);
      pLastMenu->items ().push_back (MenuElem (menuEntry.name, menuEntry.accel,
                                               bind (slot (this, &XApplication::command),
                                                     menuEntry.id)));
      break;

   case CHECKITEM:
      Check3 (pLastMenu);
      pLastMenu->items ().push_back (CheckMenuElem (menuEntry.name, menuEntry.accel,
                                                    bind (slot (this, &XApplication::command),
                                                          menuEntry.id)));
      static_cast<CheckMenuItem*> (pLastMenu->items ().back ())->set_show_toggle (true);
      break;

   case SEPARATOR:
      Check3 (pLastMenu);
      pLastMenu->items ().push_back (SeparatorElem ());
      break;

   case BRANCH:
   case LASTBRANCH:
      aLastMenus.clear ();
      pLastMenu = manage (new Menu ()); Check3 (pLastMenu);
      aLastMenus.insert (aLastMenus.end (), pLastMenu);

      pMenu->items ().push_back (MenuElem (menuEntry.name, menuEntry.accel, *pLastMenu));
      if (menuEntry.type == LASTBRANCH)
         pMenu->items ().back ()->right_justify ();

      return pLastMenu;

   case SUBMENU:
      Check3 (pLastMenu);
      pLastMenu = new Menu ();

      aLastMenus.back ()->items ().push_back (MenuElem (menuEntry.name, menuEntry.accel, *pLastMenu));
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
      apMenus[menuEntry.id] = pLastMenu->items ().back ();
   }
      
   return pLastMenu->items ().back ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a whole bunch of menuentries to the menu. Items and
//            submenus are added to the last defined menu; menus are added to
//            the menubar.
//Parameters: menuEntries: Pointer to array of MenuEntries
//            cMenus: Number of elements in the array
//Requires  : menuEntries not NULL
/*--------------------------------------------------------------------------*/
void XApplication::addMenus (const MenuEntry menuEntries[], int cMenus) {
   TRACE9 ("XApplication::addMenus (const MenuEntry[], int) - " << cMenus);

   Check3 (menuEntries);
   Check3 (pMenu);

   while (cMenus) {
      if ((menuEntries->type == RADIOITEM)
          || (menuEntries->type == LASTRADIOITEM)) {
         Check3 (aLastMenus.size ());
         Menu* pLastMenu (aLastMenus.back ());
         
         Gtk::RadioMenuItem::Group radioGroup;
         do {
            cMenus--;
            pLastMenu->items ().push_back (RadioMenuElem (radioGroup, menuEntries->name,
                                                          menuEntries->accel,
                                                          bind (slot (this, &XApplication::command),
                                                                menuEntries->id)));
            if (menuEntries->id) {
               Check (apMenus.find (menuEntries->id) == apMenus.end ());
               TRACE9 ("XApplication::addMenus (const MenuEntry[]) - Adding menu: "
                       << menuEntries->id);
               apMenus[menuEntries->id] = pLastMenu->items ().back ();
            }
         } while ((menuEntries++)->type != LASTRADIOITEM);
      }
      else {
         cMenus--;
         addMenu (*menuEntries++);
      }
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Initializes the program for internationalition by setting the
//            locale and loading the messagefile.
//Parameters: package: Name of the message-catalog
//            dir: root-directory for message-catalogs
/*--------------------------------------------------------------------------*/
void XApplication::initI18n (const char* package, const char* dir) {
   assert (package); assert (dir);

   setlocale (LC_ALL, "");                         // Activate current locale
   bindtextdomain (package, dir);
   textdomain (package);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a help-menu at the end of the menu
/*--------------------------------------------------------------------------*/
void XApplication::showHelpMenu () {
   MenuEntry menuItems[] = {
      { _("_Help"),                  _("<alt>H"), 0,                LASTBRANCH },
      { _("_Content..."),            _("F1"),     CONTENT,          ITEM },
      { _("Configure _browser ..."), "",          CONFIGUREBROWSER, ITEM },
      { "",                          "",          0,                SEPARATOR },
      { _("_About..."),              "",          ABOUT,            ITEM } };

   addMenu (menuItems[0]);
   if (getHelpfile ())
      addMenus (&menuItems[1], 4);
   else
      addMenu (menuItems[4]);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Command-handler
//Parameters: menu: ID of command (menu)
/*--------------------------------------------------------------------------*/
void XApplication::command (int menu) {
   switch (menu) {
   case ABOUT:
      showAboutbox ();
      break;

   case CONTENT: {
      Check3 (getHelpfile ());
      pid_t pid (fork ());
      
      switch (pid) {
      case 0: {                                         // Child: Start browser
         string file (getHelpfile ());
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
            string extension;
            struct stat sfile;
            while ((extension = ext.getNextNode (',')).size ()) {
               string search;
               do {
                  search = file + std::string (1, '.') + extension;
                  if (search.substr (0, 7) == "file://")
                     search.replace (0, 6, 0, '\0');

                  TRACE9 ("XApplication::command (int) - Checking for help-file "
                          << search);
                  if (!::stat (search.c_str (), &sfile) && (sfile.st_mode & S_IFREG))
                     break;

                  unsigned int pos (extension.rfind ('_'));
                  if (pos == string::npos)
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

         TRACE5 ("XApplication::command (int) - Starting browser with "
                 << file);

         execlp (helpBrowser.c_str (), helpBrowser.c_str (), file.c_str (), NULL);
         perror (_("Error starting browser for help! Reason"));
         _exit (1);
         break; }

      case -1: {
         string errMsg (_("Error starting browser for help!\n\nReason: "));
         errMsg += strerror (errno);
         gdk_threads_enter ();
         XMessageBox::Show (errMsg, XMessageBox::ERROR | XMessageBox::OK);
         gdk_threads_leave ();
         break; }
      }
      break; }

   case CONFIGUREBROWSER:
      Check3 (getHelpfile ());
      BrowserDlg::perform (helpBrowser);
      break;

   default:
      Check3 (0);
   }
}


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor for program with prg-info in client
//Parameters: pTitle: Pointer to title
//            pPrgInfo: Pointer to text describing the application
//            pCopyright: Pointer to copyright-information
/*--------------------------------------------------------------------------*/
XInfoApplication::XInfoApplication (const char* pTitle, const char* pPrgInfo,
                                    const char* pCopyright)
   : XApplication (pTitle), hboxTitle (new HBox)
   , vboxPrgInfo (new VBox), txtProgramm (new Label (pPrgInfo))
   , txtCopyright (new Label (pCopyright)), iconAuthor (NULL), iconPrg (NULL) {
   TRACE9 ("XInfoApplication::XInfoApplication ()");
   Check3 (pPrgInfo); Check3 (pCopyright);

   hboxTitle->show ();
   vboxClient->pack_start (*hboxTitle, false, false, 5);

   vboxPrgInfo->show ();
   hboxTitle->pack_end (*vboxPrgInfo);

   txtProgramm->show ();
   vboxPrgInfo->pack_start (*txtProgramm);

   txtCopyright->show ();
   vboxPrgInfo->pack_start (*txtCopyright);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XInfoApplication::~XInfoApplication () {
   TRACE9 ("XInfoApplication::~XInfoApplication ()");

   txtCopyright->hide ();
   txtProgramm->hide ();
   vboxPrgInfo->hide ();
   hboxTitle->hide ();
}


/*--------------------------------------------------------------------------*/
//Purpose   : Sets pixmap for the program
//Parameters: pIconData: Pointer to xpm-data for pixmap
/*--------------------------------------------------------------------------*/
void XInfoApplication::setIconProgram (const char* const* iconData) {
   TRACE9 ("XInfoApplication::setIconProgram");
   if (iconPrg)
      return;

   Check3 (hboxTitle);

   iconPrg = new Pixmap (iconData);
   Check3 (iconPrg);

   iconPrg->show ();
   hboxTitle->pack_start (*iconPrg, false, false, 5);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets pixmap for the programmer
//Parameters: pIconData: Pointer to xpm-data for pixmap
/*--------------------------------------------------------------------------*/
void XInfoApplication::setIconAuthor (const char* const* iconData) {
   TRACE9 ("XInfoApplication::setIconAuthor");
   if (iconAuthor)
      return;

   Check3 (hboxTitle); Check3 (vboxPrgInfo);

   iconAuthor = new Pixmap (iconData);
   Check3 (iconAuthor);

   iconAuthor->show ();
   hboxTitle->pack_end (*iconAuthor, false, false, 5);
   hboxTitle->reorder_child (*vboxPrgInfo, 3);
}
