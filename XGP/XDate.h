#ifndef XDATE_H
#define XDATE_H

//$Id: XDate.h,v 1.9 2003/07/05 05:12:12 markus Rel $

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


// Class to show a dialog to enter a date (year, month, day, hour, minute,
// second or any combination of those).
//
// Which parts of the date should be displayed/entered is specified by one of:
//   - SHOW_YEAR
//   - SHOW_MONTH
//   - SHOW_DAY
//   - SHOW_HOUR
//   - SHOW_MINUTE
//   - SHOW_SECOND
//   - SHOW_ALL
// (which are hopefully self explainatory).
class XDate : public XDialog {
 public:
   XDate (const Glib::ustring& title, ATimestamp& date, int showFields = SHOW_ALL);
   ~XDate ();

   enum { SHOW_HOUR = 1, SHOW_MINUTE = 2, SHOW_SECOND = 4, SHOW_DAY = 8,
          SHOW_MONTH = 16, SHOW_YEAR = 32, SHOW_ALL = -1 };

   static XDate* perform (const Glib::ustring& title, ATimestamp& date,
                          int showFields = SHOW_ALL) {
      return new XDate (title, date, showFields); }

   typedef SmartPtr<Gtk::HBox>        PHBox;
   typedef SmartPtr<Gtk::Calendar>    PCalendar;
   typedef SmartPtr<Gtk::SpinButton>  PSpinButton;
   typedef SmartPtr<Gtk::Adjustment>  PAdjustment;

 private:
   // Prohibited manager-functions
   XDate (const XDate&);
   const XDate& operator= (const XDate&);

   virtual void okEvent ();

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
