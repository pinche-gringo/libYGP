//$Id: XFileList.cpp,v 1.1 1999/11/13 01:18:07 Markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileList
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 17.11.1999
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


#include <gtk--/pixmap.h>

#define DEBUG 0
#include "Check.h"
#include "Trace.h"

#include "PathDirSrch.h"

#include "XFileList.h"
#include "XMessageBox.h"


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

PGdk_Pixmap XFileList::iconDir (new Gdk_Pixmap);
PGdk_Pixmap XFileList::iconDef (new Gdk_Pixmap);
PGdk_Pixmap XFileList::iconExe (new Gdk_Pixmap);


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XFileList::~XFileList () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Pre-loads the icons which should be used for the list-entires
//Parameters: path: Path where to search for the icons
//            files: Files to use as icon-files
//Remarks   : Call *after* the window (and the parent) is shown!!
/*--------------------------------------------------------------------------*/
unsigned int XFileList::loadIcons (const char* path, const char* files) {
   Check3 (iconDir); Check3 (iconDef); Check3 (iconExe);

   if (!iconDir->connected ()) {
      Gdk_Bitmap bitmap;
      Gdk_Color color (&get_style ()->gtkobj ()->bg[GTK_STATE_NORMAL]);

      // Preload icons for fast(er) access when displayed
      iconDir->create_from_xpm_d (get_window (), bitmap, color, iconDirectory);
      iconDef->create_from_xpm_d (get_window (), bitmap, color, iconDefault);
      iconExe->create_from_xpm_d (get_window (), bitmap, color, iconExecuteable);
      Check9 (iconDir->connected ());
      Check9 (iconDef->connected ());
      Check9 (iconExe->connected ());
   } // endif do first initialize

   Check3 (path); Check3 (files);

   // Use Icon.*-files as icon for *-files
   PathDirectorySearch ds (path, files);
   dirEntry file;

   Gdk_Bitmap bitmap;
   Gdk_Color color (&get_style ()->gtkobj ()->bg[GTK_STATE_NORMAL]);
   Gdk_Pixmap* temp;

   int rc (ds.find (file, FILE_NORMAL));
   while (!rc) {
      // Read icon-file and store it
      temp = new Gdk_Pixmap (NULL); Check3 (temp);
      string filename (file.path ()); filename += file.name ();
      TRACE9 ("XFileList::setIcons: Read icon " << file.path () << file.name ());

      temp->create_from_xpm (get_window (), bitmap, color, filename);

      if (temp->connected ()) {
         TRACE9 ("XCompDirs::XCompDirs: Store icon "
                 << (filename.c_str () + sizeof ("Icon")));
          icons[file.name () + sizeof ("Icon")] = temp;
      }
      else
         XMessageBox::show (string ("Error loading file '") + filename
                            + "'\nMaybe open or not an xpm-file",
                            XMessageBox::WARNING);
      rc = ds.find ();
   } // end-while icon-files found
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::append (const dirEntry* file, const gchar* text[]) {
   Gtk_CList::append (text);
   if (file)
      setIcon (file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::append (const dirEntry* file, const vector<string> text) {
   Gtk_CList::append (text);
   if (file)
      setIcon (file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Prepends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::prepend (const dirEntry* file, const gchar *text[]) {
   Gtk_CList::prepend (text);
   if (file)
      setIcon (file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Prepends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::prepend (const dirEntry* file, const vector<string> text) {
   Gtk_CList::prepend (text);
   if (file)
      setIcon (file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Inserts an entry into the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            row: Row where to insert the entry
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::insert_row (const dirEntry* file, gint row, const gchar* text[]) {
   Gtk_CList::insert_row (row, text);
   if (file)
      setIcon (file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Inserts an entry into the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            row: Row where to insert the entry
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::insert_row (const dirEntry* file, gint row, const vector<string> &text) {
   Gtk_CList::insert_row (row, text);
   if (file)
      setIcon (file);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Searches for 
//Parameters: path: Path where to search for the icons
//            files: Files to use as icon-files
/*--------------------------------------------------------------------------*/
void XFileList::setIcon (const dirEntry* pFile) {
   Check3 (pFile);

   Gdk_Pixmap* actIcon (NULL);

   if (pFile->isDirectory ())
      actIcon = iconDir;
   else if (pFile->isExecuteable ())
      actIcon = iconExe;
   else {
      actIcon = iconDef;

      const char* pName (pFile->name () - 1);

      std::map<string, Gdk_Pixmap*>::iterator i;
      do {
         // Try to find an icon for the file in the preloaded list; 
         // use every dot (.)-seperated part of the filename
         if ((i = icons.find (++pName)) != icons.end ()) {
            TRACE5 ("XPDir::command (show) - Use icon Icon."
                    << pName << " for file " << pFile->name ());
            actIcon = (*i).second;
            break;
         } // endif icons available
      } while ((pName = strchr (pName, '.')) != NULL); 
   } // end-else
   Check3 (actIcon);

   Gdk_Bitmap bmp;
   set_pixmap (rows () - 1, 0, *actIcon, bmp);

}
