 //$Id: X-Appl.cpp,v 1.1 2003/02/01 19:44:05 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : X-Windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 1.2.2003
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

#include <gzo-cfg.h>

#include <errno.h>

#include <fstream>

#include <Check.h>
#include <Trace_.h>

#include <ATStamp.h>

#include <XAbout.h>
#include "XFileDlg.h"
#include "XPrintDlg.h"
#include <XMessageBox.h>

#include <X-Appl.h>

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


XApplication::MenuEntry XAppl::menuItems[] = {
    { "_File",                  "<alt>f",      0,       BRANCH },
    { "_Open",                  "<ctl>O",      OPEN,    ITEM },
    { "_Save",                  "<ctl>s",      SAVE,    ITEM },
    { "_Print",                 "<ctl>p",      PRINT,   ITEM },
    { "",                       "",               0,       SEPARATOR },
    { "E_xit",                  "<ctl>q",      EXIT,    ITEM },
    { "_Dialogs",               "<alt>d",      0,       BRANCH } };


/*--------------------------------------------------------------------------*/
//Purpose   : Defaultconstructor; all widget are created
/*--------------------------------------------------------------------------*/
XAppl::XAppl ()
   : XApplication ("X" PACKAGE " V" VERSION)
     , tblInput (2, 2)
     , listFiles (sizeof (pTitles) / sizeof (pTitles[0]), pTitles)
     , status (), scroll () {
   TRACE3 ("XAppl::XAppl ()");

   set_usize (620, 400);

   TRACE5 ("XAppl::XAppl () -> Create menus");
   addMenus (menuItems, sizeof (menuItems) / sizeof (menuItems[0]));
   showHelpMenu ();

   // Disable menus according to state of program
   TRACE7 ("XAppl::XAppl () -> Initialize menus");
   apMenus[SAVE]->set_sensitive (false);
   apMenus[PRINT]->set_sensitive (false);

   // Create controls
   TRACE5 ("XAppl::XAppl () -> Create table");
   tblInput.show ();
   vboxClient->pack_start (tblInput, false, true, 5);

   TRACE5 ("XAppl::XAppl () -> Create scrollwindow");
   scroll.set_policy (GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   vboxClient->pack_start (scroll);

   TRACE5 ("XAppl::XAppl () -> Create file-list");
   listFiles.column_titles_passive ();
   listFiles.set_column_justification (2, GTK_JUSTIFY_RIGHT);
   listFiles.set_column_justification (3, GTK_JUSTIFY_CENTER);
   listFiles.column (0).set_width (15);
   listFiles.column (2).set_width (60);
   listFiles.column (3).set_width (105);
   listFiles.set_selection_mode (GTK_SELECTION_EXTENDED);
   listFiles.show ();

   scroll.add_with_viewport (listFiles);
   scroll.show ();

   TRACE5 ("XAppl::XAppl () -> Create statusbar");
   status.push (1, "Enter filesspecification to search for");
   status.show ();
   vboxClient->pack_start (status, false);

   show ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Command-handler
//Parameters: menu: ID of command (menu)
/*--------------------------------------------------------------------------*/
void XAppl::command (int menu) {
   switch (menu) {
   case OPEN:
      break;

   case SAVE:
      XFileDialog::perform (string ("Save search result to..."), this,
                            (XFileDialog::PACTION)&XAppl::saveToFile,
                            XFileDialog::ASK_OVERWRITE);
      break;

   case PRINT:
      XPrintDialog::perform (this, (XPrintDialog::PACTION)&XAppl::writeToStream);
      break;

   case EXIT:
      Main::quit ();
      break;

   default:
      XApplication::command (menu);
   } // end-switch
}

/*--------------------------------------------------------------------------*/
//Purpose   : Shows the about box
/*--------------------------------------------------------------------------*/
void XAppl::showAboutbox () {
   XAbout* about (new XAbout ("Anticopyright (A) 2003 Markus Schwab\n"
                              "e-mail: g17m0@lycos.com\n"
                              "\nCompiled on " __DATE__ " at " __TIME__,
                              "X" PACKAGE " V" VERSION));
   about->setIconProgram (xpmXAppl);
   about->setIconAuthor (xpmAuthor);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Save result of comparison into a file
//Parameters: file: Name of file to create
/*--------------------------------------------------------------------------*/
void XAppl::saveToFile (string& file) {
   TRACE9 ("XAppl::saveToFile (string&): " << file);

   ofstream output (file.c_str ());
   if (!output) {
      string error ("Can't create file `%1'\n Reason: %2");
      error.replace (error.find ("%1"), 2, file);
      error.replace (error.find ("%2"), 2, strerror (errno));
      XMessageBox::Show (error, XMessageBox::ERROR);
      return;
   }
   writeToStream (output);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Save result of comparison into a file
//Parameters: file: Stream to fill
/*--------------------------------------------------------------------------*/
void XAppl::writeToStream (ofstream& file) {
   TRACE9 ("XAppl::writeToStream (ofstream&)");
   Check (file);

   int lenTime (ATimestamp::now ().toString ().length () + 1);

   Check1 (listFiles.columns ().size () == 4);
   for (int i (0); i < listFiles.rows ().size (); ++i) {
      string filename (listFiles.get_text (i, 1));
      TRACE8 ("XAppl::writeToStream (ofstream&): " << filename);

      file << filename << setw (79 - filename.length () - lenTime)
           << listFiles.get_text (i, 2).c_str () << ' '
           << listFiles.get_text (i, 3).c_str () << '\n';
   } // end-for all text-columns
}


/*--------------------------------------------------------------------------*/
//Purpose   : Entrypoint of application
//Parameters: argc: Number of parameters
//            argv: Array with pointer to parameter
//Returns   : int: Status
/*--------------------------------------------------------------------------*/
int main (int argc, char* argv[]) {
   Main appl (argc, argv);
   XAppl win;
   appl.run ();
   return 0;
}
