//$Id: Dialog.cpp,v 1.11 2005/04/30 03:00:13 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : Samples
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.11 $
//AUTHOR      : Markus Schwab
//CREATED     : 01.02.2003
//COPYRIGHT   : Copyright (C) 2003 - 2005

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


#include <gtkmm/label.h>
#include <gtkmm/table.h>

#define CHECK 9
#define TRACELEVEL 9
#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/ANumeric.h>
#include <YGP/MetaEnum.h>

#include <XGP/XFileEntry.h>

#include "Dialog.h"


class MetaEnumTest : public YGP::MetaEnum {
 public:
   MetaEnumTest () {
      insert (std::make_pair (0, "Undefined"));
      insert (std::make_pair (1, "One"));
      insert (std::make_pair (2, "Two"));
      insert (std::make_pair (3, "Three"));
      insert (std::make_pair (10, "Ten"));
   }
};
static MetaEnumTest metaEnum;

static unsigned int n1 (0);
static unsigned int n2 (0);


//-----------------------------------------------------------------------------
/// (Default-)Constructor
/// \param numEntry: Value for the numeric fields
/// \param file: File read from FileEntry
//-----------------------------------------------------------------------------
Dialog::Dialog (YGP::ANumeric& numEntry, std::string& file)
   : XGP::XDialog ("Dialog", OKCANCEL),
     pClient (manage (new Gtk::Table (5, 2))),
     lblLabel (manage (new Gtk::Label ("AttributeLabel"))),
     lblEntry (manage (new Gtk::Label ("_AttributeEntry", true))),
     lblFileEntry (manage (new Gtk::Label ("_FileEntry", true))),
     entryEnum (manage (new XGP::EnumEntry (metaEnum))),
     lblNum (manage (new XGP::XAttributeLabel<YGP::ANumeric> (numEntry))),
     entryNum (manage (new XGP::XAttributeEntry<YGP::ANumeric> (numEntry))),
     spinNum (manage (new XGP::XAttributeSpinEntry<unsigned int> (n1))),
     entryFile (manage (new XGP::XFileEntry ())),
     file_ (file) {
   TRACE9 ("Dialog::Dialog (ANumeric&, std::string&) - Num: " << numEntry
           << "; String: " << file);

   TRACE5 ("Dialog::Dialog (ANumeric&, std::string&) - Creating labels");
   lblLabel->set_justify (Gtk::JUSTIFY_LEFT);
   lblEntry->set_justify (Gtk::JUSTIFY_LEFT);
   lblFileEntry->set_justify (Gtk::JUSTIFY_LEFT);
   lblEntry->set_mnemonic_widget (*entryNum);
   lblFileEntry->set_mnemonic_widget (*entryFile);

   TRACE5 ("Dialog::Dialog (ANumeric&, std::string&) - Showing fields");
   entryFile->set_text (file);

   TRACE5 ("Dialog::Dialog (ANumeric&, std::string&) - Attaching controls");
   pClient->attach (*lblLabel, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL, 5, 2);
   pClient->attach (*lblNum,   1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 5, 2);

   pClient->attach (*lblEntry, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL, 5, 2);
   pClient->attach (*entryNum, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 5, 2);

   pClient->attach (*lblFileEntry, 0, 1, 2, 3, Gtk::FILL, Gtk::FILL, 5, 2);
   pClient->attach (*entryFile,    1, 2, 2, 3, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 5, 2);

   Gtk::Label* lblEnum (manage (new Gtk::Label ("_MetaEnum", true)));
   lblFileEntry->set_mnemonic_widget (*entryEnum);

   entryEnum->set_active_text (metaEnum[n2]);
   pClient->attach (*lblEnum,   0, 1, 3, 4, Gtk::FILL, Gtk::FILL, 5, 2);
   pClient->attach (*entryEnum, 1, 2, 3, 4, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 5, 2);

   Gtk::Label* lblSpin (manage (new Gtk::Label ("_SpinButton", true)));
   lblFileEntry->set_mnemonic_widget (*spinNum);
   spinNum->set_range (0.0, 1000000.0);
   spinNum->set_increments (1.0, 100.0);
   spinNum->set_numeric (false);

   pClient->attach (*lblSpin, 0, 1, 4, 5, Gtk::FILL, Gtk::FILL, 5, 2);
   pClient->attach (*spinNum, 1, 2, 4, 5, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 5, 2);

   get_vbox ()->pack_start (*pClient, false, false, 5);
   show_all_children ();
   show ();

   entryNum->grab_focus ();
   spinNum->update ();
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Dialog::~Dialog () {
}

//-----------------------------------------------------------------------------
/// Handling of the OK button; closes dialog with commiting data
//-----------------------------------------------------------------------------
void Dialog::okEvent () {
   ok->grab_focus ();              // So that AttributeEntry-fields are updated
   entryNum->commit ();
   spinNum->commit ();
   file_ = entryFile->get_text ();

   n2 = metaEnum[entryEnum->get_active_text ()];
   XGP::XDialog::okEvent ();
}
