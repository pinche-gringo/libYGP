#ifndef XAPPLICATION_H
#define XAPPLICATION_H

//$Id: XApplication.h,v 1.25 2003/11/14 20:28:08 markus Rel $

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

#include <gtkmm/main.h>
#include <gtkmm/menu.h>
#include <gtkmm/window.h>

#include <YGP/SmartPtr.h>


// Forward declarations
namespace Gtk {
   class Main;
   class HBox;
   class VBox;
   class Label;
   class Image;
   class MenuBar;
   class AccelGroup;
}

namespace XGP {

/**Baseclass for X-applications; it creates an application-window with a
   client. The virtual method command() is used to handle the commands defined
   with the menus.

   Furthermore exist addMenus() - and addMenu () - to create the
   menu-structure. Note that separators and items are added to the last branch
   (so you better start with one of the latter ;). The menus can be accessed
   via the apMenus map.

   Menus to add are specified by objects of type XApplication::MenuEntry which
   consists of the following entries:

     - \b name: Name of the menu as it should be displayed.
     - \b accel: Accelerator in a format understood by GTK+ (like \<alt\>a).
     - \b id: Identifier for the menu. Menus causing actions must have an
           unique ID. Entries having an ID other than 0 are stored in the
           protected member \c apMenus for later usage.
     - \b type: Type of the menu; the following values are supported:
           - \c ITEM: Ordinary menu entry, usually causing an action.
           - \c CHECKITEM: Menu entry having one of two states.
           - \c RADIOITEM: Menu entries which allows the selection of only one
                           in a group.
           - \c LASTRADIOITEM: As an \c RADIOITEM, but terminating the group.
           - \c SEPARATOR: A separating line in between any of the above items.
           - \c BRANCH: A new entry in the menu bar, usually causing no action.
           - \c LASTBRANCH: As \c BRANCH but causing the entry to be aligned to
                            the right of the menu bar.
           - \c SUBMENU: Causes the creation of a new menu inside the previous
                         menu.
           - \c SUBMENUEND: As \c SUBMENU but terminates the last submenu.

   \note Due to limitations in the gtkmm toolkit (or my stupidity) you can \b
     not add RadioMenuItems with the addMenu() method; but must use the
     addMenus() method (as gtkmm needs a unique group for them, which must be
     allocated on the stack).

   Furthermore there exists the posibility to add a help-menu consisting of:

      - An about box (to be implemented by the showAboutbox() method).
      - A content menu (accessible by pressing F1), which executes a browser
        (default: galeon) with a file determinted by the
        getHelpfile() method). The help must therefore be in HTML-format.
      - A menu to configure the browser to use.

   To get the last two entries, the getHelpfile() method must not return \c
   NULL.

   The following algorithm is used to determine the help file to display:

      -# If the file does not start with a single slash (<tt>/</tt>) and does
         not specify the file-protocoll (starting with <tt>%file://</tt>), just
         pass the filename to the browser.
      -# Else assume a local file to display and check the language
         settings: If the environment variable \c LANGUAGE is not empty,
         take that value, else the locale settings (\c LC_MESSAGES).
      -# Take every (colon-separated) entry from the value of step 2
         and append it to the name of the help-file.
      -# If step 3 does not specify a valid file name, cut the
         language specifier at the last underscore (_). (E.g. change
         \c de_AT to \c de).
      -# Repeat step 4 until either a file is found or the languge
         specifier is empty. If this still does not succeed, procceed
         with step 3.
      -# Nothing worked; search for the file "as is".
      -# As last ressort append a ".en" to the filename.
*/
class XApplication : public Gtk::Window {
 public:
   // Manager functions
   XApplication (const char* pTitle);
   ~XApplication ();         // No need to be virtual. There's only 1 instance

   static void initI18n ();
   static void initI18n (const char* package, const char* dir);

 protected:
   /// Types of menu entries
   typedef enum { ITEM,                                 ///< Ordinary menu item
                  CHECKITEM,                 ///< Check menu (can be on or off)
                  RADIOITEM,     ///< Radio item; Only one of a group can be on
                  LASTRADIOITEM,              ///< Last radio item in the group
                  SEPARATOR,               ///< Separator line in between menus
                  BRANCH,                                   ///< New menu entry
                  SUBMENU,                              ///< New menu in a menu
                  SUBMENUEND,  ///< Ends a submenu (inside a menu); does not add no menu item
                  LASTBRANCH  ///< Last menu entry; this entry is right aligned
   } menuTypes;

   enum { ABOUT = 1, CONTENT, CONFIGUREBROWSER, LAST };

   /// Structure describing the menu items.
   typedef struct {
      const Glib::ustring name;           ///< Displayed name of the menu item
      const std::string accel;  ///< Keyboard shortcut to access the menu (or '\\0')
      unsigned int id;           ///< ID of the menu (can be 0, if it does not cause a callback).
       menuTypes    type;    ///< Type of the menu (separator, radio item, ...)
   } MenuEntry;

   typedef YGP::SmartPtr<Gtk::VBox>    PVBox;     ///< Smart pointer for a vbox
   typedef YGP::SmartPtr<Gtk::MenuBar> PMenuBar;    ///< Smart pointer for menu

   Gtk::Widget& addMenu (const MenuEntry& menuEntry);
   void         addMenus (const MenuEntry menuEntryies[], int cMenus);
   virtual void command (int menu);

   virtual void setIconProgram (const char* const* iconData);

   /// Returns the client window
   Gtk::VBox* getClient () const { return vboxClient; }

   // Protected data
   PMenuBar pMenu;                             ///< Menu bar of the application
   PVBox    vboxClient;                   ///< Client window of the application

   /// Map holding the menus having and ID != 0; to enable manipulation of
   /// them.
   std::map <unsigned int, Widget*> apMenus;

   //Help-menu
   void showHelpMenu ();

   /// \name Help menu handling
   //@{
   /// Returns the name of the help file to display (NULL: none)
   virtual const char* getHelpfile () { return NULL; }
   /// Displays the about box. See also XAbout for a dialog implementing one.
   virtual void showAboutbox () { }
   //@}

   Glib::ustring helpBrowser;       ///< Name of the browser to show the help.

 private:
   // Protected manager functions
   XApplication (const XApplication&);
   const XApplication& operator= (const XApplication&);

   std::vector<Gtk::Menu*> aLastMenus;
};


/**Baseclass for X-applications showing the program information inside the
   client.

   The top of the client contains 2 icons for program and programmer and two
   lines for program information and about the author.
*/
class XInfoApplication : public XApplication {
 public:
   XInfoApplication (const char* pTitle, const char* pPrgInfo, const char* pCopyright);
   ~XInfoApplication ();     // No need to be virtual. There´s only 1 instance

 protected:
   // Add information
   virtual void setIconProgram (const char* const* iconData);
   void setIconAuthor (const char* const* iconData);

 private:
   // Protected manager functions
   XInfoApplication (const XInfoApplication&);
   const XInfoApplication& operator= (const XInfoApplication&);

   typedef YGP::SmartPtr<Gtk::HBox>  PHBox;
   typedef YGP::SmartPtr<Gtk::Label> PLabel;
   typedef YGP::SmartPtr<Gtk::Image> PImage;

   PHBox  hboxTitle;
   PVBox  vboxPrgInfo;
   PLabel txtProgramm;
   PLabel txtCopyright;

   PImage iconPrg;
   PImage iconAuthor;
};

}

#endif
