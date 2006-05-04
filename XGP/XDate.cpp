//$Id: XDate.cpp,v 1.25 2006/05/04 01:26:29 markus Rel $

//PROJECT     : MessageDialog
//SUBSYSTEM   : XDate
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.25 $
//AUTHOR      : Markus Schwab
//CREATED     : 14.9.1999
//COPYRIGHT   : Copyright (C) 2001 - 2006

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

#include <gtkmm/box.h>
#include <gtkmm/calendar.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/messagedialog.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include <YGP/ATStamp.h>

#include <XGP/XDate.h>


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; adds all controls to the dialog. Only the specified fields
/// are shown.
/// \param title: Text to display in the titlebar of the dialog
/// \param date: Default date to display; will be overwritten with the input (if the dialog is leaved with OK)
/// \param showFields: Bitfield describing wich fields to show
//-----------------------------------------------------------------------------
XDate::XDate (const Glib::ustring& title, YGP::ATimestamp& date, int showFields)
   : XDialog (title, OKCANCEL)
     , client (new Gtk::HBox)
     , cal (new Gtk::Calendar ())
     , adjHour (new Gtk::Adjustment (0, 0, 23, 1, 10, 10))
     , spinHour (new Gtk::SpinButton (*adjHour, 1, 0))
     , adjMinute (new Gtk::Adjustment (0, 0, 59, 1,10, 10))
     , spinMinute (new Gtk::SpinButton (*adjMinute, 1, 0))
     , adjSecond (new Gtk::Adjustment (0, 0, 59, 1, 10, 10))
     , spinSecond (new Gtk::SpinButton (*adjSecond, 1, 0))
     , result (date) {
   Check3 (client); Check3 (showFields);
   Check3 (cal); Check3 (spinHour); Check3 (adjHour); Check3 (spinMinute);
   Check3 (adjMinute); Check3 (spinSecond); Check3 (adjSecond);

   TRACE9 ("XDate::XDate: Title '" << title << "', startvalue: " << date);

   if (!date.isDefined ())
      date = YGP::ATimestamp::now ();

   bool first (true);
   if (showFields & (SHOW_DAY | SHOW_MONTH | SHOW_YEAR)) {
      first = false;

      cal->grab_focus ();
      cal->display_options (Gtk::CALENDAR_SHOW_HEADING
                            | Gtk::CALENDAR_SHOW_DAY_NAMES
                            | Gtk::CALENDAR_SHOW_WEEK_NUMBERS);
      cal->show ();
      get_vbox ()->pack_start (*cal, false, false, 5);
      if (!(showFields & (SHOW_MONTH | SHOW_YEAR)))
         cal->display_options (Gtk::CALENDAR_NO_MONTH_CHANGE);
   }

   // Create spinbuttons
   Gtk::SpinButton* spins[] = { spinHour, spinMinute, spinSecond };
   for (unsigned int i = 0; i < sizeof (spins) / sizeof (spins[0]); ++i)
      if (showFields & (1 << i)) {
         Check3 (spins[i]);

         spins[i]->show ();
         spins[i]->set_editable (true);
         spins[i]->set_update_policy (Gtk::UPDATE_IF_VALID);
         spins[i]->set_wrap (true);
         spins[i]->set_numeric (true);
         client->pack_start (*spins[i], true, false, 5);

         if (first) {              // Set focus to first displayed spin-button
            spins[i]->grab_focus ();
            first = false;
         }
      } // endif spinbutton to show

   client->show ();
   get_vbox ()->pack_start (*client, true, false, 5);

   cal->select_day (date.getDay ());
   cal->select_month (date.getMonth () - 1, date.getYear ());

   spinHour->set_value (date.getHour ());
   spinMinute->set_value (date.getMinute ());
   spinSecond->set_value (date.getSecond ());

   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XDate::~XDate () {
   TRACE9 ("XDate::~XDate");
   hide ();
}

//-----------------------------------------------------------------------------
/// Callback after pressing OK
//-----------------------------------------------------------------------------
void XDate::okEvent () {
   TRACE9 ("XDate::okEvent ()");

   YGP::ATimestamp help;

   try {
      help.setHour (spinHour->get_value_as_int ());
      help.setMinute (spinMinute->get_value_as_int ());
      help.setSecond (spinSecond->get_value_as_int ());

      guint day, month, year;
      cal->get_date (year, month, day);
      TRACE5 ("XDate::okEvent () - Date: " << day << '.' << month << '.' << year);
      help.setYear (year);
      help.setMonth (month + 1);
      help.setDay (day);

      result = help;
      TRACE7 ("XDate::okEvent () - Result = " << result);
      delete this;
   }
   catch (std::invalid_argument& e) {
      Glib::ustring err (_("Date is not valid!\n\nReason: %1"));
      err.replace (err.find ("%1"), 2, e.what ());
      Gtk::MessageDialog msg (err, Gtk::MESSAGE_ERROR);
      msg.run ();
   }
}

}
