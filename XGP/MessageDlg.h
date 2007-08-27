#ifndef MESSAGEDLG_H
#define MESSAGEDLG_H

//$Id: MessageDlg.h,v 1.5 2007/08/27 20:34:23 markus Exp $

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


#include <gtkmm/messagedialog.h>


namespace Gtk {
   class Label;
   class Button;
}
namespace YGP {
   class StatusObject;
}


namespace XGP {

/**Message dialog, designed to display a StatusObject object.
 */
class MessageDlg : public Gtk::MessageDialog {
 public:
   MessageDlg (const YGP::StatusObject& obj);
   virtual ~MessageDlg ();

   static MessageDlg* create (const YGP::StatusObject& obj);

   void showDetails (bool show = true);
   void update (const YGP::StatusObject& obj);

 protected:
   static Gtk::MessageType getButtonType (YGP::StatusObject::type tp);
   void free (int);

 private:
   MessageDlg ();
   MessageDlg (const MessageDlg&);
   MessageDlg& operator= (const MessageDlg&);

   Gtk::Label*  detail;
   Gtk::Button* showDetail;

   sigc::connection cb;

   bool doRegister (bool show);
};

}

#endif
