//$Id: XAbout.cpp,v 1.12 2003/01/04 08:12:42 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XAbout
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.12 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.9.1999
//COPYRIGHT   : Anticopyright (A) 1999, 2000, 2001, 2002

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

#include "Trace_.h"
#include "Check.h"
#include "Internal.h"

#include "XAbout.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; adds all controls to the dialog and shows it.
//Parameters: author: Author, copyright, ... of the program displayed in the client
//            program: Name and version of the program; displaced in the title
/*--------------------------------------------------------------------------*/
XAbout::XAbout (const string& author, const string& program)
   : XDialog (CANCEL), writer (new Label (author)), client (new HBox ())
     , gpl (new Label (_("Distributed under the terms of the GNU General"
                         " Public License"))) {
   Check3 (gpl); Check3 (writer); Check3 (client);

   TRACE9 ("XAbout::XAbout - Show: " << author);
   TRACE9 ("XAbout::XAbout - Title: " << program);

   set_title (program);

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
//Purpose   : Sets the icon for the program (in xpm-format)
//Parameters: picProgram: Pixmap representing program
/*--------------------------------------------------------------------------*/
void XAbout::setIconProgram (const char* const* pIconData) {
   Check3 (client);

   pIconProgramm = new Pixmap (pIconData);

   pIconProgramm->show ();
   client->pack_start (*pIconProgramm, false, false, 5);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Sets the icon for the programmer (in xpm-format)
//Parameters: pIconData: Pointer to xpm-data for pixmap representing the programmer
/*--------------------------------------------------------------------------*/
void XAbout::setIconAuthor (const char* const* pIconData) {
   Check3 (client); Check3 (vboxPrgInfo);
   
   pIconAuthor = new Pixmap (pIconData);

   pIconAuthor->show ();
   client->pack_end (*pIconAuthor, false, false, 5);

   Check3 (writer);
   client->reorder_child (*writer, 3);
}
