#ifndef XAPPLICATION_H
#define XAPPLICATION_H

//$Id: XApplication.h,v 1.7 2000/04/07 22:44:40 Markus Rel $

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
#include <gtk--/window.h>

#include "SmartPtr.h"


// Forward declarations
#if (GTKMM_MAJOR_VERSION > 1) || ((GTKMM_MAJOR_VERSION == 1) && GTKMM_MINOR_VERSION > 0)
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

#else
class Gtk_HBox;
class Gtk_VBox;
class Gtk_Label;
class Gtk_Widget;
class Gtk_Pixmap;
class Gtk_MenuBar;
class Gtk_AccelGroup;
class Gtk_ItemFactory_MenuBar;

typedef Gtk_Main                Main;
typedef Gtk_VBox                VBox;
typedef Gtk_HBox                HBox;
typedef Gtk_Label               Label;
typedef Gtk_Widget              Widget;
typedef Gtk_Window              Window;
typedef Gtk_Pixmap              Pixmap;
typedef Gtk_MenuBar             MenuBar;
typedef Gtk_AccelGroup          AccelGroup;
typedef Gtk_ItemFactory_MenuBar ItemFactory_MenuBar;

#endif

typedef SmartPtr<HBox>           PHBox;
typedef SmartPtr<Label>          PLabel;
typedef SmartPtr<Pixmap>         PPixmap;
typedef SmartPtr<AccelGroup>     PAccelGroup;


// Baseclass for X-applications; it creates an application-window with a
// client. The virtual method command is used to handle the commands defined
// with the menus
class XApplication : public Window {
 public:
   // Manager functions
   XApplication (const char* pTitle);
   ~XApplication ();         // No need to be virtual. There´s only 1 instance

   // Events
   gint delete_event_impl (_GdkEventAny*) {
#if (GTKMM_MAJOR_VERSION > 1) || ((GTKMM_MAJOR_VERSION == 1) && GTKMM_MINOR_VERSION > 0)
      Main::quit ();
#else
      Main::instance()->quit ();
#endif
      return 0;
   }

 protected:
   // Menu-handling (including callback for menu-events)
   typedef struct {
      const string path;
      const string accel;
      int          id;
      const string type;
   } MenuEntry;

   Widget* addMenu (const MenuEntry& menuEntry);
   void        addMenus (const MenuEntry menuEntryies[], int cMenus);
   virtual void command (int menu) = 0;

   // Protected data
#if (GTKMM_MAJOR_VERSION > 1) || ((GTKMM_MAJOR_VERSION == 1) && GTKMM_MINOR_VERSION > 0)
   MenuBar* pMenu;
#else
   ItemFactory_MenuBar* pMenu;
#endif
   typedef SmartPtr<VBox>           PVBox;

   PVBox                vboxClient;

 private:
   // Protected manager functions
   XApplication (const XApplication&);
   const XApplication& operator= (const XApplication&);

   PAccelGroup accels;

#if (GTKMM_MAJOR_VERSION > 1) || ((GTKMM_MAJOR_VERSION == 1) && GTKMM_MINOR_VERSION > 0)
   Menu* pLastMenu;
#endif
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
