#ifndef X_APPL_H
#define X_APPL_H

//$Id: X-Appl.h,v 1.11 2004/09/07 05:58:15 markus Rel $

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


#include <string>
#include <iosfwd>

#include <gtkmm/statusbar.h>
#include <gtkmm/scrolledwindow.h>

#include <YGP/ATStamp.h>
#include <YGP/ANumeric.h>

#include <XGP/XFileList.h>
#include <XGP/XApplication.h>


// Class with the graphical interface to PDir
class XAppl : public XGP::XApplication {
 public:
   // Manager functions
   XAppl ();
   ~XAppl () { }

 protected:
   enum { OPEN = LAST, EXIT, SAVE, PRINT, DIALOG, DATE, CONNECT, MSGDLG };

 private:
   // Protected manager functions
   XAppl (const XAppl&);
   const XAppl& operator= (const XAppl&);

   // Event-handling
   virtual void command (int menu);

   void addActFile ();
   void addFile (const std::string& file);
   void saveToFile (const std::string& file);
   void writeToStream (std::ostream& file);

   virtual const char* getHelpfile () { return "index.html"; }
   virtual void showAboutbox ();

   class FileCols : public XGP::FileColumns {
    public:
      Gtk::TreeModelColumn <int>         size;
      Gtk::TreeModelColumn <std::string> date;

      FileCols () : XGP::FileColumns () { add (size); add (date); }
   };

   FileCols                           cols;
   Glib::RefPtr <XGP::XFileListStore> files;
   XGP::XFileList                     listFiles;
   
   Gtk::Statusbar      status;
   Gtk::ScrolledWindow scroll;

   YGP::ATimestamp  time;
   std::string file;
   YGP::ANumeric    num;

   static XGP::XApplication::MenuEntry XAppl::menuItems[];
   static const char* pTitles[];

   static const char* xpmAuthor[];
   static const char* xpmXAppl[];
};

#endif
