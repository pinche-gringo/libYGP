//$Id: XFileList.cpp,v 1.27 2003/06/02 01:32:39 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileList
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.27 $
//AUTHOR      : Markus Schwab
//CREATED     : 17.11.1999
//COPYRIGHT   : Anticopyright (A) 1999 - 2003

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


#include "Internal.h"

#include <string.h>

#include <map>

#include <gtkmm/menu.h>
#include <gtkmm/image.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/messagedialog.h>

#include <File.h>
#include <Check.h>
#include <Trace_.h>
#include <Process_.h>
#include <PathDirSrch.h>

#include "XFileDlg.h"

#include "XFileList.h"


static const char* iconDirectory[] = {
   "16 16 9 1",
   "       c None",
   ".      c #000000",
   "+      c #585858",
   "@      c #FFA858",
   "#      c #FFDCA8",
   "$      c #FFFFFF",
   "%      c #C0C0C0",
   "&      c #A0A0A4",
   "*      c #808080",
   "  .+.           ",
   " .+@#..         ",
   " .$++@#.....    ",
   " .$$%++@###@.   ",
   " .$%$%%+@###+   ",
   " .%$%%%%++++@.  ",
   " .$%%%%%%%&%.@+ ",
   " .%%%%%%%&%&.@+ ",
   " .%%%%%%&%&&.@+ ",
   " .%%%%%&%&&&.@+ ",
   " ..*%%&%&&&&.@+ ",
   "   ..*%&&&&&.@+ ",
   "     ..&&&&&.@+ ",
   "       ..&&&.@..",
   "         ..&.@..",
   "           .... " };

static const char* iconDefault[] = {
   "16 14 5 1",
   "       c None",
   ".      c #FF0000",
   "+      c #FFFFFF",
   "@      c #000000",
   "#      c #7F7F7F",
   "     ......     ",
   "    .+......    ",
   "   .+..@@...@   ",
   "   ...@  ...@   ",
   "    @@   ...@   ",
   "        ...@    ",
   "       ...@     ",
   "      ..@@      ",
   "      ....@     ",
   "      #.@#      ",
   "      #+..      ",
   "      ....#     ",
   "       ..@#     ",
   "        @#      "} ;

static const char* iconExecuteable[] = {
   "16 16 2 1",
   "       c None",
   ".      c #000000",
   "                ",
   "                ",
   "                ",
   " .....       .  ",
   "  .....     .   ",
   "   .....   .    ",
   "    ..... .     ",
   "     ... .      ",
   "      . ...     ",
   "     . .....    ",
   "    .   .....   ",
   "   .     .....  ",
   "  .       ..... ",
   "                ",
   "                ",
   "                " };

static Glib::RefPtr<Gdk::Pixbuf> iconDir;
static Glib::RefPtr<Gdk::Pixbuf> iconDef;
static Glib::RefPtr<Gdk::Pixbuf> iconExe;

