#ifndef X_APPL_H
#define X_APPL_H

//$Id: X-Appl.h,v 1.1 2003/02/01 19:44:05 markus Exp $

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


#include <gtk--/table.h>
#include <gtk--/statusbar.h>
#include <gtk--/scrolledwindow.h>

#include <XFileList.h>
#include <XApplication.h>


// Class with the graphical interface to PDir
class XAppl : public XApplication {
 public:
   // Manager functions
   XAppl ();
   ~XAppl () { }

 protected:
   enum { OPEN = LAST, EXIT, SAVE, PRINT };

 private:
   // Protected manager functions
   XAppl (const XAppl&);
   const XAppl& operator= (const XAppl&);
  
   // Event-handling
   virtual void command (int menu);

   void saveToFile (string& file);
   void writeToStream (ofstream& file);

   void showAboutbox ();

   Table     tblInput;
   XFileList listFiles;
   Statusbar status;
   ScrolledWindow scroll;

   static XApplication::MenuEntry XAppl::menuItems[];
   static const char* pTitles[];

   static const char* xpmAuthor[];
   static const char* xpmXAppl[];
};

#endif
