#ifndef X_APPL_H
#define X_APPL_H

//$Id: X-Appl.h,v 1.16 2005/01/25 01:17:14 markus Exp $

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

 private:
   // Protected manager functions
   XAppl (const XAppl&);
   const XAppl& operator= (const XAppl&);

   // Event-handling
   void open ();
   void save ();
   void print ();
   void showDialog ();
   void showDateDialog ();
   void showConnectDialog ();
   void showMsgDialog ();
   void showLoginDialog ();
   void showSearchDialog ();

   void addActFile ();
   void addFile (const std::string& file);
   void saveToFile (const std::string& file);
   void writeToStream (std::ostream& file);
   void find (const Glib::ustring& text);
   bool loginEvent (const Glib::ustring& user, const Glib::ustring& password);

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

   static const char* pTitles[];

   static const char* xpmAuthor[];
   static const char* xpmXAppl[];

   enum { SAVE = 0, PRINT, LAST };
   Gtk::Widget* apMenus[LAST];
};

#endif
