//$Id: XDialog.cpp,v 1.14 2003/11/14 20:28:08 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : X-windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.14 $
//AUTHOR      : Markus Schwab
//CREATED     : 04.01.2003
//COPYRIGHT   : Anticopyright (A) 2003

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


#include <YGP/Internal.h>

#define XK_MISCELLANY
#include <X11/keysymdef.h>

#include <gtkmm/stock.h>
#include <gtkmm/button.h>
#include <gtkmm/accelgroup.h>

#include <YGP/Check.h>
#include <YGP/Trace_.h>

#include "XGP/XDialog.h"



namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor; creates the dialog with the specified buttons
/// \param buttons: Bitfield for buttons to display
/// \param modal: Flag, if the dialog is modal
/// \param use_separator: Flag, if the dialog should display a separator line
///     between its contents and its buttons
//-----------------------------------------------------------------------------
XDialog::XDialog (unsigned int buttons, bool modal, bool use_separator)
    : Gtk::Dialog ("", modal, use_separator) {
   init (buttons);
}

//-----------------------------------------------------------------------------
/// Constructor; creates the dialog with the specified buttons
/// \param buttons: Bitfield for buttons to display
/// \param title: Title to display
/// \param modal: Flag, if the dialog is modal
/// \param use_separator: Flag, if the dialog should display a separator line
///     between its contents and its buttons
//-----------------------------------------------------------------------------
XDialog::XDialog (const Glib::ustring& title, unsigned int buttons,
                  bool modal, bool use_separator)
    : Gtk::Dialog (title, modal, use_separator) {
   init (buttons);
}

//-----------------------------------------------------------------------------
/// (Default-)Constructor; creates the dialog with the specified buttons
/// \param buttons: Bitfield for buttons to display
/// \param title: Title to display
/// \param parent: Parent of the dialog
/// \param modal: Flag, if the dialog is modal
/// \param use_separator: Flag, if the dialog should display a separator line
///     between its contents and its buttons
//-----------------------------------------------------------------------------
XDialog::XDialog (const Glib::ustring& title, Gtk::Window& parent,
                  unsigned int buttons, bool modal, bool use_separator)
    : Gtk::Dialog (title, parent, modal, use_separator) {
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
/// \param buttons: Bitfield for buttons to display
//-----------------------------------------------------------------------------
void XDialog::init (unsigned int buttons) {
   TRACE9 ("XDialog::init ()");
   ok = (buttons & OK) ? add_button (Gtk::Stock::OK,
                                     Gtk::RESPONSE_OK) : NULL;
   cancel = (buttons & CANCEL) ? add_button (Gtk::Stock::CANCEL,
                                     Gtk::RESPONSE_CANCEL) : NULL;

   get_action_area ()->set_homogeneous (false);

   if (cancel) {
      cancel->set_flags (Gtk::CAN_DEFAULT);
      cancel->grab_default ();
      Check3 (get_accel_group ());
      cancel->add_accelerator ("clicked", get_accel_group (), XK_Escape,
                               Gdk::ModifierType (0), Gtk::ACCEL_VISIBLE);
   }
   if (ok) {
      ok->set_flags (Gtk::CAN_DEFAULT);
      ok->grab_default ();
   }
}

//-----------------------------------------------------------------------------
/// Callback after button-events
/// \param cmd: ID of pressed button
//-----------------------------------------------------------------------------
void XDialog::on_response (int cmd) {
   TRACE9 ("XDialog::on_response (int) " << cmd);
   switch (cmd) {
   case Gtk::RESPONSE_OK:
      okEvent ();
      break;

   case Gtk::RESPONSE_CANCEL:
      cancelEvent ();
      break;

   default:
      command (cmd);
   }
}

//-----------------------------------------------------------------------------
/// Callback after pressing the OK button
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
/// Command handler of the dialog
/// \param action: Selected action
/// \remarks Must not be called
//-----------------------------------------------------------------------------
void XDialog::command (int action) {
   Check (0);
}

//-----------------------------------------------------------------------------
/// Frees the dialog.
/// \remarks Call only if the dialog was created with new
//-----------------------------------------------------------------------------
void XDialog::free (int) {
   delete this;
}

}
