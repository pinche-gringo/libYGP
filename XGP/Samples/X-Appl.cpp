//$Id: X-Appl.cpp,v 1.40 2008/05/06 09:06:05 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : XGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.40 $
//AUTHOR      : Markus Schwab
//CREATED     : 1.2.2003
//COPYRIGHT   : Copyright (C) 2003 - 2009, 2011

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

#include <cerrno>
#include <cstdlib>

#include <fstream>
#include <iomanip>

#include <gtkmm/button.h>
#include <gtkmm/native.h>
#include <gtkmm/liststore.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/application.h>
#include <gtkmm/popovermenubar.h>

#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>

#include <YGP/File.h>
#include <YGP/Check.h>
#include <YGP/DynTrace.h>
#include <YGP/ConnMgr.h>
#include <YGP/ANumeric.h>
#include <YGP/StatusObj.h>

#include <XGP/XDate.h>
#include <XGP/XAbout.h>
#include <XGP/XDialog.h>
#include <XGP/LoginDlg.h>
#include <XGP/XFileDlg.h>
#include <XGP/XPrintDlg.h>
#include <XGP/SearchDlg.h>
#include <XGP/AnimWindow.h>
#include <XGP/ConnectDlg.h>
#include <XGP/MessageDlg.h>

#include "Dialog.h"
#include "AnimWindow.h"

#include "X-Appl.h"


static YGP::DynTrace _dt_ ("Application");
static YGP::DynTrace dt ("Callbacks");


#include "images.h"

const char* XAppl::pTitles[] = { "", "File", "Size", "Last change" };


