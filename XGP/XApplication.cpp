//$Id: XApplication.cpp,v 1.2 2000/01/23 23:06:37 Markus Rel $

//PROJECT     : XGeneral
//SUBSYSTEM   : XApplication
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
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
#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
#include <gtk--/menubar.h>
#include <gtk--/menu.h>
#include <gtk--/radiomenuitem.h>
#else
#include <gtk--/itemfactory.h>
#endif

#define DEBUG 0
#include "Trace.h"

#include "XApplication.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor for program without prg-info in client
//Parameters: pTitle: Pointer to title
/*--------------------------------------------------------------------------*/
XApplication::XApplication (const char* pTitle)
   : vboxClient (new Gtk_VBox ())
#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
   , accels (Gtk_AccelGroup::create ()), pLastMenu (NULL)
#else
  , accels (new Gtk_AccelGroup)
#endif
{
   TRACE9 ("XApplication::XApplication");

   Check3 (accels); Check3 (vobxClient);

   Check3 (pTitle);
   set_title (pTitle);

   vboxClient->show ();
   add (*vboxClient);

#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
   pMenu = new Gtk_MenuBar (); Check3 (pMenu);

   pMenu->show ();
   vboxClient->pack_start (*pMenu, false);
#else
   // pMenu needs an accelerator-group -> Create at end
   pMenu = new Gtk_ItemFactory_MenuBar ("<Main>", *accels); Check3 (pMenu);

   Gtk_ObjectHandle<Gtk_MenuBar> menubar (pMenu->get_menubar_widget (""));
   Check3 (menubar.get_object ());
   menubar->show ();

   vboxClient->pack_start (*menubar, false);
#endif

   add_accel_group (*accels);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds one menuentry
//Parameters: menuEntry: Entry to add
/*--------------------------------------------------------------------------*/
Gtk_Widget* XApplication::addMenu (const MenuEntry& menuEntry) {
   TRACE1 ("XApplication::addMenu: " << menuEntry.path);
   Check3 (pMenu);

#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
   Check3 (accels);
   using namespace Gtk_Menu_Helpers;

   string menu (menuEntry.path.substr (menuEntry.path.find_last_of ('/') + 1));
   char chType (menuEntry.type.empty () ? 'I' : menuEntry.type[1]);
   TRACE3 ("XApplication::addMenu -> Type (" << menu << ") = " << chType);

   Element* pItem (NULL);

   switch (chType) {
   case 'I':
   case 'C':
      Check3 (pLastMenu);
      if (chType == 'I')
         pItem = new MenuElem (menu, accels,
			       bind (slot (this, &command), menuEntry.id));
      else {
         pItem = new CheckMenuElem (menu, accels,
				    bind (slot (this, &command), menuEntry.id));
         ((Gtk_CheckMenuItem*)pItem->get_child ())->set_show_toggle (true);
      } // endif

      if (!menuEntry.accel.empty ()) {
	 TRACE3 ("XApplication::addMenu -> Accelerator = " << menuEntry.accel);

	 guint           keyAccel;
	 GdkModifierType modAccel;
	 gtk_accelerator_parse (menuEntry.accel.c_str (), &keyAccel, &modAccel);
	 pItem->get_child ()->add_accelerator
	    ("activate", *accels, keyAccel, modAccel,
	     GtkAccelFlags(GTK_ACCEL_VISIBLE | GTK_ACCEL_SIGNAL_VISIBLE));
      } // endif
      break;

   case 'S':
      Check3 (pLastMenu);
      pItem = new SeparatorElem ();
      break;

   case 'B':
   case 'L': {
      pLastMenu = new Gtk_Menu (); Check3 (pLastMenu);
      MenuElem* pElem (new MenuElem (menu, accels, NULL, GDK_MOD1_MASK));
      pElem->set_submenu (*pLastMenu);
      if (chType == 'L')
         pElem->right_justify ();

      Check3 (pMenu);
      pMenu->items ().push_back (*pElem);
      return pLastMenu;
      }

   default:
      Check (0);
   } // end-switch type of menu

   if (pItem) {
      Check3 (pLastMenu);
      pLastMenu->items ().push_back (*pItem);
      return pItem->get_child ();
   } // endif

   return NULL;
#else
   return pMenu->create_item (menuEntry.path, menuEntry.accel, menuEntry.type,
                       ItemFactoryConnector<XApplication, int>
                           (this, &XApplication::command, menuEntry.id));
#endif
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
   : XApplication (pTitle), hboxTitle (new Gtk_HBox)
   , vboxPrgInfo (new Gtk_VBox), txtProgramm (new Gtk_Label (pPrgInfo))
   , txtCopyright (new Gtk_Label (pCopyright)) {
   TRACE9 ("XInfoApplication::XInfoApplication");
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
   TRACE9 ("XInfoApplication::setIconProgram");

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
   TRACE9 ("XInfoApplication::setIconAuthor");

   Check3 (hboxTitle); Check3 (vboxPrgInfo);

   iconAuthor = new Gtk_Pixmap (picAuthor, Gdk_Bitmap ()); Check3 (iconAuthor);

   iconAuthor->show ();
   hboxTitle->pack_end (*iconAuthor, false, false, 5);
   hboxTitle->reorder_child (*vboxPrgInfo, 3);
}
