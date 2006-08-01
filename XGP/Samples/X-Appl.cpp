//$Id: X-Appl.cpp,v 1.37 2006/08/01 02:31:33 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : X-Windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.37 $
//AUTHOR      : Markus Schwab
//CREATED     : 1.2.2003
//COPYRIGHT   : Copyright (C) 2003 - 2006

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

#include <cerrno>
#include <cstdlib>

#include <fstream>
#include <iomanip>

#include <gtkmm/stock.h>
#include <gtkmm/liststore.h>
#include <gtkmm/messagedialog.h>

#include <YGP/File.h>
#include <YGP/Check.h>
#include <YGP/DynTrace.h>
#include <YGP/ConnMgr.h>
#include <YGP/ANumeric.h>
#include <YGP/StatusObj.h>

#include <XGP/XDate.h>
#include <XGP/XAbout.h>
#include <XGP/LoginDlg.h>
#include <XGP/XFileDlg.h>
#include <XGP/XPrintDlg.h>
#include <XGP/SearchDlg.h>
#include <XGP/ConnectDlg.h>
#include <XGP/MessageDlg.h>

#include "Dialog.h"
#include "X-Appl.h"


static YGP::DynTrace _dt_ ("Application");
static YGP::DynTrace dt ("Callbacks");

// Pixmap for program
const char* XAppl::xpmXAppl[] = {
   /* width height ncolors chars_per_pixel */
   "48 48 5 1",
   /* colors */
   "  c #000000",
   ". c #505550",
   "X c #AEAAAE",
   "o c #FFFFFF",
   "O c None",
   /* pixels */
   "oooooooooooooooooooooooooooooooooooooooooooooooo",
   "o                                              o",
   "o oo.                                      oo. o",
   "o oX                                       oX  o",
   "o .                                        .   .",
   "o                                              .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XX                                       .XX .",
   "o XX .X XXXXXoooooooooXoooooooooooXoooooooXoXX .",
   "o XX X..X....o........ o.......... o...... oXX .",
   "o XX .X X....o........ o.......... o...... oXX .",
   "o XX X..X....X         X           X       oXX .",
   "o XX .X XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXoXX .",
   "o XX X..X....XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXoXX .",
   "o XX .X X....XX.X.X.XXXXX.X.X.X.XXXXX.X.XXXoXX .",
   "o XX X..X....XX.X.X.XXXXX.X.X.X.XXXXX.X.XXXoXX .",
   "o XX .X XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXoXX .",
   "o XX X..X....XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXoXX .",
   "o XX .. X....XX.X.X.X.XXX.X.X.XXXXXXX.X.XXXoXX .",
   "o XX oo.X....XX.X.X.X.XXX.X.X.XXXXXXX.X.XXXoXX .",
   "o XX oX XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXoXX .",
   "o XX .  X....XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXoXX .",
   "o XX oo.X....XX.X.X.XXXXX.X.X.X.XXXXX.X.XXXoXX .",
   "o XX oX X....XX.X.X.XXXXX.X.X.X.XXXXX.X.XXXoXX .",
   "o XX .  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXoXX .",
   "o XX.oooooooooooooooooooooooooooooooooooooooXX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXX.............XXX .",
   "o XXoo.X.X.X.X.XXXXXX.X.X.X.XX.ooooooooooooXXX .",
   "o XXoX X.X.X.X.XXXXXX.X.X.X.XX.ooooooooooooXXX .",
   "o XX.  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXX.............XXX .",
   "o XXoo.X.X.X.XXXXXXXXXX.X.X.XX.ooooooooooooXXX .",
   "o XXoX X.X.X.XXXXXXXXXX.X.X.XX.ooooooooooooXXX .",
   "o XX.  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XXXXXXXXXXXXXoooooooo.Xoooooooo.Xoooooooo.XX .",
   "o XXXXXXXXXXXXXoXXXXXXX XoXXXXXXX XoXXXXXXX XX .",
   "o XXXXXXXXXXXXXoXXXXXXX XoXXXXXXX XoXXXXXXX XX .",
   "o XXXXXXXXXXXXX.        X.        X.        XX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX .",
   "o                                              .",
   "ooooo..........................................." };