static std::map<std::string, Glib::RefPtr<Gdk::Pixbuf> > icons;


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; loads the default icons to display in the listbox
//Parameters: columns: Columns of the model
/*--------------------------------------------------------------------------*/
IFileStore::IFileStore (const FileColumns& columns) : cols (columns) {
#ifdef PKGDIR
   loadIcons (PKGDIR, "Icon_*.xpm", sizeof ("Icon_") - 1);
#endif
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
IFileStore::~IFileStore () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Loads (additional) icons which should be used for the list-entries
//Parameters: path: Path where to search for the icons
//            files: Files to use as icon-files
//            namePrefix: Length of the prefix of the name, which should be removed before comparing with actual filename
//Requires  : namePrefix < strlen (files)
/*--------------------------------------------------------------------------*/
void IFileStore::loadIcons (const char* path, const char* files,
                            unsigned int namePrefix) {
   Check1 (path); Check1 (files);
   Check1 (namePrefix < strlen (files));
   TRACE2 ("IFileList::loadIcons (const char*, const char*, unsigned int) - "
           << path << '/' << files);

   static bool first (true);
   if (first) {
      first = false;
      iconDir = Gdk::Pixbuf::create_from_xpm_data (iconDirectory);
      iconDef = Gdk::Pixbuf::create_from_xpm_data (iconDefault);
      iconExe = Gdk::Pixbuf::create_from_xpm_data (iconExecuteable);
   } // endif first call to loadIcons: Create default-icons

   // Use Icon.*-files as icon for *-files
   PathDirectorySearch ds (path, files);

   const File* file = ds.find (DirectorySearch::FILE_NORMAL);
   std::string type;
   while (file) {
      // Read icon-file and store it
      std::string filename (file->path ()); filename += file->name ();
      TRACE5 ("IFileList::loadIcons (const char*, const char*, unsigned int) - Read icon "
              << filename);

      const char* pTypepart (file->name () + namePrefix);
      TRACE9 ("IFileList::loadIcons (const char*, const char*, unsigned int) - Store icon "
              << std::string (pTypepart, strrchr (pTypepart, '.') - pTypepart));
      type.assign (pTypepart,
                   strrchr (pTypepart, '.') - pTypepart);
      icons[type] = Gdk::Pixbuf::create_from_file (filename);
      file = ds.next ();
   } // end-while icon-files found
}


/*--------------------------------------------------------------------------*/
//Purpose   : Searches for and sets an icon for the passed file
//
//            The icon for the file is calculated according the following
//            algorithm:
//               - Use special (predifined) ones for directories or executeables.
//               - Use the icon named as the name of file is stored in the icon-map.
//               - Remove the first part (til the next dot (.)) of the name and
//                 repeat the previous step.
//               - If no name-part is left use a special default-icon.
//Parameters: row: Path where to search for the icons
//            files: Files to use as icon-files
//Returns   : Reference to inserted line
/*--------------------------------------------------------------------------*/
Gtk::TreeModel::iterator IFileStore::setIcon (Gtk::TreeModel::iterator row,
                                              const File& file) {
   TRACE7 ("XFileList_setIcon (Gtk::TreeModel::iterator, const File&)");

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

   (*row)[cols.icon] = actIcon;
   return row;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XFileList::~XFileList () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the file name of the passed line; which is considered
//            to be stored in the column 1
//Parameters: line: Line in list to get the filename from
//Returns   : Filename
/*--------------------------------------------------------------------------*/
std::string XFileList::getFilename (unsigned int line) const {
   Check1 (fileModel);
   return fileModel->getFilename (line);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the file name of the passed line; which is considered
//            to be stored in the column 1
//Parameters: line: Line in list to get the filename from
//            file: Filename to set
/*--------------------------------------------------------------------------*/
void XFileList::setFilename (unsigned int line, const std::string& file) {
   Check1 (fileModel);
   fileModel->setFilename (line, file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after clicking in list; if its by button 3: Display menu
//Parameters: event: Datails about the event
//Returns   : true: Event has been handled; false else
/*--------------------------------------------------------------------------*/
bool XFileList::on_event (GdkEvent* event) {
   TRACE2 ("XFileList::on_event (GdkEvent*) - " << event->type);

   if (event->type == GDK_BUTTON_RELEASE) {
      GdkEventButton* bev ((GdkEventButton*)(event));
      if (bev->button == 3) {
         if (pMenuPopAction) {
            delete pMenuPopAction;
            pMenuPopAction = NULL;
         }

         TRACE9 ("XFileList::on_event (GdkEvent*) - Y-offset: " << bev->y);
         TRACE9 ("XFileList::on_event (GdkEvent*) - VScroll "
                 << (get_vadjustment () ? get_vadjustment ()->get_value () : -1));
         float scrolled (get_vadjustment ()
                         ? get_vadjustment ()->get_value () : 0);
         int lineHeight (0);
         int t;
         get_column_cell_renderer (1)->get_size (*this, t, t, t, lineHeight);
         TRACE9 ("XFileList::on_event (GdkEvent*) - Line height: " << lineHeight);
         Check3 (lineHeight);
         unsigned int entry (static_cast<unsigned int>
                             ((bev->y + scrolled)
                              / (lineHeight + 2)));
         TRACE9 ("XFileList::on_event (GdkEvent*) - Line " << entry
                 << " = " << getFilename (entry));

         if (entry < get_model ()->children ().size ()) {
            TRACE8 ("XFileList::on_event (GdkEvent*) - Creating menu");
            pMenuPopAction = new Gtk::Menu;

            // Testing if $EDITOR exists and add that to list; else use VI
            std::string editor (_("Open in %1 ..."));
            const char* ed;
            if ((ed = getenv ("EDITOR")) == NULL)
               ed = "vi";
            editor.replace (editor.find ("%1"), 2, ed);

            pMenuPopAction->items ().push_back
               (Gtk::Menu_Helpers::MenuElem
                (editor,
                 bind (slot (*this, &XFileList::startInTerm),
                       ed, entry)));
            pMenuPopAction->items ().push_back
               (Gtk::Menu_Helpers::MenuElem
                (_("Rename/Move ..."),
                 bind (slot (*this, &XFileList::move), entry)));
            pMenuPopAction->items ().push_back
               (Gtk::Menu_Helpers::MenuElem
                (_("Delete"),
                 bind (slot (*this, &XFileList::remove), entry)));

            addMenus (*pMenuPopAction, entry);
            pMenuPopAction->popup (bev->button, bev->time);
         }
         return true;
      }
   }
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Starts the passed program in a terminal with argument as argument
//Parameters: file: File to execute
//            line: Line in list of file to pass as argument
/*--------------------------------------------------------------------------*/
void XFileList::startInTerm (const char* file, unsigned int line) {
   const char* term (getenv ("TERM"));
   if (term) {
      std::string entry (getFilename (line));
      const char* args[] = { term, "-e", file, entry.c_str (), NULL };
      execProgram (term, args, false);
   }
   else {
      Gtk::MessageDialog msg (_("Environment variable `TERM' not defined!"),
                              Gtk::MESSAGE_ERROR);
      msg.run ();
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Starts the passed program in the background with argument as argument
//Parameters: file: File to execute
//            line: Line in list of file to pass as argument
/*--------------------------------------------------------------------------*/
void XFileList::startProgram (const char* file, unsigned int line) {
   std::string entry (getFilename (line));
   const char* args[] = { file, entry.c_str (), NULL };
   execProgram (args[0], args, false);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Starts the passed program in the foreground with argument as argument
//Parameters: file: File to execute
//            line: Line in list of file to pass as argument
/*--------------------------------------------------------------------------*/
void XFileList::executeProgram (const char* file, unsigned int line) {
   std::string entry (getFilename (line));
   const char* args[] = { file, entry.c_str (), NULL };
   execProgram (args[0], args, true);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Starts the passed program with argument
//Parameters: file: File to execute
//            args: Arguments for the program
//            sync: Flag, if file should be executed synchron or not
//Returns   : Status; false, if exeuctions failed
/*--------------------------------------------------------------------------*/
bool XFileList::execProgram (const char* file, const char* const args[], bool sync) {
   try {
      if (sync)
         Process::execute (file, args);
      else
         Process::execAsync (file, args);
      return true;
   }
   catch (std::string& err) {
      Gtk::MessageDialog msg (err, Gtk::MESSAGE_ERROR);
      msg.run ();
   }
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Moves the file in line to another location/name
//Parameters: line: Line in list of file to pass as argument
/*--------------------------------------------------------------------------*/
void XFileList::move (unsigned int line) {
   std::string file (IFileDialog::perform (std::string ("Move file to ..."),
                                           IFileDialog::ASK_OVERWRITE)->execModal ());

   if (file.length ()) {
      std::string entry (getFilename (line));
      const char* args[] = { "mv", "-f", entry.c_str (), file.c_str (), NULL };
      if (execProgram (args[0], args, true)) {
         try {
            File objFile (file.c_str ());
            setFilename (line, file);
         }
         catch (std::string& err) {
            Gtk::MessageDialog msg (err, Gtk::MESSAGE_ERROR);
            msg.run ();
         }
      }
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Removes the passed file; both from the system and from the list
//Parameters: line: Line in list of file to pass as argument
/*--------------------------------------------------------------------------*/
void XFileList::remove (unsigned int line) {
   std::string entry (getFilename (line));
   const char* args[] = { "rm", "-f", entry.c_str (), NULL };
   if (execProgram (args[0], args, true)) {
      // TODO get_model ()->row_deleted (entry);
   }
}
