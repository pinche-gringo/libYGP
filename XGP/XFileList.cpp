//$Id: XFileList.cpp,v 1.24 2003/02/18 02:57:35 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileList
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.24 $
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

#include <gtk--/menu.h>
#include <gtk--/pixmap.h>
#include <gtk--/adjustment.h>

#include <File.h>
#include <Check.h>
#include <Trace_.h>
#include <Process_.h>
#include <PathDirSrch.h>

#include "XFileDlg.h"
#include "XMessageBox.h"

#include "XFileList.h"


const char* XFileList::iconDirectory[] = {
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

const char* XFileList::iconDefault[] = {
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

const char* XFileList::iconExecuteable[] = {
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

Gdk_Pixmap XFileList::iconDir;
Gdk_Pixmap XFileList::iconDef;
Gdk_Pixmap XFileList::iconExe;


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XFileList::~XFileList () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Loads (additional) icons which should be used for the list-entries
//Parameters: path: Path where to search for the icons
//            files: Files to use as icon-files
//            namePrefix: Length of the prefix of the name, which should be removed before comparing with actual filename
//Requires  : namePrefix < strlen (files)
/*--------------------------------------------------------------------------*/
unsigned int XFileList::loadIcons (const char* path, const char* files,
                        	   unsigned int namePrefix) {
   Check3 (path); Check3 (files);
   Check3 (namePrefix < strlen (files));
   TRACE2 ("XFileList::loadIcons (const char*, const char*, unsigned int) - "
           << path << '/' << files);

   Gdk_Color color (&Widget::gtkobj ()->style->bg[GTK_STATE_NORMAL]);
   Gdk_Window parentWin (get_parent_window ());
   static bool first (true);
   if (first) {
      first = false;
      Gdk_Bitmap bitmap;
      iconDir.create_from_xpm_d (parentWin, bitmap, color, iconDirectory);
      iconDef.create_from_xpm_d (parentWin, bitmap, color, iconDefault);
      iconExe.create_from_xpm_d (parentWin, bitmap, color, iconExecuteable);
   } // endif first call to loadIcons: Create default-icons

   // Use Icon.*-files as icon for *-files
   PathDirectorySearch ds (path, files);

   const File* file = ds.find (DirectorySearch::FILE_NORMAL);
   std::string type;
   while (file) {
      // Read icon-file and store it
      string filename (file->path ()); filename += file->name ();
      TRACE5 ("XFileList::loadIcons (const char*, const char*, unsigned int) - Read icon "
              << filename);

      const char* pTypepart (file->name () + namePrefix);
      TRACE9 ("XFileList::loadIcons (const char*, const char*, unsigned int) - Store icon "
              << std::string (pTypepart, strrchr (pTypepart, '.') - pTypepart));
      type.assign (pTypepart,
                   strrchr (pTypepart, '.') - pTypepart);
      icons[type].create_from_xpm (parentWin, color, filename);
      file = ds.next ();
   } // end-while icon-files found
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends an entry to the list
//Parameters: file: File to use to query icon; NULL: Don´t show an icon
//            text: Array of text for the other columns
/*--------------------------------------------------------------------------*/
gint XFileList::append (const File* file, const Gtk::SArray& text) {
   rows ().push_back (text);
   if (file)
      setIcon (rows ().size () - 1, *file);
   return rows ().size () - 1;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Prepends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::prepend (const File* file, const Gtk::SArray& text) {
   rows ().push_front (text);
   if (file)
      setIcon (0, *file);
   return 0;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Inserts an entry into the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            row: Row where to insert the entry (starting with 0)
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::insert (const File* file, gint row, const Gtk::SArray& text) {
   rows ().insert (CList::row (row), text);
   if (file)
      setIcon (row, *file);
   return row;
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
/*--------------------------------------------------------------------------*/
void XFileList::setIcon (int row, const File& file) {
   TRACE7 ("XFileList::setIcon");

   Gdk_Pixmap* actIcon (&iconDef);

   if (file.isDirectory ())
      actIcon = &iconDir;
   else if (file.isExecuteable ())
      actIcon = &iconExe;
   else {
      const char* pName (file.name () - 1);

      std::map<string, Gdk_Pixmap>::iterator i;
      do {
         // Try to find an icon for the file in the preloaded list; 
         // use every dot (.)-seperated part of the filename
         if ((i = icons.find (++pName)) != icons.end ()) {
            TRACE9 ("XFileList::setIcon (int, const File*) - Use icon "
                    << pName << " for file " << file.name ());
            actIcon = &(*i).second;
            break;
         } // endif icons available
      } while ((pName = strchr (pName, '.')) != NULL); 
   } // end-else
   Check3 (actIcon);

   Gdk_Bitmap bmp;
   cell (row, 0).set_pixmap (*actIcon, bmp);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Callback when widget is realized (shown); loads the default icons.
/*--------------------------------------------------------------------------*/
void XFileList::realize_impl () {
   TRACE9 ("XFileList::realize_impl ()");

   CList::realize_impl ();
#ifdef PKGDIR
   loadIcons (PKGDIR, "Icon_*.xpm", sizeof ("Icon_") - 1);
#endif

   event.connect (slot (this, &XFileList::listSelected));
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after clicking in list; if its by button 3: Display menu
//Parameters: event: Datails about the event
//Returns   : true: Event has been handled; false else
/*--------------------------------------------------------------------------*/
gint XFileList::listSelected (GdkEvent* event) {
   TRACE2 ("XDirComp::listSelected (GdkEvent*) - " << event->type);

   if (event->type == GDK_BUTTON_PRESS) {
      GdkEventButton* bev ((GdkEventButton*)(event));
      if (bev->button == 3) {
         if (pMenuPopAction) {
            delete pMenuPopAction;
            pMenuPopAction = NULL;
         }

         TRACE9 ("XFileList::listSelected (GdkEvent*) - Y-offset: " << bev->y);
         TRACE9 ("XFileList::listSelected (GdkEvent*) - Height of line: "
                 << get_row_height ());
         TRACE9 ("XFileList::listSelected (GdkEvent*) - VScroll "
                 << (get_vadjustment () ? get_vadjustment ()->get_value () : -1));
         float scrolled (get_vadjustment ()
                         ? get_vadjustment ()->get_value () : 0);
         unsigned int entry (static_cast<unsigned int>
                             ((bev->y + scrolled)
                              / (get_row_height () + 1)));
         TRACE9 ("XFileList::listSelected (GdkEvent*) - Line " << entry
                 << " = " << get_text (entry, 1));

         if (entry < rows ().size ()) {
            TRACE8 ("XFileList::listSelected (GdkEvent*) - Creating menu");
            pMenuPopAction = new Gtk::Menu;

            // Testing if $EDITOR exists and add that to list; else use VI
            string editor (_("Open in %1 ..."));
            const char* ed;
            if ((ed = getenv ("EDITOR")) == NULL)
               ed = "vi";
            editor.replace (editor.find ("%1"), 2, ed);

            pMenuPopAction->items ().push_back
               (Gtk::Menu_Helpers::MenuElem
                (editor,
                 bind (slot (this, &XFileList::startInTerm),
                       ed, entry)));
            pMenuPopAction->items ().push_back
               (Gtk::Menu_Helpers::MenuElem
                (_("Rename/Move ..."),
                 bind (slot (this, &XFileList::move), entry)));
            pMenuPopAction->items ().push_back
               (Gtk::Menu_Helpers::MenuElem
                (_("Delete"),
                 bind (slot (this, &XFileList::remove), entry)));

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
      string entry (getFilename (line));
      const char* args[] = { term, "-e", file, entry.c_str (), NULL };
      execProgram (term, args, false);
   }
   else
      XMessageBox::Show (_("Environment variable `TERM' not defined!"),
                         XMessageBox::ERROR | XMessageBox::OK);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Starts the passed program in the background with argument as argument
//Parameters: file: File to execute
//            line: Line in list of file to pass as argument
/*--------------------------------------------------------------------------*/
void XFileList::startProgram (const char* file, unsigned int line) {
   string entry (getFilename (line));
   const char* args[] = { file, entry.c_str (), NULL };
   execProgram (args[0], args, false);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Starts the passed program in the foreground with argument as argument
//Parameters: file: File to execute
//            line: Line in list of file to pass as argument
/*--------------------------------------------------------------------------*/
void XFileList::executeProgram (const char* file, unsigned int line) {
   string entry (getFilename (line));
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
   catch (string& error) {
      XMessageBox::Show  (error, XMessageBox::ERROR | XMessageBox::OK);
   }
   return false;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Moves the file in line to another location/name
//Parameters: line: Line in list of file to pass as argument
/*--------------------------------------------------------------------------*/
void XFileList::move (unsigned int line) {
   string file (IFileDialog::perform (string ("Move file to ..."),
                                      IFileDialog::ASK_OVERWRITE)->execModal ());

   if (file.length ()) {
      string entry (getFilename (line));
      const char* args[] = { "mv", "-f", entry.c_str (), file.c_str (), NULL };
      if (execProgram (args[0], args, true)) {
         try {
            File objFile (file.c_str ());
            setIcon (line, objFile);
            setFilename (line, file);
         }
         catch (string& error) {
            XMessageBox::Show  (error, XMessageBox::ERROR | XMessageBox::OK);
         }
      }
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Removes the passed file; both from the system and from the list
//Parameters: line: Line in list of file to pass as argument
/*--------------------------------------------------------------------------*/
void XFileList::remove (unsigned int line) {
   string entry (getFilename (line));
   const char* args[] = { "rm", "-f", entry.c_str (), NULL };
   if (execProgram (args[0], args, true)) {
      rows ().remove (row (line));
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the file name of the passed line; which is considered
//            to be stored in the column 1
//Parameters: line: Line in list to get the filename from
//Returns   : Filename
/*--------------------------------------------------------------------------*/
string XFileList::getFilename (unsigned int line) const {
   return get_text (line, 1);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Retrieves the file name of the passed line; which is considered
//            to be stored in the column 1
//Parameters: line: Line in list to get the filename from
//            file: Filename to set
/*--------------------------------------------------------------------------*/
void XFileList::setFilename (unsigned int line, const string& file) {
   cell (line, 1).set_text (const_cast <string&> (file));
}
