#ifndef XDATE_H
#define XDATE_H

//$Id: XDate.h,v 1.13 2003/10/19 00:03:22 markus Rel $

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


#include <XDialog.h>

#include <SmartPtr.h>


// Forward declarations
class ATimestamp;

namespace Gtk {
   class HBox;
   class Calendar;
   class Adjustment;
   class SpinButton;
}


/**Class to show a dialog to enter a date (year, month, day, hour, minute,
   second or any combination of those).

   Which parts of the date should be displayed/entered is specified by one of:
     - SHOW_YEAR
     - SHOW_MONTH
     - SHOW_DAY
     - SHOW_HOUR
     - SHOW_MINUTE
     - SHOW_SECOND
     - SHOW_ALL

   (which are hopefully self explainatory).
*/
class XDate : public XDialog {
 public:
   /// Constructor
   /// \param title: Title to display for the dialog
   /// \param date: Date/time to preselect in the dialog
   /// \param showFields: Which fields to display; a combination of the \c
   ///     SHOW_* values.
   XDate (const Glib::ustring& title, ATimestamp& date, int showFields = SHOW_ALL);
   ~XDate ();

   /// Possible values to show
   enum { SHOW_HOUR = 1,                    ///< Add a field to change the hour
          SHOW_MINUTE = 2,                ///< Add a field to change the minute
          SHOW_SECOND = 4,                ///< Add a field to change the second
          SHOW_DAY = 8,                             ///< Add a calendar control
          SHOW_MONTH = 16, ///< Enables to change month (and year) in the calender control
          SHOW_YEAR = 32, ///< Enables to change year (and month) in the calender control
          SHOW_ALL = -1            ///< Show all fields the dialog has to offer
   };

   /// Method to display the dialog; caring about freeing it afterwards
   /// \param title: Title to display for the dialog
   /// \param date: Date/time to preselect in the dialog
   /// \param showFields: Which fields to display; a combination of the \c
   ///     SHOW_* values.
   static XDate* create (const Glib::ustring& title, ATimestamp& date,
                         int showFields = SHOW_ALL) {
      XDate* dlg (new XDate (title, date, showFields));
      dlg->signal_response ().connect (slot (*dlg, &XDialog::free));
      return dlg;
   }

 private:
   // Prohibited manager-functions
   XDate (const XDate&);
   const XDate& operator= (const XDate&);

   virtual void okEvent ();

   typedef SmartPtr<Gtk::HBox>        PHBox;
   typedef SmartPtr<Gtk::Calendar>    PCalendar;
   typedef SmartPtr<Gtk::SpinButton>  PSpinButton;
   typedef SmartPtr<Gtk::Adjustment>  PAdjustment;

   PHBox       client;
   PCalendar   cal;
   PAdjustment adjHour;
   PSpinButton spinHour;
   PAdjustment adjMinute;
   PSpinButton spinMinute;
   PAdjustment adjSecond;
   PSpinButton spinSecond;

   ATimestamp& result;
};

#endif
