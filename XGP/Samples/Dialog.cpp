//$Id: Dialog.cpp,v 1.1 2003/02/03 03:47:42 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : X-Windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 01.02.2003
//COPYRIGHT   : Anticopyright (A) 2003

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


#include <gtk--/label.h>
#include <gtk--/table.h>

#include <ANumeric.h>
#include <XFileEntry.h>

#include "Dialog.h"


/*--------------------------------------------------------------------------*/
//Purpose   : (Default-)Constructor
//Parameters: numEntry: Value for the numeric fields
//            file: File read from FileEntry
/*--------------------------------------------------------------------------*/
Dialog::Dialog (ANumeric& numEntry, string& file)
   : XDialog (OKCANCEL), pClient (new Gtk::Table (3, 2))
     , lblLabel (new Gtk::Label ("AttributeLabel"))
     , lblEntry (new Gtk::Label ("AttributeEntry"))
     , lblFileEntry (new Gtk::Label ("FileEntry"))
     , lblNum (new XAttributeLabel<ANumeric> (numEntry))
     , entryNum (new XAttributeEntry<ANumeric> (numEntry))
     , entryFile (new XFileEntry (MAX_PATH))
     , file_ (file) {
   set_title ("Dialog");

   lblLabel->set_justify (GTK_JUSTIFY_LEFT);
   lblEntry->set_justify (GTK_JUSTIFY_LEFT);
   lblFileEntry->set_justify (GTK_JUSTIFY_LEFT);
   lblLabel->show ();
   lblEntry->show ();
   lblFileEntry->show ();

   lblNum->show ();
   entryNum->show ();
   entryFile->show ();

   entryFile->set_text (file);

   pClient->attach (*lblLabel, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 2);
   pClient->attach (*lblNum,   1, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 5, 2);

   pClient->attach (*lblEntry, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 2);
   pClient->attach (*entryNum, 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, GTK_FILL, 5, 2);

   pClient->attach (*lblFileEntry, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 2);
   pClient->attach (*entryFile,    1, 2, 2, 3, GTK_FILL | GTK_EXPAND, GTK_FILL, 5, 2);

   pClient->show ();

   get_vbox ()->pack_start (*pClient, false, false, 5);
   show ();

   entryNum->grab_focus ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
Dialog::~Dialog () {
   delete lblLabel;
   delete lblEntry;
   delete lblFileEntry;
   delete lblNum;
   delete entryNum;
   delete entryFile;
   delete pClient;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Handling of the OK button; closes dialog with commiting data
/*--------------------------------------------------------------------------*/
void Dialog::okEvent () {
   entryNum->commit ();
   file_ = entryFile->get_text ();
   XDialog::okEvent ();
}
