#ifndef LOGINDLG_H
#define LOGINDLG_H

//$Id: LoginDlg.h,v 1.3 2004/10/24 00:20:17 markus Exp $

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


#include <gtkmm/entry.h>
#include <gtkmm/stock.h>

#include <YGP/Check.h>
#include <XGP/XDialog.h>

namespace Gtk {
   class Table;
}


namespace XGP {

/**Dialog to enter the information to perform a login (userid and password)

   The password is not displayed openly.
 */
class ILoginDialog : public XGP::XDialog {
 public:
   ILoginDialog (const Glib::ustring& title);
   virtual ~ILoginDialog ();

   /// Sets the passed user in the dialog
   /// \param user: User to set in the dialog
   void setUser (const Glib::ustring& user) {
      txtUser->set_text (user);
      txtUser->select_region (0, user.size ());
   }
   void setCurrentUser ();

 protected:
   Gtk::Table* pClient;             ///< Pointer to the client information area
   Gtk::Entry* txtUser;                ///< Textfield, where user enters the ID
   Gtk::Entry* txtPassword;      ///< Textfield, where user enters the password

   enum { LOGIN };

 private:

   // Prohibited manager functions
   ILoginDialog (const ILoginDialog& other);
   const ILoginDialog& operator= (const ILoginDialog& other);

   void inputChanged ();
};


/**Dialog to enter the information to perform a login (userid and password)

   The password is not displayed openly.

   This class reports the user-entered input via a callback to the caller.
 */
template <class T>
class TLoginDialog : public ILoginDialog {
 public:
   /// Declaration of the type of the callback for the user action
   typedef bool (T::*PCALLBACK)(const Glib::ustring& user,
				const Glib::ustring& password);

   TLoginDialog (const Glib::ustring& title, T& parent,
		 const PCALLBACK callback)
      : ILoginDialog (title), parent (parent), callback (callback) { }
   virtual ~TLoginDialog () { }

   /// Creates the dialog (and set it as child of the parent)
   /// \param title: Title of the dialog
   /// \param parent: Object which should be informed about the selection
   /// \param callback: Method of parent to handle information about selection 
   /// \remarks Cares also about freeing the dialog
   static TLoginDialog* create (const Glib::ustring& title, T& parent,
                               const PCALLBACK callback) {
      TLoginDialog<T>* dlg (new TLoginDialog<T> (title, parent, callback));
      dlg->get_window ()->set_transient_for (parent.get_window ());
      dlg->signal_response ().connect (mem_fun (*dlg, &TLoginDialog<T>::free));
      return dlg;
   }

 private:
   //Prohibited manager functions
   TLoginDialog (const TLoginDialog& other);
   const TLoginDialog& operator= (const TLoginDialog& other);

   /// Callback after clicking on a button in the dialog
   /// \param id: ID of clicked button
   virtual void command (int id) {
      if (id == LOGIN) {
	 Check3 (txtUser); Check3 (txtUser->get_text_length ());
	 Check3 (txtPassword);
	 if ((parent.*callback) (txtUser->get_text (), txtPassword->get_text ()))
	    response (Gtk::RESPONSE_OK);
      }
      else
	 XDialog::command (id);
   }

   T&        parent;
   PCALLBACK callback;
};

}

#endif
