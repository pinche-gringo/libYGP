//$Id: XDialog.cpp,v 1.3 2003/02/24 17:31:36 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : X-windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
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

#include <gtk--/button.h>

#include <Check.h>

#include "XDialog.h"


/*--------------------------------------------------------------------------*/
//Purpose   : (Default-)Constructor; creates the dialog with the specified buttons
//Parameters: buttons: Bitfield for buttons to display
/*--------------------------------------------------------------------------*/
XDialog::XDialog (unsigned int buttons)
   : Gtk::Dialog ()
     , ok ((buttons & OK) ? new Gtk::Button (_("OK")) : NULL)
     , cancel ((buttons & CANCEL) ? new Gtk::Button (_("Cancel")) : NULL) {
   addButtons (buttons);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Constructor from an GTK+-object
//Parameters: dialog: GTK+ object to use
//            buttons: Bitfield for buttons to display
/*--------------------------------------------------------------------------*/
XDialog::XDialog (GtkDialog* dialog, unsigned int buttons)
   : Gtk::Dialog (dialog)
     , ok ((buttons & OK) ? new Gtk::Button (_("OK")) : NULL)
     , cancel ((buttons & CANCEL) ? new Gtk::Button (_("Cancel")) : NULL) {
   addButtons (buttons);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
XDialog::~XDialog () {
   delete ok;
   delete cancel;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds the specified buttons to the dialog
//Parameters: buttons: Bitfield for buttons to display
/*--------------------------------------------------------------------------*/
void XDialog::addButtons (unsigned int buttons) {
   get_action_area ()->set_homogeneous (false);
   if (buttons & OK) {
      Check3 (ok);
      addButton (*ok);
      ok->clicked.connect (bind (slot (this, &XDialog::basicCommand), OK));
      ok->grab_default ();
   }

   if (buttons & CANCEL) {
      Check3 (cancel);
      addButton (*cancel, false);
      cancel->clicked.connect (bind (slot (this, &XDialog::basicCommand), CANCEL));
      if (!(buttons & OK))
         cancel->grab_default ();
      Check3 (get_accel_group ());
      cancel->add_accelerator ("clicked", *get_accel_group (), XK_Escape, 0,
                               static_cast<GtkAccelFlags> (0));
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds the passed button to the action area of the dialog
//Parameters: button: Button to add
//            start: Flag, if button should be added at the beginning 
/*--------------------------------------------------------------------------*/
void XDialog::addButton (Gtk::Button& button, bool start) {
   button.set_usize (90, -1);
   button.show ();
   start ? get_action_area ()->pack_start (button, false, false, 5)
      : get_action_area ()->pack_end (button, false, false, 5);
   button.set_flags (GTK_CAN_DEFAULT);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after button-events
//Parameters: cmd: ID of pressed button
/*--------------------------------------------------------------------------*/
void XDialog::basicCommand (buttons cmd) {
   if (cmd == OK)
      okEvent ();
   else if (cmd == CANCEL)
      cancelEvent ();
   else
      Check (0);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after pressing the OK button
/*--------------------------------------------------------------------------*/
void XDialog::okEvent () {
   delete this;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback after pressing the Cancel button
/*--------------------------------------------------------------------------*/
void XDialog::cancelEvent () {
   delete this;
}
