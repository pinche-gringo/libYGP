#ifndef XAPPLICATION_H
#define XAPPLICATION_H

//$Id: XApplication.h,v 1.14 2003/01/14 20:51:34 markus Exp $

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

#include <map>
#include <vector>

#include <gtk--/main.h>
#include <gtk--/menu.h>
#include <gtk--/window.h>

#include "SmartPtr.h"


// Forward declarations
namespace Gtk {
   class Main;
   class HBox;
   class VBox;
   class Label;
   class Pixmap;
   class MenuBar;
   class AccelGroup;
}

using namespace Gtk;

typedef SmartPtr<HBox>           PHBox;
typedef SmartPtr<Label>          PLabel;
typedef SmartPtr<Pixmap>         PPixmap;
typedef SmartPtr<MenuBar>        PMenuBar;
typedef SmartPtr<AccelGroup>     PAccelGroup;


// Baseclass for X-applications; it creates an application-window with a
// client. The virtual method command is used to handle the commands defined
// with the menus.
//
// Furthermore exist the addMenu(s) to create the menu-structure. Note that
// separators and items are added to the last branch (so you better start with
// one of the latter ;). The menus can be accessed via the apMenus map.
//
// Menus to add are specified by objects of type XApplication::MenuEntry which
// consists of the following entries:
//
//   - name: Name of the menu as it should be displayed.
//   - accel: Accelerator in a format understood by Gtk (like <alt>a).
//   - id: Identifier for the menu. Menus causing actions must have an unique
//         ID. Entries having an ID other than 0 are stored in the protected
//         member apMenus for later usage.
//   - type: Type of the menu; the following values are supported:
//             * ITEM: Ordinary menu entry, usually causing an action.
//             * CHECKITEM: Menu entry having one of two states.
//             * RADIOITEM: Menu entries which allows the selection of only one
//                          in a group.
//             * LASTRADIOITEM: As an RADIOITEM, but terminating the group.
//             * SEPARATOR: A separating line in between any of the above items.
//             * BRANCH: A new entry in the menu bar, usually causing no action.
//             * LASTBRANCH: As BRANCH but causing the entry to be aligned to
//                           the right of the menu bar.
//             * SUBMENU: Causes the creation of a new menu inside the previous
//                        menu.
//             * SUBMENUEND: As SUBMENU but terminates the last submenu.
//
// Note: Due to limitations in the Gtk-- toolkit (or my stupidity) you can NOT
// add RadioMenuItems with the addMenu method; but must use the addMenus method
// (as Gtk-- needs a unique group for them, which must be allocated on the
// stack).
class XApplication : public Window {
 public:
   // Manager functions
   XApplication (const char* pTitle);
   ~XApplication ();         // No need to be virtual. There´s only 1 instance

   static void initI18n (const char* package, const char* dir);

 protected:
   typedef enum { ITEM, CHECKITEM, RADIOITEM, LASTRADIOITEM, SEPARATOR, BRANCH,
                  SUBMENU, SUBMENUEND, LASTBRANCH } menuTypes;

   enum { ABOUT = 1, CONTENT, CONFIGUREBROWSER, LAST };

   // Menu-handling (including callback for menu-events)
   typedef struct {
      const string name;
      const string accel;
      unsigned int id;
      menuTypes    type;
   } MenuEntry;
   typedef SmartPtr<VBox>           PVBox;

   Widget* addMenu (const MenuEntry& menuEntry);
   void        addMenus (const MenuEntry menuEntryies[], int cMenus);
   virtual void command (int menu) = 0;

   VBox* getClient () const { return vboxClient; }

   // Protected data
   PMenuBar pMenu;
   PVBox    vboxClient;

   map <unsigned int, Widget*> apMenus;

   //Help-menu
   void showHelpMenu ();

   //Callbacks for help
   virtual const char* getHelpfile () { return NULL; }
   virtual void showAboutbox () { }

   string helpBrowser;

 private:
   // Protected manager functions
   XApplication (const XApplication&);
   const XApplication& operator= (const XApplication&);

   // Events
   gint delete_event_impl (_GdkEventAny*) {
      Main::quit ();
      return 0;
   }

   vector<Menu*> aLastMenus;
};


// Baseclass for X-applications showing the program-information inside the
// client.
//
// The top of the client contains 2 icons for program and programmer and two
// lines for program-information and about the author.
class XInfoApplication : public XApplication {
 public:
   XInfoApplication (const char* pTitle, const char* pPrgInfo, const char* pCopyright);
   ~XInfoApplication ();     // No need to be virtual. There´s only 1 instance

 protected:
   // Add information
   void setIconProgram (const char* const* iconData);
   void setIconAuthor (const char* const* iconData);

 private:
   // Protected manager functions
   XInfoApplication (const XInfoApplication&);
   const XInfoApplication& operator= (const XInfoApplication&);

   PHBox  hboxTitle;
   PVBox  vboxPrgInfo;
   PLabel txtProgramm;
   PLabel txtCopyright;

   PPixmap iconPrg;
   PPixmap iconAuthor;
};

#endif
