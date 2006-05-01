//$Id: XFileList.cpp,v 1.45 2006/05/01 02:24:13 markus Exp $

//PROJECT     : libXGP
//SUBSYSTEM   : XFileList
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.45 $
//AUTHOR      : Markus Schwab
//CREATED     : 17.11.1999
//COPYRIGHT   : Copyright (C) 1999 - 2004, 2006

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


#include <YGP/Internal.h>

#include <string.h>

#include <map>

#include <gtkmm/menu.h>
#include <gtkmm/stock.h>
#include <gtkmm/image.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treestore.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/messagedialog.h>

#include <YGP/File.h>
#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/Process.h>
#include <YGP/PathDirSrch.h>

#include "XGP/XFileDlg.h"

#include "XGP/XFileList.h"


static Glib::RefPtr<Gdk::Pixbuf> iconDir;
static Glib::RefPtr<Gdk::Pixbuf> iconDef;
static Glib::RefPtr<Gdk::Pixbuf> iconExe;

static std::map<std::string, Glib::RefPtr<Gdk::Pixbuf> > icons;


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
   pColumn->pack_start (*manage (rPB), false);
   pColumn->add_attribute (*rPB, "pixbuf", 0);

   Gtk::CellRendererText* rTxt (new Gtk::CellRendererText);
   pColumn->pack_start (*manage (rTxt));
   pColumn->add_attribute (*rTxt, "text", 1);
   append_column (*pColumn);

#ifdef PKGDIR
   loadIcons (PKGDIR, "Icon_*.png", sizeof ("Icon_") - 1);
#endif
}

//-----------------------------------------------------------------------------
/// Loads (additional) icons which should be used for the list-entries
/// \param path: Path where to search for the icons
/// \param files: Files to use as icon-files
/// \param namePrefix: Length of the prefix of the name, which should be removed
///    before comparing with actual filename
/// \pre \c namePrefix < strlen (files)
//-----------------------------------------------------------------------------
void XFileList::loadIcons (const char* path, const char* files, unsigned int namePrefix) {
   Check1 (path); Check1 (files);
   Check1 (namePrefix < strlen (files));
   TRACE2 ("XFileList::loadIcons (const char*, const char*, unsigned int) - " << path << '/' << files);

   if (!iconExe) {
      YGP::PathDirectorySearch ds (path, "Directory.png");
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
	 std::string filename (file->path ()); filename += file->name ();
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
}

//-----------------------------------------------------------------------------
/// Searches for and sets an icon for the passed file The icon for the file is
/// calculated according the following algorithm: - Use special (predifined)
/// ones for directories or executeables. - Use the icon named as the name of
/// file is stored in the icon-map. - Remove the first part (til the next dot
/// (.)) of the name and repeat the previous step. - If no name-part is left
/// use a special default-icon.
/// \param row: Path where to search for the icons
/// \param file: Files to use as icon-files
/// \returns Reference to inserted line
//-----------------------------------------------------------------------------
Glib::RefPtr<Gdk::Pixbuf> XFileList::getIcon4File (const YGP::File& file) {
   TRACE7 ("getIcon4File (Gtk::TreeModel::iterator, const File&)");

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
}

//----------------------------------------------------------------------------
/// Retrieves the file name of the passed line; which is considered to be
/// stored in the column 1
/// \param line: Line in list to get the filename from
/// \returns std::string: Filename
//----------------------------------------------------------------------------
std::string XFileList::getFilename (const Gtk::TreeIter& line) const {
   std::string file;
   Gtk::TreeRow row (*line);
   row.get_value (1, file);
   return file;
}

//-----------------------------------------------------------------------------
/// Sets the file name of the passed line; which is considered to be
/// stored in the column 1
/// \param line: Line in list to get the filename from
/// \param file: Filename to set
//-----------------------------------------------------------------------------
void XFileList::setFilename (Gtk::TreeIter& line, const std::string& file) {
   Gtk::TreeRow row (*line);
   row.set_value (1, file);
}

//-----------------------------------------------------------------------------
/// Callback after clicking in list; if its by button 3: Display menu
/// \param event: Datails about the event
/// \returns \c true: Event has been handled; false else
//-----------------------------------------------------------------------------
bool XFileList::on_event (GdkEvent* event) {
   TRACE2 ("XFileList::on_event (GdkEvent*) - " << event->type);

   if (event->type == GDK_BUTTON_RELEASE) {
      GdkEventButton* bev ((GdkEventButton*)(event));
      if (bev->button == 3) {
         if (pMenuPopAction) {
            delete pMenuPopAction;
            pMenuPopAction = NULL;
	 }

	 Gtk::TreeModel::Path pathAct;
	 Gtk::TreeViewColumn* colFocus;
	 get_cursor (pathAct, colFocus);

         if (!pathAct.empty ()) {
            TRACE8 ("XFileList::on_event (GdkEvent*) - Creating menu");
            pMenuPopAction = new Gtk::Menu;

            // Testing if $EDITOR exists and add that to list; else use VI
            Glib::ustring editor (Glib::locale_to_utf8 (_("Open in %1 ...")));
            const char* ed;
            if ((ed = getenv ("EDITOR")) == NULL)
               ed = "vi";
            editor.replace (editor.find ("%1"), 2, ed);

	    // Get selected row
	    Check3 (get_model ());
	    Gtk::TreeIter iAct (get_model ()->get_iter (pathAct)); Check3 (iAct);

            pMenuPopAction->items ().push_back
               (Gtk::Menu_Helpers::MenuElem
                (editor,
                 bind (mem_fun (*this, &XFileList::startInTerm),
                       ed, iAct)));
            pMenuPopAction->items ().push_back
               (Gtk::Menu_Helpers::MenuElem
                (Glib::locale_to_utf8 (_("Rename/Move ...")),
                 bind (mem_fun (*this, &XFileList::move), iAct)));
            pMenuPopAction->items ().push_back
               (Gtk::Menu_Helpers::MenuElem
                (Glib::locale_to_utf8 (_("Delete")),
                 bind (mem_fun (*this, &XFileList::remove), iAct)));

            addMenus (*pMenuPopAction, iAct);
            pMenuPopAction->popup (bev->button, bev->time);
         }
         return true;
      }
   }
   return false;
}

//-----------------------------------------------------------------------------
/// Starts the passed program in a terminal with argument as argument
/// \param file: File to execute
/// \param line: Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::startInTerm (const char* file, Gtk::TreeIter line) {
   const char* term (getenv ("TERM"));
   if (term) {
      std::string entry (getFilename (line));
      const char* args[] = { term, "-e", file, entry.c_str (), NULL };
      execProgram (term, args, false);
   }
   else {
      Gtk::MessageDialog msg (Glib::locale_to_utf8 (_("Environment variable "
                                                      "`TERM' not defined!")),
                              Gtk::MESSAGE_ERROR);
      msg.run ();
   }
}

