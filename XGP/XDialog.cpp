//$Id: XDialog.cpp,v 1.25 2008/03/30 13:39:17 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : XDialog
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.25 $
//AUTHOR      : Markus Schwab
//CREATED     : 04.01.2003
//COPYRIGHT   : Copyright (C) 2003, 2004, 2006, 2008, 2011

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


#include <gtkmm/stock.h>
#include <gtkmm/button.h>
#include <gtkmm/accelgroup.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "XGP/XDialog.h"



namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; creates the dialog with the specified buttons
/// \param buttons Bitfield for buttons to display
/// \param modal Flag, if the dialog is modal
//-----------------------------------------------------------------------------
XDialog::XDialog (unsigned int buttons, bool modal)
   : Gtk::Dialog (Glib::ustring (), modal), ok (NULL), cancel (NULL) {
   init (buttons);
}

//-----------------------------------------------------------------------------
/// Constructor; creates the dialog with the specified buttons
/// \param buttons Bitfield for buttons to display
/// \param title Title to display
/// \param modal Flag, if the dialog is modal
//-----------------------------------------------------------------------------
XDialog::XDialog (const Glib::ustring& title, unsigned int buttons, bool modal)
   : Gtk::Dialog (title, modal), ok (NULL), cancel (NULL) {
   init (buttons);
}

//-----------------------------------------------------------------------------
/// (Default-)Constructor; creates the dialog with the specified buttons
/// \param buttons Bitfield for buttons to display
/// \param title Title to display
/// \param parent Parent of the dialog
/// \param modal Flag, if the dialog is modal
//-----------------------------------------------------------------------------
XDialog::XDialog (const Glib::ustring& title, Gtk::Window& parent,
                  unsigned int buttons, bool modal)
   : Gtk::Dialog (title, parent, modal), ok (NULL), cancel (NULL) {
   init (buttons);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XDialog::~XDialog () {
   TRACE9 ("XDialog::~XDialog ()");
   delete ok;
   delete cancel;
}


//-----------------------------------------------------------------------------
/// Adds the specified buttons to the dialog
/// \param buttons Bitfield for buttons to display
//-----------------------------------------------------------------------------
void XDialog::init (unsigned int buttons) {
   TRACE9 ("XDialog::init ()");
   if (buttons & OK) {
      ok = manage (new Gtk::Button (Gtk::Stock::OK));
      ok->signal_clicked ().connect (mem_fun (*this, &XDialog::handleOK));
      get_action_area ()->pack_start (*ok, false, false, 5);
      ok->set_can_default ();
      ok->show ();
   }
   else
      ok = NULL;
   cancel = (buttons & CANCEL) ? add_button ((buttons & OK) ? Gtk::Stock::CANCEL
					     :  Gtk::Stock::CLOSE,
					     Gtk::RESPONSE_CANCEL) : NULL;

   get_action_area ()->set_homogeneous (false);

   if (cancel)
      cancel->grab_default ();
   if (ok) {
      ok->grab_default ();
   }
}

//-----------------------------------------------------------------------------
/// Callback after button-events
/// \param cmd ID of pressed button
//-----------------------------------------------------------------------------
void XDialog::on_response (int cmd) {
   TRACE9 ("XDialog::on_response (int) " << cmd);
   if (cmd == Gtk::RESPONSE_CANCEL)
      cancelEvent ();
   else
      command (cmd);
}

//-----------------------------------------------------------------------------
/// Internal callback after pressing the OK button
//-----------------------------------------------------------------------------
void XDialog::handleOK () {
   TRACE9 ("XDialog::handleOK ()");
   if (isDataOK ()) {
      okEvent ();
      response (Gtk::RESPONSE_OK);
   }
}

//-----------------------------------------------------------------------------
/// Callback after pressing the OK button; the user can override that to implemnt
/// own behaviour
//-----------------------------------------------------------------------------
void XDialog::okEvent () {
   TRACE9 ("XDialog::okEvent ()");
}

//-----------------------------------------------------------------------------
/// Callback after pressing the Cancel button
//-----------------------------------------------------------------------------
void XDialog::cancelEvent () {
   TRACE9 ("XDialog::cancelEvent ()");
}

//-----------------------------------------------------------------------------
/// Checks if the data entered in the dialog is OK
/// \returns bool True, if dialog can be left by selecting OK
//-----------------------------------------------------------------------------
bool XDialog::isDataOK () {
   TRACE9 ("XDialog::isDataOK ()");
   return true;
}

//-----------------------------------------------------------------------------
/// Command handler of the dialog
/// \param action Selected action
/// \remarks Must not be called
//-----------------------------------------------------------------------------
void XDialog::command (int action) {
   TRACE9 ("XDialog::command (int) - " << action);
}

//-----------------------------------------------------------------------------
/// Frees the dialog.
/// \remarks Call only if the dialog was created with new
//-----------------------------------------------------------------------------
void XDialog::free (int) {
   TRACE9 ("XDialog::free (int)");
   delete this;
}

}