// Pixmap for author
const char* XAppl::xpmAuthor[] = {
   "56 43 5 1",
   " 	c None",
   "!	c #0000FF",
   "@	c #000000",
   "-	c #AEAAAE",
   ".	c #FFFFFF",
   ".......................................................@",
   "..-----------------------------------------------------@",
   "..-----------------------------------------------------@",
   "..----------------------!!!!!!-------------------------@",
   "..--------------------!!!!@@!!!!-----------------------@",
   "..------------------!!!!@@---@@!!!---------------------@",
   "..-----------------!!!@@--------!!!--------------------@",
   "..-----------------!!!@---!!----!!!--------------------@",
   "..----------------!!!@---!!!!@---!!!-------------------@",
   "..----------------!!@---!!!!!!@---!!@------------------@",
   "..----------------!!@----!!!!@----!!@------------------@",
   "..----------------!!!-----!!@----!!!@------------------@",
   "..-----------------!!!-----@-----!!!@------------------@",
   "..-----------------!!!!---------!!!@-------------------@",
   "..------------------!!!!-------!!!@--------------------@",
   "..--------------------!!!!--!!!!@@---------------------@",
   "..----------------------!!!!!!@@-----------------------@",
   "..-----------------------!!!!@-------------------------@",
   "..------------------------!!@--------------------------@",
   "..------------------------!!@--------------------------@",
   "..------------------!!!!!!!!!!!!!!---------------------@",
   "..-----------------!!!!!!!!!!!!!!!!--------------------@",
   "..----------------!!!@@@@@!!@@@@@!!!-------------------@",
   "..----------------!!@-----!!@-----!!@------------------@",
   "..----------------!!@-----!!@-----!!@------------------@",
   "..----------------!!@-----!!@-----!!@------------------@",
   "..----------------!!@-----!!@-----!!@------------------@",
   "..-----------------@@-----!!@------@@------------------@",
   "..------------------------!!@--------------------------@",
   "..--------------------!!!!!!!!!!-----------------------@",
   "..------------------!!!!!!!!!!!!!!---------------------@",
   "..-----------------!!!@@@@@@@@@!!!!--------------------@",
   "..-----------------!!@----------!!!!-------------------@",
   "..----------------!!!@-----------!!!@------------------@",
   "..----------------!!@------------!!!@------------------@",
   "..----------------!!@-------------!!@------------------@",
   "..----------------!!@-------------!!@------------------@",
   "..----------------!@--------------!!@------------------@",
   "..----------------!!@--------------@@------------------@",
   "..-----------------!@----------------------------------@",
   "..------------------@----------------------------------@",
   "..-----------------------------------------------------@",
   "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" };


const char* XAppl::pTitles[] = { "", "File", "Size", "Last change" };


//-----------------------------------------------------------------------------
/// Defaultconstructor; all widget are created
//-----------------------------------------------------------------------------
XAppl::XAppl ()
   : XApplication ("X-Appl V" LIB_RELEASE)
     , files (Gtk::ListStore::create (cols)), listFiles (files)
     , status (), scroll () {
   TRACE3 ("XAppl::XAppl ()");

   setIconProgram (xpmXAppl);
   set_default_size (620, 400);

   TRACE5 ("XAppl::XAppl () -> Create menus");
   Check3 (grpAction);
   Glib::ustring ui ("<ui><menubar name='Menu'>"
		     "  <menu action='File'>"
		     "    <menuitem action='FOpen'/>"
		     "    <menuitem action='FSave'/>"
		     "    <menuitem action='FPrint'/>"
		     "    <separator/>"
		     "    <menuitem action='FQuit'/>"
		     "  </menu>"
		     "  <menu action='Dialogs'>"
		     "    <menuitem action='DDialog'/>"
		     "    <menuitem action='DDate'/>"
		     "    <menuitem action='DConnection'/>"
		     "    <menuitem action='DMsg'/>"
		     "    <menuitem action='DLogin'/>"
		     "    <menuitem action='DSearch'/>"
		     "  </menu>");

   grpAction->add (Gtk::Action::create ("File", "_File"));
   grpAction->add (Gtk::Action::create ("FOpen", Gtk::Stock::OPEN),
		   mem_fun (*this, &XAppl::open));
   grpAction->add (apMenus[SAVE] = Gtk::Action::create ("FSave", Gtk::Stock::SAVE),
		   mem_fun (*this, &XAppl::save));
   grpAction->add (apMenus[PRINT] = Gtk::Action::create ("FPrint", Gtk::Stock::PRINT),
		   mem_fun (*this, &XAppl::print));
   grpAction->add (Gtk::Action::create ("FQuit", Gtk::Stock::QUIT),
		   mem_fun (*this, &XAppl::hide));
   grpAction->add (Gtk::Action::create ("Dialogs", "_Dialogs"));
   grpAction->add (Gtk::Action::create ("DDialog", "_Dialog ...",
					"Tests the common dialog"),
		   Gtk::AccelKey ("<ctl>D"),
		   mem_fun (*this, &XAppl::showDialog));
   grpAction->add (Gtk::Action::create ("DDate", "Da_te ...",
					"Tests the date dialog"),
		   Gtk::AccelKey ("<ctl>T"),
		   mem_fun (*this, &XAppl::showDateDialog));
   grpAction->add (Gtk::Action::create ("DConnection", "_Connection ...",
					"Tests the connection dialog"),
		   Gtk::AccelKey ("<ctl>C"),
		   mem_fun (*this, &XAppl::showConnectDialog));
   grpAction->add (Gtk::Action::create ("DMsg", "_Messagedialog ...",
					"Tests the message dialog"),
		   Gtk::AccelKey ("<ctl>M"),
		   mem_fun (*this, &XAppl::showMsgDialog));
   grpAction->add (Gtk::Action::create ("DLogin", "_Logindialog ...",
					"Tests the login dialog"),
		   Gtk::AccelKey ("<ctl>L"),
		   mem_fun (*this, &XAppl::showLoginDialog));
   grpAction->add (Gtk::Action::create ("DSearch", Gtk::Stock::FIND, "_Searchdialog ...",
					"Tests the search dialog"),
		   mem_fun (*this, &XAppl::showSearchDialog));
   addHelpMenu (ui, true);
   ui += "</menubar></ui>";
   mgrUI->insert_action_group (grpAction);
   add_accel_group (mgrUI->get_accel_group ());
   mgrUI->add_ui_from_string (ui);

   getClient ()->pack_start (*mgrUI->get_widget("/Menu"), Gtk::PACK_SHRINK);

   // Disable menus according to state of program
   TRACE7 ("XAppl::XAppl () -> Initialize menus");
   apMenus[SAVE]->set_sensitive (false);
    apMenus[PRINT]->set_sensitive (false);

   TRACE5 ("XAppl::XAppl () -> Create scrollwindow");
   scroll.set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   vboxClient->pack_start (scroll);

   TRACE5 ("XAppl::XAppl () -> Create file-list");
   listFiles.append_column ("Size", cols.size);
   listFiles.append_column ("Date", cols.date);
   listFiles.get_column (0)->set_min_width (390);
   listFiles.get_column (1)->set_min_width (60);
   listFiles.get_column (2)->set_min_width (105);
   listFiles.get_selection ()->set_mode (Gtk::SELECTION_EXTENDED);

   scroll.add (listFiles);

   TRACE5 ("XAppl::XAppl () -> Create statusbar");
   status.push ("Populate the list with File-Open or Dialogs-Dialog");
   vboxClient->pack_start (status, Gtk::PACK_SHRINK);

   show_all_children ();
}

