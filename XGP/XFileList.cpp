//PROJECT     : libXGP
//SUBSYSTEM   : XFileList
//REFERENCES  :
//TODO        :
//BUGS        :
//AUTHOR      : Markus Schwab
//CREATED     : 17.11.1999
//COPYRIGHT   : Copyright (C) 1999 - 2004, 2006, 2008 - 2011, 2026

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include <string.h>

#include <map>

#ifdef HAVE_GIOMM
#  include <giomm/file.h>
#  include <giomm/fileicon.h>
#  include <giomm/themedicon.h>
#endif

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdkmm/display.h>

#include <gtkmm/image.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/popovermenu.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treestore.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/messagedialog.h>

#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>

#include "XGP/XDialog.h"

#define CHECK 9
#define TRACELEVEL 9
#include <YGP/File.h>
#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/Process.h>
#include <YGP/PathDirSrch.h>

#include "XGP/XFileDlg.h"

#include "XGP/XFileList.h"


static Glib::RefPtr<Gdk::Pixbuf> iconDef;
#ifndef HAVE_GIOMM
static Glib::RefPtr<Gdk::Pixbuf> iconDir;
static Glib::RefPtr<Gdk::Pixbuf> iconExe;

static std::map<std::string, Glib::RefPtr<Gdk::Pixbuf> > icons;
#endif


