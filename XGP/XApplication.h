#ifndef XAPPLICATION_H
#define XAPPLICATION_H

//$Id: XApplication.h,v 1.10 2002/07/08 03:37:44 markus Exp $

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
// with the menus
//
// Furthermore exist the addMenu(s) to create the menu-structure. Note that
// separators and (check)items are added to the last branch (so you better
// start with one of the latter ;) Should you need a more flexible handling
// (for dynamic menus, for example), save the pointer to the branch (which is
// returned by addMenu))
class XApplication : public Window {
 public:
   // Manager functions
   XApplication (const char* pTitle);
   ~XApplication ();         // No need to be virtual. There´s only 1 instance

   // Events
   gint delete_event_impl (_GdkEventAny*) {
      Main::quit ();
      return 0;
   }

   static void initI18n (const char* package, const char* dir);

 protected:
   typedef enum { ITEM, CHECKITEM, SEPARATOR, BRANCH, LASTBRANCH } menuTypes;

   // Menu-handling (including callback for menu-events)
   typedef struct {
      const string name;
      const string accel;
      int          id;
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

 private:
   // Protected manager functions
   XApplication (const XApplication&);
   const XApplication& operator= (const XApplication&);

   Menu* pLastMenu;
};


// Baseclass for X-applications; it creates an application-window with a
// client. The client contains the program-info on top (2 icons for program
// and programmer, 2 lines for program-info and author)
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
