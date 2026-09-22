// PROJECT     : MessageDialog
// SUBSYSTEM   : XDate
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 14.9.1999
// COPYRIGHT   : Copyright (C) 2001 - 2006, 2008, 2009, 2011, 2026

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

#include <string>

#include <glibmm/datetime.h>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/calendar.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/spinbutton.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include <YGP/ATStamp.h>

#include <XGP/XDate.h>
#include <XGP/XDialog.h>

namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; adds all controls to the dialog. Only the specified fields
/// are shown.
/// \param title Text to display in the titlebar of the dialog
/// \param date Default date to display; will be overwritten with the input (if the dialog is leaved with OK)
/// \param showFields Bitfield describing wich fields to show
//-----------------------------------------------------------------------------
XDate::XDate(const Glib::ustring& title, YGP::ATimestamp& date, int showFields)
    : XDialog(title, OKCANCEL), client(new Gtk::Box), cal(new Gtk::Calendar()),
      spinHour(new Gtk::SpinButton(Gtk::Adjustment::create(0, 0, 23, 1, 10, 10), 1, 0)),
      spinMinute(new Gtk::SpinButton(Gtk::Adjustment::create(0, 0, 59, 1, 10, 10), 1, 0)),
      spinSecond(new Gtk::SpinButton(Gtk::Adjustment::create(0, 0, 59, 1, 10, 10), 1, 0)), result(date) {
    Check3(client);
    Check3(showFields);
    Check3(cal);
    Check3(spinHour);
    Check3(spinMinute);
    Check3(spinSecond);

    TRACE9("XDate::XDate: Title '" << title << "', startvalue: " << date);

    if (!date.isDefined())
        date = YGP::ATimestamp::now();

    bool first(true);
    if (showFields & (SHOW_DAY | SHOW_MONTH | SHOW_YEAR)) {
        first = false;

        cal->grab_focus();
        cal->set_show_heading();
        cal->set_show_day_names();
        cal->set_show_week_numbers();
        cal->set_margin(5);
        get_content_area()->append(*cal);
        // Note: GTK4's Gtk::Calendar no longer supports locking month/year
        // navigation while still showing the day grid, so unlike before the
        // user can always browse to a different month/year here even if only
        // SHOW_DAY was requested.
    }

    // Create spinbuttons
    Gtk::SpinButton* spins[] = {spinHour.get(), spinMinute.get(), spinSecond.get()};
    for (unsigned int i = 0; i < sizeof(spins) / sizeof(spins[0]); ++i)
        if (showFields & (1 << i)) {
            Check3(spins[i]);

            spins[i]->set_editable(true);
            spins[i]->set_update_policy(Gtk::SpinButton::UpdatePolicy::IF_VALID);
            spins[i]->set_wrap(true);
            spins[i]->set_numeric(true);
            spins[i]->set_hexpand();
            spins[i]->set_margin(5);
            client->append(*spins[i]);

            if (first) { // Set focus to first displayed spin-button
                spins[i]->grab_focus();
                first = false;
            }
        } // endif spinbutton to show

    get_content_area()->append(*client);

    cal->set_date(Glib::DateTime::create_local(date.getYear(), date.getMonth(), date.getDay(), 0, 0, 0));

    spinHour->set_value(date.getHour());
    spinMinute->set_value(date.getMinute());
    spinSecond->set_value(date.getSecond());

    show();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XDate::~XDate() {
    TRACE9("XDate::~XDate");
    hide();
}

//-----------------------------------------------------------------------------
/// Callback after pressing OK
//-----------------------------------------------------------------------------
void XDate::okEvent() {
    TRACE9("XDate::okEvent()");

    YGP::ATimestamp help;

    try {
        help.setHour(spinHour->get_value_as_int());
        help.setMinute(spinMinute->get_value_as_int());
        help.setSecond(spinSecond->get_value_as_int());

        Glib::DateTime dt(cal->get_date());
        TRACE5("XDate::okEvent() - Date: " << dt.get_day_of_month() << '.' << dt.get_month() << '.' << dt.get_year());
        help.setYear(dt.get_year());
        help.setMonth(dt.get_month());
        help.setDay(dt.get_day_of_month());

        result = help;
        TRACE7("XDate::okEvent() - Result = " << result);
    }
    catch (std::invalid_argument& e) {
        Glib::ustring err(_("Date is not valid!\n\nReason: %1"));
        err.replace(err.find("%1"), 2, e.what());
        Gtk::MessageDialog msg(err, false, Gtk::MessageType::ERROR);
        runModal(msg);
    }
}

} // namespace XGP
