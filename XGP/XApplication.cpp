//$Id: XApplication.cpp,v 1.11 2002/04/28 00:19:16 markus Rel $

//PROJECT     : XGeneral
//SUBSYSTEM   : XApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.11 $
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

#include <signal.h>

#include "Check.h"
#include "StackTrc.h"

#include <gtk--/box.h>
#include <gtk--/label.h>
#include <gtk--/pixmap.h>
#include <gtk--/accelgroup.h>

#include <gtk--/menubar.h>
#include <gtk--/menu.h>
#include <gtk--/radiomenuitem.h>

#define DEBUG 0
#include "Trace_.h"

#include "XApplication.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor for program without prg-info in client
//Parameters: pTitle: Pointer to title
/*--------------------------------------------------------------------------*/
XApplication::XApplication (const char* pTitle)
   : vboxClient (new VBox ())
   /*, accels (AccelGroup::create ())*/, pMenu (new MenuBar ()), pLastMenu (NULL)
{
   TRACE9 ("XApplication::XApplication ()");

   signal (SIGSEGV, handleSignal);
   signal (SIGBUS, handleSignal);

   //Check3 (accels);
   Check3 (vobxClient);

   Check3 (pTitle);
   set_title (pTitle);

   vboxClient->show ();
   add (*vboxClient);

   Check3 (pMenu);
   pMenu->show ();
   vboxClient->pack_start (*pMenu, false);

   // add_accel_group (*accels);
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
//Purpose   : Adds one menuentry
//Parameters: menuEntry: Entry to add
/*--------------------------------------------------------------------------*/
Widget* XApplication::addMenu (const MenuEntry& menuEntry) {
   TRACE1 ("XApplication::addMenu:  (const MenuEntry&) - " << menuEntry.path);
   Check3 (pMenu);

   using namespace Menu_Helpers;

   TRACE3 ("XApplication::addMenu -> Type (" << menuEntry.name << ") = " << chType);

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
      pLastMenu = manage (new Menu ()); Check3 (pLastMenu);
      Check3 (pMenu);
      pMenu->items ().push_back (MenuElem (menuEntry.name, menuEntry.accel, *pLastMenu));

      if (menuEntry.type == LASTBRANCH)
         pMenu->items ().back ()->right_justify ();

      return pLastMenu;

   default:
      Check (0);
   } // end-switch type of menu

   return pLastMenu->items ().back ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds menuentries
//Parameters: menuEntries: Pointer to array of MenuEntries
/*--------------------------------------------------------------------------*/
void XApplication::addMenus (const MenuEntry menuEntries[], int cMenus) {
   TRACE9 ("XApplication::addMenus (" << cMenus << ')');

   Check3 (menuEntries);
   Check3 (pMenu);

   while (cMenus--)
      addMenu (*menuEntries++);
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
