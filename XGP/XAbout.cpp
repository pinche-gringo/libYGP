//$Id: XAbout.cpp,v 1.1 1999/11/13 01:18:07 Markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XAbout
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.9.1999
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


#include <gtk--/box.h>
#include <gtk--/label.h>
#include <gtk--/button.h>
#include <gtk--/pixmap.h>

#define DEBUG 0
#include "Trace.h"
#include "Check.h"

#include "XAbout.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; adds all controls to the dialog
//Parameters: author: Author of the programm
//            program: Description of the program-version
/*--------------------------------------------------------------------------*/
XAbout::XAbout (const string& author, const string& program)
   : Gtk_Dialog (), writer (new Gtk_Label (author)), client (new Gtk_HBox ())
   , gpl (new Gtk_Label ("Distributed under the terms of the GNU General Public License"))
   , ok (new Gtk_Button ("OK")) {
   Check3 (gpl); Check3 (writer); Check3 (client);

   TRACE9 ("XAbout::XAbout - Show: " << text);
   TRACE9 ("XAbout::XAbout - Title: " << program);

   set_title (program);

   ok->set_usize (90, 30);
   ok->show ();
   connect_to_method (ok->clicked, this, &command, 0);
   get_action_area ()->pack_start (*ok, false, false, 5);
   ok->set_flags (GTK_CAN_DEFAULT);
   ok->grab_default ();

   writer->show ();
   client->pack_end (*writer, true, false, 5);         // Put text into client
   client->show ();
   get_vbox ()->pack_start (*client, true, false, 5);

   gpl->show ();
   get_vbox ()->pack_start (*gpl, true, true, 5);

   show ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XAbout::~XAbout () {
   TRACE9 ("XAbout::~XAbout");
   hide ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets pixmap for the program
//Parameters: picProgram: Pixmap for program
/*--------------------------------------------------------------------------*/
void XAbout::setIconProgram (Gdk_Pixmap& picProgram) {
   Check3 (client);

   iconPrg = new Gtk_Pixmap (picProgram, Gdk_Bitmap ()); Check3 (iconPrg);

   iconPrg->show ();
   client->pack_start (*iconPrg, false, false, 5);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets pixmap for the programmer
//Parameters: picAuthor: Pixmap for programmer
/*--------------------------------------------------------------------------*/
void XAbout::setIconAuthor (Gdk_Pixmap& picAuthor) {
   Check3 (client); Check3 (vboxPrgInfo);

   iconAuthor = new Gtk_Pixmap (picAuthor, Gdk_Bitmap ()); Check3 (iconAuthor);

   iconAuthor->show ();
   client->pack_end (*iconAuthor, false, false, 5);
   client->reorder_child (*writer, 3);
}