namespace XGP {

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XFileList::~XFileList () {
}


//-----------------------------------------------------------------------------
/// Creates the first column for the list holding an icon for the file and its name
//-----------------------------------------------------------------------------
void XFileList::init () {
   Gtk::TreeView::Column* pColumn (new Gtk::TreeView::Column (_("File")));

   Gtk::CellRendererPixbuf* rPB (new Gtk::CellRendererPixbuf);
   pColumn->pack_start (*rPB, false);
   pColumn->add_attribute (*rPB, "pixbuf", 0);

   Gtk::CellRendererText* rTxt (new Gtk::CellRendererText);
   pColumn->pack_start (*rTxt);
   pColumn->add_attribute (*rTxt, "text", 1);
   append_column (*pColumn);

#if defined (PKGDIR) && !defined (HAVE_GIOMM)
   loadIcons (PKGDIR, "Icon_*.png", sizeof ("Icon_") - 1);
#endif

   Glib::RefPtr<Gtk::GestureClick> rightClick (Gtk::GestureClick::create ());
   rightClick->set_button (GDK_BUTTON_SECONDARY);
   rightClick->signal_pressed ().connect (sigc::mem_fun (*this, &XFileList::onRightClick));
   add_controller (rightClick);
}

//-----------------------------------------------------------------------------
/// Loads (additional) icons which should be used for the list-entries
/// \param path Path where to search for the icons
/// \param files Files to use as icon-files
/// \param namePrefix Length of the prefix of the name, which should be removed
///    before comparing with actual filename
/// \pre \c namePrefix < strlen (files)
//-----------------------------------------------------------------------------
void XFileList::loadIcons (const char* path, const char* files, unsigned int namePrefix) {
   Check1 (path); Check1 (files);
   Check1 (namePrefix < strlen (files));
   TRACE2 ("XFileList::loadIcons (const char*, const char*, unsigned int) - " << path << '/' << files);

#ifndef HAVE_GIOMM
   if (!iconExe) {
      std::string p (path);
      YGP::PathDirectorySearch ds (p, "Directory.png");
      const YGP::File* file (ds.find (YGP::IDirectorySearch::FILE_NORMAL)); Check2 (file);
      std::string filename (file->path ()); filename += file->name ();
      iconDir = Gdk::Pixbuf::create_from_file (filename); Check1 (iconDir);

      file = ds.find ("Default.png"); Check2 (file);
      filename = file->path (); filename += file->name ();
      iconDef = Gdk::Pixbuf::create_from_file (filename);

      file = ds.find ("Executable.png"); Check2 (file);
      filename = file->path (); filename += file->name ();
      iconExe = Gdk::Pixbuf::create_from_file (filename);

      // Use Icon.*-files as icon for *-files
      ds.find (path, files);

      std::string type;
      while (file) {
	 // Read icon-file and store it
         filename = file->path();
         filename += file->name ();
	 TRACE5 ("XFileList::loadIcons (const char*, const char*, unsigned int) - Read icon " << filename);

	 const char* pTypepart (file->name () + namePrefix);
	 TRACE9 ("XFileList::loadIcons (const char*, const char*, unsigned int) - Store icon "
		 << std::string (pTypepart, strrchr (pTypepart, '.') - pTypepart));
	 type.assign (pTypepart,
		      strrchr (pTypepart, '.') - pTypepart);
	 icons[type] = Gdk::Pixbuf::create_from_file (filename);
	 file = ds.next ();
      } // end-while icon-files found
   } // endif first call to loadIcons: Create default-icons
#endif
}

//-----------------------------------------------------------------------------
/// Searches for and sets an icon for the passed file The icon for the file is
/// calculated according the following algorithm: - Use special (predifined)
/// ones for directories or executeables. - Use the icon named as the name of
/// file is stored in the icon-map. - Remove the first part (til the next dot
/// (.)) of the name and repeat the previous step. - If no name-part is left
/// use a special default-icon.
/// \param file Files to use as icon-files
/// \returns Glib::RefPtr<Gdk::Pixbuf> Reference to inserted line
//-----------------------------------------------------------------------------
Glib::RefPtr<Gdk::Pixbuf> XFileList::getIcon4File (const YGP::File& file) {
   TRACE7 ("getIcon4File (Gtk::TreeModel::iterator, const File&)" << file.path () << file.name ());

#ifdef HAVE_GIOMM
   Glib::RefPtr<Gio::File> gFile (Gio::File::create_for_path (std::string (file.path ()) + file.name ()));
   Glib::RefPtr<Gio::FileInfo> gInfo (gFile->query_info (G_FILE_ATTRIBUTE_STANDARD_ICON));

   Glib::RefPtr<const Gio::Icon> gIcon (gInfo->get_icon ());
   Glib::RefPtr<const Gio::FileIcon> fIcon (std::dynamic_pointer_cast<const Gio::FileIcon> (gIcon));
   if (fIcon)
      return Gdk::Pixbuf::create_from_file (fIcon->get_file ()->get_path ());
   else {
      Glib::RefPtr<const Gio::ThemedIcon> tIcon (std::dynamic_pointer_cast<const Gio::ThemedIcon> (gIcon));
      if (tIcon) {
	 // icon = tIcon->get_names ();            // TODO: Fix when implemented
	 const gchar* const* names (g_themed_icon_get_names (const_cast<GThemedIcon*> (tIcon->gobj ())));
	 if (names) {
	    Glib::RefPtr<Gtk::IconTheme> theme
	       (Gtk::IconTheme::get_for_display (Gdk::Display::get_default ()));
	    while (*names) {
	       try {
		  Glib::RefPtr<Gtk::IconPaintable> icon (theme->lookup_icon (*names, 16));
		  Glib::RefPtr<Gio::File> iconFile (icon ? icon->get_file () : Glib::RefPtr<Gio::File> ());
		  if (iconFile)
		     return Gdk::Pixbuf::create_from_file (iconFile->get_path ());
	       }
	       catch (...) { }
	       ++names;
	    }
	 }
      }
   }
   return Glib::RefPtr<Gdk::Pixbuf> ();
#else
   Glib::RefPtr<Gdk::Pixbuf> actIcon (iconDef);

   if (file.isDirectory ())
      actIcon = iconDir;
   else if (file.isExecuteable ())
      actIcon = iconExe;
   else {
      const char* pName (file.name () - 1);

      std::map<std::string, Glib::RefPtr<Gdk::Pixbuf> >::iterator i;
      do {
         // Try to find an icon for the file in the preloaded list;
         // use every dot (.)-seperated part of the filename
         if ((i = icons.find (++pName)) != icons.end ()) {
            TRACE9 ("XFileList_setIcon (Gtk::TreeModel::iterator, const File&)"
                    " - Use icon " << pName << " for file " << file.name ());
            actIcon = (*i).second;
            break;
         } // endif icons available
      } while ((pName = strchr (pName, '.')) != NULL);
   } // end-else

   Check3 (actIcon);
   return actIcon;
#endif
}

//----------------------------------------------------------------------------
/// Retrieves the file name of the passed line; which is considered to be
/// stored in the column 1
/// \param line Line in list to get the filename from
/// \returns std::string Filename
//----------------------------------------------------------------------------
std::string XFileList::getFilename (const Gtk::TreeModel::iterator& line) const {
   std::string file;
   Gtk::TreeRow row (*line);
   row.get_value (1, file);
   return file;
}

//-----------------------------------------------------------------------------
/// Sets the file name of the passed line; which is considered to be
/// stored in the column 1
/// \param line Line in list to get the filename from
/// \param file Filename to set
//-----------------------------------------------------------------------------
void XFileList::setFilename (Gtk::TreeModel::iterator& line, const std::string& file) {
   Gtk::TreeRow row (*line);
   row.set_value (1, file);
}

//-----------------------------------------------------------------------------
/// Callback after right-clicking in the list: Display the popup menu
/// \param x X-coordinate (widget-relative) of the click
/// \param y Y-coordinate (widget-relative) of the click
//-----------------------------------------------------------------------------
void XFileList::onRightClick (int, double x, double y) {
   TRACE2 ("XFileList::onRightClick (int, double, double) - " << x << '/' << y);

   if (pMenuPopAction) {
      pMenuPopAction->unparent ();
      delete pMenuPopAction;
      pMenuPopAction = NULL;
   }

   Gtk::TreeModel::Path pathAct;
   if (get_path_at_pos (static_cast<int> (x), static_cast<int> (y), pathAct)
       && !pathAct.empty ()) {
      TRACE8 ("XFileList::onRightClick (int, double, double) - Creating menu");

      // Get selected row
      Check3 (get_model ());
      Gtk::TreeModel::iterator iAct (get_model ()->get_iter (pathAct)); Check3 (iAct);

      Glib::RefPtr<Gio::SimpleActionGroup> actions (Gio::SimpleActionGroup::create ());
      Glib::RefPtr<Gio::Menu> menu (Gio::Menu::create ());

      // Testing if $EDITOR exists and add that to list; else use VI
      Glib::ustring editor (_("Open in %1 ..."));
      const char* ed;
      if ((ed = getenv ("EDITOR")) == NULL)
	 ed = "vi";
      editor.replace (editor.find ("%1"), 2, ed);

      actions->add_action
	 ("open", sigc::bind (sigc::mem_fun (*this, &XFileList::startInTerm), ed, iAct));
      menu->append (editor, "popup.open");

      actions->add_action
	 ("move", sigc::bind (sigc::mem_fun (*this, &XFileList::move), iAct));
      menu->append (_("Rename/Move ..."), "popup.move");

      actions->add_action
	 ("delete", sigc::bind (sigc::mem_fun (*this, &XFileList::remove), iAct));
      menu->append (_("Delete"), "popup.delete");

      addMenus (menu, actions, iAct);

      insert_action_group ("popup", actions);

      pMenuPopAction = new Gtk::PopoverMenu (menu);
      pMenuPopAction->set_parent (*this);
      pMenuPopAction->set_has_arrow (false);
      Gdk::Rectangle rect (static_cast<int> (x), static_cast<int> (y), 1, 1);
      pMenuPopAction->set_pointing_to (rect);
      pMenuPopAction->popup ();
   }
}

//-----------------------------------------------------------------------------
/// Starts the passed program in a terminal with argument as argument
/// \param file File to execute
/// \param line Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::startInTerm (const char* file, Gtk::TreeModel::iterator line) {
   const char* term (getenv ("TERM"));
   if (term) {
      std::string entry (getFilename (line));
      const char* args[] = { term, "-e", file, entry.c_str (), NULL };
      execProgram (term, args, false);
   }
   else {
      Gtk::MessageDialog msg (_("Environment variable `TERM' not defined!"),
                              false, Gtk::MessageType::ERROR);
      runModal (msg);
   }
}

