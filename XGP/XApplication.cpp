//$Id: XApplication.cpp,v 1.1 1999/11/13 01:18:07 Markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
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


#include <Check.h>

#include <gtk--/box.h>
#include <gtk--/label.h>
#include <gtk--/pixmap.h>
#include <gtk--/accelgroup.h>
#include <gtk--/itemfactory.h>

#include "XApplication.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor for program without prg-info in client
//Parameters: pTitle: Pointer to title
/*--------------------------------------------------------------------------*/
XApplication::XApplication (const char* pTitle)
   : vboxClient (new Gtk_VBox ()), accels (new Gtk_AccelGroup ()) {
   Check3 (accels); Check3 (vobxClient);

   // pMenu needs an accelerator-group -> Create at end
   pMenu = new Gtk_ItemFactory_MenuBar ("<Main>", *accels); Check3 (pMenu);

   add_accel_group (*accels);

   Check3 (pTitle);
   set_title (pTitle);

   vboxClient->show ();
   add (vboxClient);

   Gtk_ObjectHandle<Gtk_MenuBar> menubar (pMenu->get_menubar_widget (""));
   Check3 (menubar.get_object ());
   menubar->show ();

   vboxClient->pack_start (*menubar, false);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds one menuentry
//Parameters: menuEntry: Entry to add
/*--------------------------------------------------------------------------*/
Gtk_Widget* XApplication::addMenu (const MenuEntry& menuEntry) {
   Check3 (pMenu);
   return pMenu->create_item (menuEntry.path, menuEntry.accel, menuEntry.type,
                       ItemFactoryConnector<XApplication, int>
                           (this, &XApplication::command, menuEntry.id));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds menuentries
//Parameters: menuEntries: Pointer to array of MenuEntries
/*--------------------------------------------------------------------------*/
void XApplication::addMenus (const MenuEntry menuEntries[], int cMenus) {
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
   : XApplication (pTitle), hboxTitle (new Gtk_HBox)
   , vboxPrgInfo (new Gtk_VBox), txtProgramm (new Gtk_Label (pPrgInfo))
   , txtCopyright (new Gtk_Label (pCopyright)) {
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
//Purpose   : Sets pixmap for the program
//Parameters: picProgram: Pixmap for program
/*--------------------------------------------------------------------------*/
void XInfoApplication::setIconProgram (Gdk_Pixmap& picProgram) {
   Check3 (hboxTitle);

   iconPrg = new Gtk_Pixmap (picProgram, Gdk_Bitmap ()); Check3 (iconPrg);

   iconPrg->show ();
   hboxTitle->pack_start (*iconPrg, false, false, 5);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets pixmap for the programmer
//Parameters: picAuthor: Pixmap for programmer
/*--------------------------------------------------------------------------*/
void XInfoApplication::setIconAuthor (Gdk_Pixmap& picAuthor) {
   Check3 (hboxTitle); Check3 (vboxPrgInfo);

   iconAuthor = new Gtk_Pixmap (picAuthor, Gdk_Bitmap ()); Check3 (iconAuthor);

   iconAuthor->show ();
   hboxTitle->pack_end (*iconAuthor, false, false, 5);
   hboxTitle->reorder_child (*vboxPrgInfo, 3);
}