//-----------------------------------------------------------------------------
/// Defaultconstructor; all widget are created
//-----------------------------------------------------------------------------
XAppl::XAppl ()
   : XApplication ("X-Appl V" LIB_RELEASE), cols (), files (Gtk::ListStore::create (cols)),
     listFiles (files) , status (), scroll (), time (), file (), num () {
   TRACE3 ("XAppl::XAppl ()");

   setIconProgram (picXAppl, sizeof (picXAppl));
   set_default_size (620, 400);

   TRACE5 ("XAppl::XAppl () -> Create menus");
   Check3 (grpAction);

   Glib::RefPtr<Gio::Menu> menu (Gio::Menu::create ());

   Glib::RefPtr<Gio::Menu> menuFile (Gio::Menu::create ());
   Glib::RefPtr<Gio::Menu> secFile (Gio::Menu::create ());
   grpAction->add_action ("FOpen", sigc::mem_fun (*this, &XAppl::open));
   secFile->append ("_Open", "win.FOpen");
   apMenus[SAVE] = grpAction->add_action ("FSave", sigc::mem_fun (*this, &XAppl::save));
   secFile->append ("_Save", "win.FSave");
   apMenus[PRINT] = grpAction->add_action ("FPrint", sigc::mem_fun (*this, &XAppl::print));
   secFile->append ("_Print", "win.FPrint");
   menuFile->append_section (secFile);
   grpAction->add_action ("FQuit", sigc::mem_fun (*this, &XAppl::hide));
   menuFile->append ("_Quit", "win.FQuit");
   menu->append_submenu ("_File", menuFile);

   Glib::RefPtr<Gio::Menu> menuDialogs (Gio::Menu::create ());
   grpAction->add_action ("DDialog", sigc::mem_fun (*this, &XAppl::showDialog));
   menuDialogs->append ("_Dialog ...", "win.DDialog");
   grpAction->add_action ("DDate", sigc::mem_fun (*this, &XAppl::showDateDialog));
   menuDialogs->append ("Da_te ...", "win.DDate");
   grpAction->add_action ("DConnection", sigc::mem_fun (*this, &XAppl::showConnectDialog));
   menuDialogs->append ("_Connection ...", "win.DConnection");
   grpAction->add_action ("DMsg", sigc::mem_fun (*this, &XAppl::showMsgDialog));
   menuDialogs->append ("_Messagedialog ...", "win.DMsg");
   grpAction->add_action ("DLogin", sigc::mem_fun (*this, &XAppl::showLoginDialog));
   menuDialogs->append ("_Logindialog ...", "win.DLogin");
   grpAction->add_action ("DSearch", sigc::mem_fun (*this, &XAppl::showSearchDialog));
   menuDialogs->append ("_Searchdialog ...", "win.DSearch");
   menu->append_submenu ("_Dialogs", menuDialogs);

   Glib::RefPtr<Gio::Menu> menuExtras (Gio::Menu::create ());
   grpAction->add_action ("XAnimate", sigc::mem_fun (*this, &XAppl::animate));
   menuExtras->append ("_Animate", "win.XAnimate");
   menu->append_submenu ("_Extras", menuExtras);

   addHelpMenu (menu, true);

   Gtk::PopoverMenuBar* menuBar (Gtk::make_managed<Gtk::PopoverMenuBar> (menu));
   getClient ()->append (*menuBar);

   // Disable menus according to state of program
   TRACE7 ("XAppl::XAppl () -> Initialize menus");
   apMenus[SAVE]->set_enabled (false);
   apMenus[PRINT]->set_enabled (false);

   TRACE5 ("XAppl::XAppl () -> Create scrollwindow");
   scroll.set_policy (Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
   scroll.set_hexpand (); scroll.set_vexpand ();
   vboxClient->append (scroll);

   TRACE5 ("XAppl::XAppl () -> Create file-list");
   listFiles.append_column ("Size", cols.size);
   listFiles.append_column ("Date", cols.date);
   listFiles.get_column (0)->set_min_width (390);
   listFiles.get_column (1)->set_min_width (60);
   listFiles.get_column (2)->set_min_width (105);
   listFiles.get_selection ()->set_mode (Gtk::SelectionMode::MULTIPLE);

   scroll.set_child (listFiles);

   TRACE5 ("XAppl::XAppl () -> Create statusbar");
   status.push ("Populate the list with File-Open or Dialogs-Dialog");
   vboxClient->append (status);
}

//-----------------------------------------------------------------------------
/// Opens a dialog to select file(s)
//-----------------------------------------------------------------------------
void XAppl::open () {
   OTRACE3 (dt, "XAppl::open");
   XGP::FileDialog::create ("Add file(s)",
			    Gtk::FileChooser::Action::OPEN,
			    XGP::FileDialog::MUST_EXIST
			    | XGP::FileDialog::MULTIPLE)
      ->sigSelected.connect (sigc::mem_fun (*this, &XAppl::addFile));
}

//-----------------------------------------------------------------------------
/// Opens a dialog to save to a file
//-----------------------------------------------------------------------------
void XAppl::save () {
   OTRACE3 (dt, "XAppl::save");
   XGP::FileDialog::create ("Save search result to ...",
			    Gtk::FileChooser::Action::SAVE,
			    XGP::FileDialog::ASK_OVERWRITE)
      ->sigSelected.connect (sigc::mem_fun (*this, &XAppl::saveToFile));
}

//-----------------------------------------------------------------------------
/// Opens a dialog to print
//-----------------------------------------------------------------------------
void XAppl::print () {
   OTRACE3 (dt, "XAppl::print");
   XGP::PrintDialog::create ()->sigPrint.connect (sigc::mem_fun (*this, &XAppl::writeToStream));
}

//-----------------------------------------------------------------------------
/// Opens a dialog to enter a date
//-----------------------------------------------------------------------------
void XAppl::showDateDialog () {
   OTRACE3 (dt, "XAppl::showDateDialog ()");
   XGP::XDate::create ("Enter date", time);
}

//-----------------------------------------------------------------------------
/// Opens a dialog to connect with other computers
//-----------------------------------------------------------------------------
void XAppl::showConnectDialog () {
   OTRACE3 (dt, "XAppl::showConnectDialog ()");
   static YGP::ConnectionMgr cmgr;
   XGP::ConnectDlg::perform (4, 4711, cmgr);
}

//-----------------------------------------------------------------------------
/// Opens a dialog to enter a number and a filename
//-----------------------------------------------------------------------------
void XAppl::showDialog () {
   TRACE9 ("XAppl::showDialog () - Num: " << num << "; File: " << file);
   TDialog<XAppl>::create (*this, &XAppl::addActFile, num, file);
}

//-----------------------------------------------------------------------------
/// Opens a dialog displaying a status message
//-----------------------------------------------------------------------------
void XAppl::showMsgDialog () {
   OTRACE3 (dt, "XAppl::showMsgDialog ()");
   YGP::StatusObject obj (YGP::StatusObject::INFO,
			  "Some very important detail");
   obj.generalize ("Generalized info-message");
   XGP::MessageDlg::create (obj);
}

//-----------------------------------------------------------------------------
/// Opens a dialog to enter login data
//-----------------------------------------------------------------------------
void XAppl::showLoginDialog () {
   XGP::LoginDialog* dlg (XGP::LoginDialog::create (""));
   dlg->set_transient_for (*this);
   dlg->sigLogin.connect (sigc::mem_fun (*this, &XAppl::loginEvent));
}

//-----------------------------------------------------------------------------
/// Opens a dialog to enter login data
//-----------------------------------------------------------------------------
void XAppl::showSearchDialog () {
   XGP::SearchDialog::create (*this)->signalFind.connect
      (sigc::mem_fun (*this, &XAppl::find));
}

//-----------------------------------------------------------------------------
/// Shows the about box
//-----------------------------------------------------------------------------
void XAppl::showAboutbox () {
   XGP::XAbout* about (XGP::XAbout::create
                       (Glib::locale_to_utf8
			("Copyright (C) 2003 - 2009 Markus Schwab\ne-mail: g17m0@users.sourceforge.net\n"
			 "\nCompiled on " __DATE__ " at " __TIME__),
			"X-Appl V" PACKAGE_VERSION));
   about->setIconProgram (picXAppl, sizeof (picXAppl));
   about->setIconAuthor (picAuthor, sizeof (picAuthor));
}

//-----------------------------------------------------------------------------
/// Add the value of file to the list
//-----------------------------------------------------------------------------
void XAppl::addActFile () {
   if (file.size ())
      // This could be optimized ...
      for (unsigned int i (0); i < (unsigned int)num; ++i)
	 addFile (file);
}

//-----------------------------------------------------------------------------
/// Add the selected file to the list
/// \param file: Name of file to add
//-----------------------------------------------------------------------------
void XAppl::addFile (const std::string& file) {
   TRACE9 ("XAppl::addFile (const std::string&) - " << file);

   try {
      YGP::File objFile (file.c_str ());
      YGP::ATimestamp t (objFile.time (), false );
      YGP::ANumeric s (objFile.size ());
      std::string name (objFile.path ());
      name += objFile.name ();

      Check3 (files);
      Gtk::TreeModel::Row row (*(files->append ()));
      row[cols.icon] = XGP::XFileList::getIcon4File (objFile);
      row[cols.name] = objFile.name ();
      row[cols.size] = s.toString ();
      row[cols.date] = t.toString ().c_str ();

      // Enable menus
      apMenus[SAVE]->set_enabled (true);
      apMenus[PRINT]->set_enabled (true);
   }
   catch (YGP::FileError& e) {
      Gtk::MessageDialog dlg (e.what (), false, Gtk::MessageType::ERROR);
      XGP::runModal (dlg);
   }
}

//-----------------------------------------------------------------------------
/// Save result of comparison into a file
/// \param file: Name of file to create
//-----------------------------------------------------------------------------
void XAppl::saveToFile (const std::string& file) {
   TRACE9 ("XAppl::saveToFile (string&): " << file);

   FILE* output (fopen (file.c_str (), "w"));
   if (!output) {
      std::string err ("Can't create file `%1'\n Reason: %2");
      err.replace (err.find ("%1"), 2, file);
      err.replace (err.find ("%2"), 2, strerror (errno));
      Gtk::MessageDialog dlg (err, false, Gtk::MessageType::ERROR);
      XGP::runModal (dlg);
      return;
   }
   writeToStream (output);
}

//-----------------------------------------------------------------------------
/// Save result of comparison into a file
/// \param file: Stream to fill
//-----------------------------------------------------------------------------
void XAppl::writeToStream (FILE* file) {
   TRACE9 ("XAppl::writeToStream (FILE*)");
   Check (file);

   Gtk::TreeNodeChildren rows (files->children ());
   Gtk::TreeNodeChildren::const_iterator i (rows.begin ());
   while (i != rows.end ()) {
      std::string filename ((*i)[cols.name]);
      TRACE8 ("XAppl::writeToStream (FILE*): " << filename);
      Glib::ustring date ((*i)[cols.date]);
      Glib::ustring size ((*i)[cols.size]);

      filename.append (78 - filename.length () - size.length () - date.length (), ' ');
      fprintf (file, "%s%s %s\n", filename.c_str (), size.c_str (), date.c_str ());
      ++i;
   } // end-for all text-columns
}

//-----------------------------------------------------------------------------
/// Callback for login-dialog
/// \param user: Input for the user
/// \param password: Input for the password
/// \returns bool: true
//-----------------------------------------------------------------------------
bool XAppl::loginEvent (const Glib::ustring& user,
			const Glib::ustring& password) {
   status.pop ();
   Glib::ustring txt ("User: ");
   txt += user;
   txt += "; Password: ";
   txt += password;
   status.push (txt);
   return true;
}

//-----------------------------------------------------------------------------
/// Callback for find-dialog
/// \param text: Text to find
//-----------------------------------------------------------------------------
void XAppl::find (const Glib::ustring& text) {
   status.pop ();
   Glib::ustring txt ("Find: ");
   status.push (txt + text);
}


//-----------------------------------------------------------------------------
/// Command animate - animates a window
//-----------------------------------------------------------------------------
void XAppl::animate () {
   YGP::StatusObject obj (YGP::StatusObject::INFO, "Animated window");
   Glib::signal_idle ().connect
      (sigc::bind (sigc::ptr_fun (&XAppl::doAnimate), XGP::MessageDlg::create (obj)));
}

//-----------------------------------------------------------------------------
/// Animates a window
//-----------------------------------------------------------------------------
bool XAppl::doAnimate (Gtk::Widget* winAnim) {
   Glib::RefPtr<Gdk::Surface> surface
      (winAnim->get_native () ? winAnim->get_native ()->get_surface () : Glib::RefPtr<Gdk::Surface> ());
   AnimWindow* albl (AnimWindow::create (surface));
   albl->animate ();
   return false;
}


//-----------------------------------------------------------------------------
/// Entrypoint of application
/// \param argc: Number of parameters
/// \param argv: Array with pointer to parameter
/// \returns int: Status
//-----------------------------------------------------------------------------
int main (int argc, char* argv[]) {
   srand (time (0));

   XAppl::initI18n ();

   Glib::RefPtr<Gtk::Application> appl (Gtk::Application::create ("org.ygp.x-appl"));

   // Keyboard accelerators for the actions set up in XAppl::XAppl()
   appl->set_accel_for_action ("win.DDialog", "<Control>d");
   appl->set_accel_for_action ("win.DDate", "<Control>t");
   appl->set_accel_for_action ("win.DConnection", "<Control>c");
   appl->set_accel_for_action ("win.DMsg", "<Control>m");
   appl->set_accel_for_action ("win.DLogin", "<Control>l");
   appl->set_accel_for_action ("win.XAnimate", "<Control>a");

   return appl->make_window_and_run<XAppl> (argc, argv);
}