//-----------------------------------------------------------------------------
/// Starts the passed program in the background with argument as argument
/// \param file File to execute
/// \param line Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::startProgram (const char* file, Gtk::TreeModel::iterator line) {
   std::string entry (getFilename (line));
   const char* args[] = { file, entry.c_str (), NULL };
   execProgram (args[0], args, false);
}

//-----------------------------------------------------------------------------
/// Starts the passed program in the foreground with argument as argument
/// \param file File to execute
/// \param line Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::executeProgram (const char* file, Gtk::TreeModel::iterator line) {
   std::string entry (getFilename (line));
   const char* args[] = { file, entry.c_str (), NULL };
   execProgram (args[0], args, true);
}

//-----------------------------------------------------------------------------
/// Starts the passed program with argument
/// \param file File to execute
/// \param args Arguments for the program
/// \param sync Flag, if file should be executed synchron or not
/// \returns bool Status; false, if exeuctions failed
//-----------------------------------------------------------------------------
bool XFileList::execProgram (const char* file, const char* const args[], bool sync) {
   try {
      if (sync)
         YGP::Process::execute (file, args);
      else
         YGP::Process::execAsync (file, args);
      return true;
   }
   catch (YGP::ExecError& err) {
      Gtk::MessageDialog msg (err.what (), false, Gtk::MessageType::ERROR);
      runModal (msg);
   }
   return false;
}

