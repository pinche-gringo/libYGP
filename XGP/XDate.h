#ifndef XGP_XDATE_H
#define XGP_XDATE_H

//$Id: XDate.h,v 1.18 2008/05/18 13:21:27 markus Rel $

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include <boost/scoped_ptr.hpp>

#include <XGP/XDialog.h>


// Forward declarations
namespace YGP {
   class ATimestamp;
}
namespace Gtk {
   class HBox;
   class Calendar;
   class Adjustment;
   class SpinButton;
}


namespace XGP {

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
   /// \param title Title to display for the dialog
   /// \param date Date/time to preselect in the dialog
   /// \param showFields Which fields to display; a combination of the \c
   ///     SHOW_* values.
   XDate (const Glib::ustring& title, YGP::ATimestamp& date, int showFields = SHOW_ALL);
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
   /// \param title Title to display for the dialog
   /// \param date Date/time to preselect in the dialog
   /// \param showFields Which fields to display; a combination of the \c
   ///     SHOW_* values.
   static XDate* create (const Glib::ustring& title, YGP::ATimestamp& date,
                         int showFields = SHOW_ALL) {
      XDate* dlg (new XDate (title, date, showFields));
      dlg->signal_response ().connect (mem_fun (*dlg, &XDate::free));
      return dlg;
   }

 private:
   // Prohibited manager-functions
   XDate (const XDate&);
   const XDate& operator= (const XDate&);

   virtual void okEvent ();

   typedef boost::scoped_ptr<Gtk::HBox>        PHBox;
   typedef boost::scoped_ptr<Gtk::Calendar>    PCalendar;
   typedef boost::scoped_ptr<Gtk::SpinButton>  PSpinButton;
   typedef boost::scoped_ptr<Gtk::Adjustment>  PAdjustment;

   PHBox       client;
   PCalendar   cal;
   PAdjustment adjHour;
   PSpinButton spinHour;
   PAdjustment adjMinute;
   PSpinButton spinMinute;
   PAdjustment adjSecond;
   PSpinButton spinSecond;

   YGP::ATimestamp& result;
};

}

#endif