//-----------------------------------------------------------------------------
/// Starts the passed program in the background with argument as argument
/// \param file: File to execute
/// \param line: Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::startProgram (const char* file, Gtk::TreeIter line) {
   std::string entry (getFilename (line));
   const char* args[] = { file, entry.c_str (), NULL };
   execProgram (args[0], args, false);
}

//-----------------------------------------------------------------------------
/// Starts the passed program in the foreground with argument as argument
/// \param file: File to execute
/// \param line: Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::executeProgram (const char* file, Gtk::TreeIter line) {
   std::string entry (getFilename (line));
   const char* args[] = { file, entry.c_str (), NULL };
   execProgram (args[0], args, true);
}

//-----------------------------------------------------------------------------
/// Starts the passed program with argument
/// \param file: File to execute
/// \param args: Arguments for the program
/// \param sync: Flag, if file should be executed synchron or not
/// \returns Status; false, if exeuctions failed
//-----------------------------------------------------------------------------
bool XFileList::execProgram (const char* file, const char* const args[], bool sync) {
   try {
      if (sync)
         YGP::Process::execute (file, args);
      else
         YGP::Process::execAsync (file, args);
      return true;
   }
   catch (std::string& err) {
      Gtk::MessageDialog msg (err, Gtk::MESSAGE_ERROR);
      msg.run ();
   }
   return false;
}

//-----------------------------------------------------------------------------
/// Moves the file in line to another location/name
/// \param line: Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::move (Gtk::TreeIter line) {
   std::string file (FileDialog::create (std::string ("Move file to ..."),
					 Gtk::FILE_CHOOSER_ACTION_OPEN,
					 FileDialog::ASK_OVERWRITE)->execModal ());

   if (file.length ()) {
      std::string entry (getFilename (line));
      const char* args[] = { "mv", "-f", entry.c_str (), file.c_str (), NULL };
      if (execProgram (args[0], args, true)) {
         try {
            YGP::File objFile (file.c_str ());
            setFilename (line, file);
         }
         catch (std::string& err) {
            Gtk::MessageDialog msg (err, Gtk::MESSAGE_ERROR);
            msg.run ();
         }
      }
   }
}

//-----------------------------------------------------------------------------
/// Removes the passed file; both from the system and from the list
/// \param line: Line in list of file to pass as argument
//-----------------------------------------------------------------------------
void XFileList::remove (Gtk::TreeIter line) {
   TRACE4 ("XFileList::remove (Gtk::TreeIter) - " << getFilename (line));
   std::string entry (getFilename (line));
   const char* args[] = { "rm", "-f", entry.c_str (), NULL };
   if (execProgram (args[0], args, true)) {
      Glib::RefPtr<Gtk::TreeStore> ptr (Glib::RefPtr<Gtk::TreeStore>::cast_dynamic (get_model ()));
      if (ptr)
	 ptr->erase (line);
      else {
	 Glib::RefPtr<Gtk::ListStore> ptr (Glib::RefPtr<Gtk::ListStore>::cast_dynamic (get_model ()));
	 if (ptr)
	    ptr->erase (line);
      }
   }
}

//-----------------------------------------------------------------------------
/// Adds further menus to the default popup-menu
/// \param menu: Menu where to add some entries to
/// \param line: Line for which to add entries
//-----------------------------------------------------------------------------
void XFileList::addMenus (Gtk::Menu& menu, const Gtk::TreeIter& line) {
}

}