//-----------------------------------------------------------------------------
/// Moves the file in line to another location/name
/// \param line Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::move (Gtk::TreeModel::iterator line) {
   std::string file (FileDialog::create (std::string ("Move file to ..."),
					 Gtk::FileChooser::Action::OPEN,
					 FileDialog::ASK_OVERWRITE)->execModal ());

   if (file.length ()) {
      std::string entry (getFilename (line));
      const char* args[] = { "mv", "-f", entry.c_str (), file.c_str (), NULL };
      if (execProgram (args[0], args, true)) {
         try {
            YGP::File objFile (file.c_str ());
            setFilename (line, file);
         }
         catch (YGP::FileError& err) {
            Gtk::MessageDialog msg (err.what (), false, Gtk::MessageType::ERROR);
            runModal (msg);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/// Removes the passed file; both from the system and from the list
/// \param line Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::remove(Gtk::TreeModel::iterator line) {
   TRACE4("XFileList::remove (Gtk::TreeModel::iterator) - " << getFilename(line));
   std::string entry (getFilename(line));
   const char* args[] = { "rm", "-f", entry.c_str(), NULL };
   if (execProgram(args[0], args, true)) {
      Glib::RefPtr<Gtk::TreeStore> ptr (std::dynamic_pointer_cast<Gtk::TreeStore> (get_model()));
      if (ptr)
	 ptr->erase (line);
      else {
	 Glib::RefPtr<Gtk::ListStore> lptr (std::dynamic_pointer_cast<Gtk::ListStore> (get_model()));
	 if (lptr)
	    lptr->erase(line);
      }
   }
}

//-----------------------------------------------------------------------------
/// Adds further menus to the default popup-menu
/// \param Menu where to add some entries to
/// \param Action group backing the menu, to add the entries' actions to
/// \param Line for which to add entries
//-----------------------------------------------------------------------------
void XFileList::addMenus (const Glib::RefPtr<Gio::Menu>&,
			  const Glib::RefPtr<Gio::SimpleActionGroup>&, const Gtk::TreeModel::iterator&) {
}

}
