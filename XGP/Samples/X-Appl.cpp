//$Id: X-Appl.cpp,v 1.19 2004/01/15 06:26:35 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : X-Windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.19 $
//AUTHOR      : Markus Schwab
//CREATED     : 1.2.2003
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

#include <cerrno>
#include <cstdlib>

#include <fstream>
#include <iomanip>

#include <gtkmm/liststore.h>
#include <gtkmm/messagedialog.h>

#include <YGP/File.h>
#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/ConnMgr.h>
#include <YGP/ANumeric.h>
#include <YGP/Internal.h>

#include <XGP/XDate.h>
#include <XGP/XAbout.h>
#include <XGP/XFileDlg.h>
#include <XGP/XPrintDlg.h>
#include <XGP/ConnectDlg.h>

#include "Dialog.h"
#include "X-Appl.h"

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


XGP::XApplication::MenuEntry XAppl::menuItems[] = {
    { "_File",                  "<alt>F",      0,       BRANCH },
    { "_Open ...",              "<ctl>O",      OPEN,    ITEM },
    { "_Save ...",              "<ctl>S",      SAVE,    ITEM },
    { "_Print ...",             "<ctl>P",      PRINT,   ITEM },
    { "",                       "",            0,       SEPARATOR },
    { "E_xit",                  "<ctl>Q",      EXIT,    ITEM },
    { "_Dialogs",               "<alt>D",      0,       BRANCH },
    { "_Dialog ...",            "<ctl>D",      DIALOG,  ITEM },
    { "Da_te ...",              "<ctl>T",      DATE,    ITEM },
    { "_Connection ...",        "<ctl>C",      CONNECT, ITEM },
    { "_Menus",                 "<alt>M",      0,       BRANCH },
    { "_Radiobuttons",          "<alt>R",      0,       SUBMENU },
    {    "Button _1",           "<ctl>1",      0,       RADIOITEM },
    {    "Button _2",           "<ctl>2",      0,       RADIOITEM },
    {    "Button _3",           "<ctl>3",      0,       LASTRADIOITEM },
    {  "",                      "",            0,       SUBMENUEND },
    { "_Checkbuttons",          "<alt>C",      0,       SUBMENU },
    {    "Button _1",           "<alt>1",      0,       CHECKITEM },
    {    "Button _2",           "<alt>2",      0,       CHECKITEM },
    {    "Button _3",           "<alt>3",      0,       CHECKITEM } };


