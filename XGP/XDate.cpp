//$Id: XDate.cpp,v 1.10 2003/01/04 08:12:42 markus Exp $

//PROJECT     : XGeneral
//SUBSYSTEM   : XAbout
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.10 $
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


#include <string>

#include <gtk--/box.h>
#include <gtk--/spinbutton.h> 
#include <gtk--/adjustment.h>

#include <Check.h>
#include <Trace_.h>
#include <Internal.h>

#include "ATStamp.h"
#include "XMessageBox.h"

#include "XDate.h"


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; adds all controls to the dialog. Only the specified
//            fields are shown.
//Parameters: title: Text to display in the titlebar of the dialog
//            date: Default date to display; will be overwritten with the input (if the dialog is leaved with OK)
//            showFields: Bitfield describing wich fields to show
/*--------------------------------------------------------------------------*/
XDate::XDate (const string& title, ATimestamp& date, int showFields)
   : XDialog (OKCANCEL)
   , client (new HBox)
   , adjDay (new Adjustment (1, 1, 31, 1, 10, 10))
   , spinDay (new SpinButton (*adjDay, 1, 0))
   , adjMonth (new Adjustment (1, 1, 12, 1, 4, 4))
   , spinMonth (new SpinButton (*adjMonth, 1, 0))
   , adjYear (new Adjustment (1900, 1900, 9999, 1, 10, 10))
   , spinYear (new SpinButton (*adjYear, 1, 0))
   , adjHour (new Adjustment (0, 0, 23, 1, 10, 10))
   , spinHour (new SpinButton (*adjHour, 1, 0))
   , adjMinute (new Adjustment (0, 0, 59, 1,10, 10))
   , spinMinute (new SpinButton (*adjMinute, 1, 0))
   , adjSecond (new Adjustment (0, 0, 59, 1, 10, 10))
   , spinSecond (new SpinButton (*adjSecond, 1, 0))
, result (date) {

   Check3 (client); Check3 (spinDay);
   Check3 (spinMonth); Check3 (spinYear); Check3 (adjDay); Check3 (adjMonth);
   Check3 (adjYear); Check3 (spinHour); Check3 (adjHour); Check3 (spinMinute);
   Check3 (adjMinute); Check3 (spinSecond); Check3 (adjSecond);
   Check3 (showFields);

   TRACE9 ("XDate::XDate: Title '" << title << "', startvalue: " << date);

   set_title (title);

   // Create spinbuttons
   SpinButton* spins[] = { spinDay, spinMonth, spinYear, spinHour,
                           spinMinute, spinSecond };
   bool first (true);
   for (int i = 0; i < sizeof (spins) / sizeof (spins[0]); ++i)
      if (showFields & (1 << i)) {
         Check3 (spins[i]);

         spins[i]->show ();
         spins[i]->set_editable (true);
         spins[i]->set_update_policy (GTK_UPDATE_IF_VALID);
         spins[i]->set_wrap (true);
         spins[i]->set_numeric (true);
         client->pack_start (*spins[i], true, false, 5);

         if (first) {              // Set focus to first displayed spin-button
            spins[i]->grab_focus ();
            first = false;
         }
      } // endif spinbutton to show

   spinYear->set_usize (60, -1);

   client->show ();
   get_vbox ()->pack_start (*client, true, false, 5);

   if (!date.isDefined ())
      date = ATimestamp::now ();

   spinDay->set_value (date.getDay ());
   spinMonth->set_value (date.getMonth ());
   spinYear->set_value (date.getYear ());
   spinHour->set_value (date.getHour ());
   spinMinute->set_value (date.getMinute ());
   spinSecond->set_value (date.getSecond ());

   show ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XDate::~XDate () {
   TRACE9 ("XDate::~XDate");
   hide ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after pressing OK
/*--------------------------------------------------------------------------*/
void XDate::okEvent () {
   TRACE9 ("XDate::okEvent ()");

   ATimestamp help;

   help.setHour (spinHour->get_value_as_int ());
   help.setMinute (spinMinute->get_value_as_int ());
   help.setSecond (spinSecond->get_value_as_int ());

   help.setYear (spinYear->get_value_as_int ());
   help.setMonth (spinMonth->get_value_as_int ());
   help.setDay (spinDay->get_value_as_int ());

   if (help.checkIntegrity ()) {
      std::string error (_("Date `%1' is not valid!"));
      error.replace (error.find ("%1"), 2, help.toString ());
      XMessageBox::Show (error, XMessageBox::CANCEL | XMessageBox::ERROR);
      return;
   }
   else {
      result = help;;
      TRACE7 ("XDate::okEvent () - Result = " << result);
   }
   delete this;
}
