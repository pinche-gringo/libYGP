//$Id: XDialog.cpp,v 1.6 2003/03/06 03:10:50 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : X-windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
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


#include <Internal.h>

#define XK_MISCELLANY
#include <X11/keysymdef.h>

#include <gtkmm/stock.h>
#include <gtkmm/button.h>
#include <gtkmm/accelgroup.h>

#define CHECK 9
#define TRACELEVEL 9
#include <Check.h>
#include <Trace_.h>

#include "XDialog.h"


/*--------------------------------------------------------------------------*/
//Purpose   : (Default-)Constructor; creates the dialog with the specified buttons
//Parameters: buttons: Bitfield for buttons to display
/*--------------------------------------------------------------------------*/
XDialog::XDialog (unsigned int buttons)
   : Gtk::Dialog ()
     , ok ((buttons & OK) ? add_button (Gtk::Stock::OK,
                                        Gtk::RESPONSE_OK) : NULL)
     , cancel ((buttons & CANCEL) ? add_button (Gtk::Stock::CANCEL,
                                                Gtk::RESPONSE_CANCEL) : NULL) {
   TRACE9 ("XDialog::XDialog (unsigned int) - " << buttons);
   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor from an GTK+-object
//Parameters: dialog: GTK+ object to use
//            buttons: Bitfield for buttons to display
/*--------------------------------------------------------------------------*/
XDialog::XDialog (GtkDialog* dialog, unsigned int buttons)
   : Gtk::Dialog (dialog)
     , ok ((buttons & OK) ? add_button (Gtk::Stock::OK,
                                        Gtk::RESPONSE_OK) : NULL)
     , cancel ((buttons & CANCEL) ? add_button (Gtk::Stock::CANCEL,
                                                Gtk::RESPONSE_CANCEL) : NULL) {
   TRACE9 ("XDialog::XDialog (GtkDialog*, unsigned int) - " << buttons);
   init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XDialog::~XDialog () {
   TRACE9 ("XDialog::~XDialog ()");
   delete ok;
   delete cancel;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Initializes the dialog
//Parameters: buttons: Bitfield for buttons to display
/*--------------------------------------------------------------------------*/
void XDialog::init () {
   TRACE9 ("XDialog::init ()");
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

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after button-events
//Parameters: cmd: ID of pressed button
/*--------------------------------------------------------------------------*/
void XDialog::on_response (int cmd) {
   TRACE9 ("XDialog::on_response (int)" << cmd);
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

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after pressing the OK button
/*--------------------------------------------------------------------------*/
void XDialog::okEvent () {
   TRACE9 ("XDialog::okEvent ()");
   delete this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after pressing the Cancel button
/*--------------------------------------------------------------------------*/
void XDialog::cancelEvent () {
   TRACE9 ("XDialog::okEvent ()");
   delete this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Command handler of the dialog
//Parameters: action: Selected action
//Remarks   : Must not be called
/*--------------------------------------------------------------------------*/
void XDialog::command (int action) {
   Check (0);
}
