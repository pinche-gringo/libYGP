#ifndef XAPPLICATION_H
#define XAPPLICATION_H

//$Id: XApplication.h,v 1.2 2000/01/23 23:06:37 Markus Rel $

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
class Gtk_VBox;
class Gtk_HBox;
class Gtk_Label;
class Gdk_Pixmap;
class Gtk_Pixmap;
class Gtk_AccelGroup;
#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
class Gtk_MenuBar;
#else
class Gtk_ItemFactory_MenuBar;
#endif

definePtr (Gtk_HBox);
definePtr (Gtk_VBox);
definePtr (Gtk_Label);
definePtr (Gtk_Pixmap);
definePtr (Gtk_AccelGroup);


// Baseclass for X-applications; it creates an application-window with a
// client. The virtual method command is used to handle the commands defined
// with the menus
class XApplication : public Gtk_Window {
 public:
   // Manager functions
   XApplication (const char* pTitle);
   ~XApplication () { }      // No need to be virtual. There´s only 1 instance

   // Events
   gint delete_event_impl (_GdkEventAny*) {
#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
      Gtk_Main::instance()->quit ();
#else
      Gtk_Main::instance()->quit ();
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

   Gtk_Widget* addMenu (const MenuEntry& menuEntry);
   void        addMenus (const MenuEntry menuEntryies[], int cMenus);
   virtual void command (int menu) = 0;

   // Protected data
#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
   Gtk_MenuBar* pMenu;
#else
   Gtk_ItemFactory_MenuBar* pMenu;
#endif
   PGtk_VBox                vboxClient;

 private:
   // Protected manager functions
   XApplication (const XApplication&);
   const XApplication& operator= (const XApplication&);

   PGtk_AccelGroup accels;

#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
   Gtk_Menu* pLastMenu;
#endif
};


// Baseclass for X-applications; it creates an application-window with a
// client. The client contains the program-info on top (2 icons for program
// and programmer, 2 lines for program-info and author)
class XInfoApplication : public XApplication {
 public:
   XInfoApplication (const char* pTitle, const char* pPrgInfo, const char* pCopyright);
   ~XInfoApplication () { }  // No need to be virtual. There´s only 1 instance

 protected:
   // Add information
   void setIconProgram (Gdk_Pixmap& picProgram);
   void setIconAuthor (Gdk_Pixmap& picAuthor);

 private:
   // Protected manager functions
   XInfoApplication (const XInfoApplication&);
   const XInfoApplication& operator= (const XInfoApplication&);

   PGtk_HBox  hboxTitle;
   PGtk_VBox  vboxPrgInfo;
   PGtk_Label txtProgramm;
   PGtk_Label txtCopyright;

   PGtk_Pixmap iconPrg;
   PGtk_Pixmap iconAuthor;
};

#endif
