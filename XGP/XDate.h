#ifndef XDATE_H
#define XDATE_H

//$Id: XDate.h,v 1.2 2000/02/24 22:16:36 Markus Exp $

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


#ifndef __STRING__
class string;
#endif

#include <gtk--/dialog.h>

#include <SmartPtr.h>


// Forward declarations
class ATimestamp;

#if GTKMM_MAJOR_VERSION >= 1 && GTKMM_MINOR_VERSION > 0
namespace Gtk {
   class HBox;
   class Button;
   class Adjustment;
   class SpinButton;
}
using namespace Gtk;

#else
typedef Gtk_HBox              HBox;
typedef Gtk_Button            Button;
typedef Gtk_Dialog            Dialog;
typedef Gtk_Adjustment        Adjustment;
typedef Gtk_SpinButton        SpinButton;
#endif

typedef SmartPtr<HBox>        PHBox;
typedef SmartPtr<Button>      PButton;
typedef SmartPtr<SpinButton>  PSpinButton;
typedef SmartPtr<Adjustment>  PAdjustment;


// Class to enter a date.
class XDate : public Dialog {
 public:
   XDate (const string& title, ATimestamp& date,
          int showFields = SHOW_ALL);
   ~XDate ();

   enum { SHOW_DATE = 1, SHOW_MONTH = 2, SHOW_YEAR = 4, SHOW_HOUR = 8,
          SHOW_MINUTE = 16, SHOW_SECOND = 32, SHOW_ALL = -1 };

   static XDate* perform (const string& title, ATimestamp& date) {
      return new XDate (title, date); }

 private:
   // Prohibited manager-functions
   XDate (const XDate&);
   const XDate& operator= (const XDate&);

   typedef enum { OK, CANCEL } commands;

   void command (commands);

   PButton ok;
   PButton cancel;

   PHBox   client;

   PAdjustment adjDay;
   PSpinButton spinDay;
   PAdjustment adjMonth;
   PSpinButton spinMonth;
   PAdjustment adjYear;
   PSpinButton spinYear;

   PAdjustment adjHour;
   PSpinButton spinHour;
   PAdjustment adjMinute;
   PSpinButton spinMinute;
   PAdjustment adjSecond;
   PSpinButton spinSecond;

   ATimestamp& result;
};

#endif
