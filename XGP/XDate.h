#ifndef XDATE_H
#define XDATE_H

//$Id: XDate.h,v 1.1 1999/11/13 01:18:07 Markus Rel $

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

class Gtk_HBox;
class Gtk_Button;
class Gtk_Adjustment;
class Gtk_SpinButton;

definePtr (Gtk_HBox);
definePtr (Gtk_Button);
definePtr (Gtk_SpinButton);
definePtr (Gtk_Adjustment);


// Class to enter a date.
class XDate : public Gtk_Dialog {
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

   PGtk_Button ok;
   PGtk_Button cancel;

   PGtk_HBox   client;

   PGtk_Adjustment adjDay;
   PGtk_SpinButton spinDay;
   PGtk_Adjustment adjMonth;
   PGtk_SpinButton spinMonth;
   PGtk_Adjustment adjYear;
   PGtk_SpinButton spinYear;

   PGtk_Adjustment adjHour;
   PGtk_SpinButton spinHour;
   PGtk_Adjustment adjMinute;
   PGtk_SpinButton spinMinute;
   PGtk_Adjustment adjSecond;
   PGtk_SpinButton spinSecond;

   ATimestamp& result;
};

#endif
