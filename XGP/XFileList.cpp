//$Id: XFileList.cpp,v 1.14 2002/04/11 07:48:06 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XFileList
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.14 $
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

#include <string.h>

#include <gtk--/pixmap.h>

#define DEBUG 0
#include "Check.h"
#include "Trace_.h"

#include "PathDirSrch.h"

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
//Purpose   : Pre-loads the icons which should be used for the list-entires
//Parameters: path: Path where to search for the icons
//            files: Files to use as icon-files
//            namePrefix: Prefix of icons-files, which should be removed
//            before comparing with actual filename
//Requires  : namePrefix < strlen (files)
/*--------------------------------------------------------------------------*/
unsigned int XFileList::loadIcons (const char* path, const char* files,
                        	   unsigned int namePrefix) {
   Check3 (path); Check3 (files);
   Check3 (namePrefix < strlen (files));
   TRACE2 ("XFileList::loadIcons -> " << path << '/' << files);

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
      TRACE5 ("XFileList::loadIcons: Read icon " << filename);

      const char* pTypepart (file->name () + namePrefix);
      TRACE9 ("XFileList::loadIcons: Store icon "
              << std::string (pTypepart, strrchr (pTypepart, '.') - pTypepart));
      type.assign (pTypepart,
                   strrchr (pTypepart, '.') - pTypepart);
      icons[type].create_from_xpm (parentWin, color, filename);
      file = ds.next ();
   } // end-while icon-files found
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::append (const File* file, const gchar* text[]) {
   CList::append (text);
   if (file)
      setIcon (rows ().size () - 1, file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Appends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::append (const File* file, const vector<string> text) {
   CList::append (text);
   if (file)
      setIcon (rows ().size () - 1, file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Prepends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::prepend (const File* file, const gchar *text[]) {
   CList::prepend (text);
   if (file)
      setIcon (0, file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Prepends an entry to the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::prepend (const File* file, const vector<string> text) {
   CList::prepend (text);
   if (file)
      setIcon (0, file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Inserts an entry into the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            row: Row where to insert the entry
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::insert_row (const File* file, gint row, const gchar* text[]) {
   CList::insert_row (row, text);
   if (file)
      setIcon (row, file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Inserts an entry into the list
//Parameters: file: File; used to query icon; NULL: Don´t show icon
//            row: Row where to insert the entry
//            text: Text for list
/*--------------------------------------------------------------------------*/
gint XFileList::insert_row (const File* file, gint row, const vector<string> &text) {
   CList::insert_row (row, text);
   if (file)
      setIcon (row, file);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Searches and sets an icon for the passed file
//Parameters: path: Path where to search for the icons
//            files: Files to use as icon-files
//Remarks   : The icon for the file is calculated according the following
//            algorithm:
//               - Use special ones for directories or executeables
//               - Check if the name of file is stored in the icon-map
//               - Remove the first part (til the next dot (.) of the name and
//                 repeat the previous step
//               - If no name-part is left use a special default-icon
/*--------------------------------------------------------------------------*/
void XFileList::setIcon (int row, const File* pFile) {
   TRACE7 ("XFileList::setIcon");
   Check3 (pFile);

   Gdk_Pixmap* actIcon (&iconDef);

   if (pFile->isDirectory ())
      actIcon = &iconDir;
   else if (pFile->isExecuteable ())
      actIcon = &iconExe;
   else {
      const char* pName (pFile->name () - 1);

      std::map<string, Gdk_Pixmap>::iterator i;
      do {
         // Try to find an icon for the file in the preloaded list; 
         // use every dot (.)-seperated part of the filename
         if ((i = icons.find (++pName)) != icons.end ()) {
            TRACE9 ("XFileList::setIcon - Use icon "
                    << pName << " for file " << pFile->name ());
            actIcon = &(*i).second;
            break;
         } // endif icons available
      } while ((pName = strchr (pName, '.')) != NULL); 
   } // end-else
   Check3 (actIcon);

   Gdk_Bitmap bmp;
   set_pixmap (row, 0, *actIcon, bmp);
}


void XFileList::realize_impl () {
   TRACE9 ("XFileList::realize_impl");

   CList::realize_impl ();
#ifdef PKGDIR
   loadIcons (PKGDIR, "Icon_*.xpm", sizeof ("Icon_") - 1);
#endif
}
