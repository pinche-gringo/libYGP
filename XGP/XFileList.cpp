//$Id: XFileList.cpp,v 1.4 2000/01/26 22:26:38 Markus Rel $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileList
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
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

PGtk_Pixmap XFileList::iconDir (NULL);
PGtk_Pixmap XFileList::iconDef (NULL);
PGtk_Pixmap XFileList::iconExe (NULL);


/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XFileList::~XFileList () {
   // Delete all loaded icons
   std::map<string, Gtk_Pixmap*>::iterator i;
   for (i = icons.begin (); i != icons.end (); ++i)
      delete (*i).second;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Pre-loads the icons which should be used for the list-entires
//Parameters: path: Path where to search for the icons
//            files: Files to use as icon-files
//Remarks   : Call *after* the window (and the parent) is shown!!
/*--------------------------------------------------------------------------*/
unsigned int XFileList::loadIcons (const char* path, const char* files) {
   Check3 (path); Check3 (files);
   TRACE2 ("XFileList::loadIcons -> " << path << '/' << files);

   if (iconDir == NULL) {
      TRACE5 ("XFileList::loadIcons -> Create default icons");
#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
      iconDir = new Gtk_Pixmap (iconDirectory);
      iconDef = new Gtk_Pixmap (iconDefault);
      iconExe = new Gtk_Pixmap (iconExecuteable);
#else
      iconDir = new Gtk_Pixmap (*this, iconDirectory);
      iconDef = new Gtk_Pixmap (*this, iconDefault);
      iconExe = new Gtk_Pixmap (*this, iconExecuteable);
#endif
      iconDir->realize ();
      iconDef->realize ();
      iconExe->realize ();
   } // endif do first initialize

   Check3 (iconDir); Check3 (iconDef); Check3 (iconExe);

   // Use Icon.*-files as icon for *-files
   PathDirectorySearch ds (path, files);
   dirEntry file;

   Gtk_Pixmap* temp;

   int rc (ds.find (file, DirectorySearch::FILE_NORMAL));
   while (!rc) {
      // Read icon-file and store it
      string filename (file.path ()); filename += file.name ();
      TRACE5 ("XFileList::loadIcons: Read icon " << file.path () << file.name ());

#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
      temp = new Gtk_Pixmap (filename);
#else
      temp = new Gtk_Pixmap (*this, filename);
#endif
      temp->realize ();
      Check3 (temp);
      TRACE9 ("XCompDirs::loadIcons: Store icon "
	      << (file.name () + sizeof ("Icon")));
      icons[file.name () + sizeof ("Icon")] = temp;
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
      setIcon (rows () - 1, file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::append (const dirEntry* file, const vector<string> text) {
   Gtk_CList::append (text);
   if (file)
      setIcon (rows () - 1, file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Prepends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::prepend (const dirEntry* file, const gchar *text[]) {
   Gtk_CList::prepend (text);
   if (file)
      setIcon (0, file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Prepends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::prepend (const dirEntry* file, const vector<string> text) {
   Gtk_CList::prepend (text);
   if (file)
      setIcon (0, file);
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
      setIcon (row, file);
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
      setIcon (row, file);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Searches and sets an icon for the passed file
//Parameters: path: Path where to search for the icons
//            files: Files to use as icon-files
//Remarks   : The icon for the file is calculated according the following
//            algorithm:
//               - Use special one for directories or executeables
//               - Check if the name of file is stored in the icon-map
//               - Remove the first part (til the next dot (.) of the name and
//                 repeat the previous step
//               - If no name-part left use a special default-icon
/*--------------------------------------------------------------------------*/
void XFileList::setIcon (int row, const dirEntry* pFile) {
   TRACE7 ("XFileList::setIcon");
   Check3 (pFile);

   Gtk_Pixmap* actIcon (NULL);

   if (pFile->isDirectory ())
      actIcon = iconDir;
   else if (pFile->isExecuteable ())
      actIcon = iconExe;
   else {
      actIcon = iconDef;

      const char* pName (pFile->name () - 1);

      std::map<string, Gtk_Pixmap*>::iterator i;
      do {
         // Try to find an icon for the file in the preloaded list; 
         // use every dot (.)-seperated part of the filename
         if ((i = icons.find (++pName)) != icons.end ()) {
            TRACE9 ("XFileList::setIcon - Use icon Icon."
                    << pName << " for file " << pFile->name ());
            actIcon = (*i).second;
            break;
         } // endif icons available
      } while ((pName = strchr (pName, '.')) != NULL); 
   } // end-else
   Check3 (actIcon);

   Gdk_Bitmap bmp;
   Gdk_Pixmap pixmap;
   actIcon->get (pixmap, bmp);
   set_pixmap (row, 0, pixmap, bmp);
}


void XFileList::realize_impl () {
   TRACE9 ("XFileList::realize_impl");

   Gtk_CList::realize_impl ();
#ifdef PKGDIR
   loadIcons (PKGDIR, "Icon.*");
#endif
}
