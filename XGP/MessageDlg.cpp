//$Id: MessageDlg.cpp,v 1.5 2004/01/17 04:02:32 markus Rel $

//PROJECT     : Cardgames
//SUBSYSTEM   : <FILLIN>
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.5 $
//AUTHOR      : Markus Schwab
//CREATED     : 19.11.2003
//COPYRIGHT   : Copyright (C) 2003, 2004

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

#include <gtkmm/label.h>
#include <gtkmm/button.h>

#include <YGP/StatusObj.h>

#include <XGP/XDialog.h>

#include "MessageDlg.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param obj: StatusObject to display
//-----------------------------------------------------------------------------
MessageDlg::MessageDlg (const YGP::StatusObject& obj)
    : Gtk::MessageDialog (Glib::locale_to_utf8 (obj.getMessage ()),
                          getButtonType (obj.getType ()))
      , detail (new Gtk::Label)
      , showDetail (new Gtk::Button) {
   get_vbox ()->pack_end (*detail, Gtk::PACK_EXPAND_WIDGET, 5);
   get_action_area ()->pack_end (*showDetail, Gtk::PACK_SHRINK, 5);
   showDetail->set_use_underline ();

   if (obj.hasDetails ()) {
      showDetails (false);
      detail->set_text (Glib::locale_to_utf8 (obj.getDetails ()));
      showDetail->show ();
   }
   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
MessageDlg::~MessageDlg () {
   delete detail;
   delete showDetail;
}


//----------------------------------------------------------------------------
/// Converts 
/// \param tp: Type of the StatusObject to display 
/// \return Gtk::MessageType: Type of message to display
//----------------------------------------------------------------------------
Gtk::MessageType MessageDlg::getButtonType (YGP::StatusObject::type tp) {
   return ((tp == YGP::StatusObject::ERROR)
           ? Gtk::MESSAGE_ERROR : (Gtk::MessageType)tp);
}

//----------------------------------------------------------------------------
/// Shows or hides the details box and adapts the show button accordingly
/// \param show: Flag, if details should be shown 
/// \remarks If the object does not have any details, the show-details button
///       is disabled
//----------------------------------------------------------------------------
void MessageDlg::showDetails (bool show) {
   showDetail->set_label (Glib::locale_to_utf8
                          (show ? _("Hide _details") : _("Show _details")));
   show ? detail->show () : detail->hide ();

   cb.disconnect ();
   cb = showDetail->signal_clicked ().connect
       (bind (slot (*this, &MessageDlg::showDetails), !show));
}

//----------------------------------------------------------------------------
/// Creates a (modeless) dialog and registers a handler to free it after
/// deleting.
/// \param obj: StatusObject object to display in the dialog
//  \returns MessageDlg*: Pointer to created dialog
//----------------------------------------------------------------------------
MessageDlg* MessageDlg::create (const YGP::StatusObject& obj) {
   MessageDlg* dlg (new MessageDlg (obj));
   dlg->signal_response ().connect (slot (*dlg, &MessageDlg::free));
   return dlg;
}

//-----------------------------------------------------------------------------
/// Frees the dialog.
/// \remarks Call only if the dialog was created with new
//-----------------------------------------------------------------------------
void MessageDlg::free (int) {
   delete this;
}

//-----------------------------------------------------------------------------
/// Frees the dialog.
/// \remarks Call only if the dialog was created with new
//-----------------------------------------------------------------------------
void MessageDlg::update (const YGP::StatusObject& obj) {
   set_message (Glib::locale_to_utf8 (obj.getMessage ()));
   if (obj.hasDetails ()) {
      detail->set_text (Glib::locale_to_utf8 (obj.getDetails ()));
      showDetail->show ();
   }
   else {
      detail->set_text ("");
      showDetail->hide ();
   }
}

}