/*--------------------------------------------------------------------------*/
//Purpose   : Defaultconstructor; all widget are created
/*--------------------------------------------------------------------------*/
XAppl::XAppl ()
   : XApplication ("X" PACKAGE " V" LIB_RELEASE)
     , files (XGP::XFileListStore::create (cols)), listFiles (files)
     , status (), scroll () {
   TRACE3 ("XAppl::XAppl ()");

   setIconProgram (xpmXAppl);
   set_default_size (620, 400);

   TRACE5 ("XAppl::XAppl () -> Create menus");
   addMenus (menuItems, sizeof (menuItems) / sizeof (menuItems[0]));
   showHelpMenu ();

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

/*--------------------------------------------------------------------------*/
//Purpose   : Command-handler
//Parameters: menu: ID of command (menu)
/*--------------------------------------------------------------------------*/
void XAppl::command (int menu) {
   switch (menu) {
   case OPEN:
      XGP::TFileDialog<XAppl>::create (Glib::locale_to_utf8 (_("Add file...")),
                                       *this, &XAppl::addFile,
                                       XGP::IFileDialog::MUST_EXIST);
      break;

   case SAVE:
      XGP::TFileDialog<XAppl>::create (Glib::locale_to_utf8 (_("Save search result to ...")),
                                       *this, &XAppl::saveToFile,
                                       XGP::IFileDialog::ASK_OVERWRITE);
      break;

   case PRINT:
      XGP::TPrintDialog<XAppl>::create (*this, &XAppl::writeToStream);
      break;

   case EXIT:
      hide ();
      break;

   case DATE:
      XGP::XDate::create ("Enter date", time);
      break;

   case CONNECT: {
      static YGP::ConnectionMgr cmgr;
      XGP::ConnectDlg::perform (4, 4711, cmgr); 
      break; }

   case DIALOG:
      TRACE9 ("XAppl::command (int) - Num: " << num << "; File: " << file);
      TDialog<XAppl>::create (*this, &XAppl::addActFile, num, file);
      break;

   default:
      XGP::XApplication::command (menu);
   } // end-switch
}

/*--------------------------------------------------------------------------*/
//Purpose   : Shows the about box
/*--------------------------------------------------------------------------*/
void XAppl::showAboutbox () {
   XGP::XAbout* about (XGP::XAbout::create
                       ("Copyright (C) 2003, 2004 Markus Schwab\ne-mail: g17m0@lycos.com\n"
                        "\nCompiled on " __DATE__ " at " __TIME__,
                        "X" PACKAGE " V" VERSION));
   about->setIconProgram (xpmXAppl);
   about->setIconAuthor (xpmAuthor);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Add the value of file to the list
/*--------------------------------------------------------------------------*/
void XAppl::addActFile () {
   for (unsigned int i (0); i < (unsigned int)num; ++i)
      addFile (file);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Add the selected file to the list
//Parameters: file: Name of file to add
/*--------------------------------------------------------------------------*/
void XAppl::addFile (const std::string& file) {
   TRACE9 ("XAppl::addFile (std::string&): " << file);

   try {
      YGP::File objFile (file.c_str ());
      YGP::ATimestamp t (objFile.time (), false );
      std::string name (objFile.path ());
      name += objFile.name ();

      Check3 (files);
      Gtk::TreeModel::Row row (*(files->append (&objFile)));
      row[cols.size] = objFile.size ();
      row[cols.date] = t.toString ().c_str ();

      // Enable menus
      apMenus[SAVE]->set_sensitive (true);
      apMenus[PRINT]->set_sensitive (true);
   }
   catch (std::string& e) {
      Gtk::MessageDialog dlg (e, Gtk::MESSAGE_ERROR);
      dlg.run ();
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Save result of comparison into a file
//Parameters: file: Name of file to create
/*--------------------------------------------------------------------------*/
void XAppl::saveToFile (const std::string& file) {
   TRACE9 ("XAppl::saveToFile (string&): " << file);

   std::ofstream output (file.c_str ());
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

/*--------------------------------------------------------------------------*/
//Purpose   : Save result of comparison into a file
//Parameters: file: Stream to fill
/*--------------------------------------------------------------------------*/
void XAppl::writeToStream (std::ostream& file) {
   TRACE9 ("XAppl::writeToStream (ofstream&)");
   Check (file);

   int lenTime (YGP::ATimestamp::now ().toString ().length () + 1);

   Gtk::TreeNodeChildren rows (files->children ());
   Gtk::TreeNodeChildren::const_iterator i (rows.begin ());
   while (i != rows.end ()) {
      std::string filename ((*i)[cols.name]);
      TRACE8 ("XAppl::writeToStream (ofstream&): " << filename);
      std::string date ((*i)[cols.date]);

      file << filename << std::setw (78 - filename.length () - lenTime) << ' ' 
           << (*i)[cols.size] << ' ' << date << '\n';
      ++i;
   } // end-for all text-columns
}


/*--------------------------------------------------------------------------*/
//Purpose   : Entrypoint of application
//Parameters: argc: Number of parameters
//            argv: Array with pointer to parameter
//Returns   : int: Status
/*--------------------------------------------------------------------------*/
int main (int argc, char* argv[]) {
   srand (time (0));

   XAppl::initI18n ();

   Gtk::Main appl (argc, argv);
   XAppl win;
   appl.run (win);
   return 0;
}