//-----------------------------------------------------------------------------
/// Opens a dialog to select file(s)
//-----------------------------------------------------------------------------
void XAppl::open () {
   OTRACE3 (dt, "XAppl::open");
   XGP::FileDialog::create ("Add file(s)",
			    Gtk::FILE_CHOOSER_ACTION_OPEN,
			    XGP::FileDialog::MUST_EXIST
			    | XGP::FileDialog::MULTIPLE)
      ->sigSelected.connect (mem_fun (*this, &XAppl::addFile));
}

//-----------------------------------------------------------------------------
/// Opens a dialog to save to a file
//-----------------------------------------------------------------------------
void XAppl::save () {
   OTRACE3 (dt, "XAppl::save");
   XGP::FileDialog::create ("Save search result to ...",
			    Gtk::FILE_CHOOSER_ACTION_SAVE,
			    XGP::FileDialog::ASK_OVERWRITE)
      ->sigSelected.connect (mem_fun (*this, &XAppl::saveToFile));
}

//-----------------------------------------------------------------------------
/// Opens a dialog to print
//-----------------------------------------------------------------------------
void XAppl::print () {
   OTRACE3 (dt, "XAppl::print");
   XGP::PrintDialog::create ()->sigPrint.connect (mem_fun (*this, &XAppl::writeToStream));
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
   dlg->get_window ()->set_transient_for (get_window ());
   dlg->sigLogin.connect (mem_fun (*this, &XAppl::loginEvent));
}

//-----------------------------------------------------------------------------
/// Opens a dialog to enter login data
//-----------------------------------------------------------------------------
void XAppl::showSearchDialog () {
   XGP::SearchDialog::create (get_window ())->signalFind.connect
      (mem_fun (*this, &XAppl::find));
}

//-----------------------------------------------------------------------------
/// Shows the about box
//-----------------------------------------------------------------------------
void XAppl::showAboutbox () {
   XGP::XAbout* about (XGP::XAbout::create
                       (Glib::locale_to_utf8
			("Copyright (C) 2003 - 2006 Markus Schwab\ne-mail: g17m0@lycos.com\n"
			 "\nCompiled on " __DATE__ " at " __TIME__),
			"X-Appl V" VERSION));
   about->setIconProgram (xpmXAppl);
   about->setIconAuthor (xpmAuthor);
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
      apMenus[SAVE]->set_sensitive (true);
      apMenus[PRINT]->set_sensitive (true);
   }
   catch (YGP::FileError& e) {
      Gtk::MessageDialog dlg (e.what (), Gtk::MESSAGE_ERROR);
      dlg.run ();
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
      Gtk::MessageDialog dlg (err, Gtk::MESSAGE_ERROR);
      dlg.run ();
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
/// Entrypoint of application
/// \param argc: Number of parameters
/// \param argv: Array with pointer to parameter
/// \returns int: Status
//-----------------------------------------------------------------------------
int main (int argc, char* argv[]) {
   srand (time (0));

   XAppl::initI18n ();

   Gtk::Main appl (argc, argv);
   XAppl win;
   appl.run (win);
   return 0;
}
