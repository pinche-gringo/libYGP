//$Id: Dialog.cpp,v 1.16 2008/05/06 09:04:40 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : XGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.16 $
//AUTHOR      : Markus Schwab
//CREATED     : 01.02.2003
//COPYRIGHT   : Copyright (C) 2003 - 2005, 2008, 2011

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


#include <gtkmm/label.h>
#include <gtkmm/grid.h>
#include <gtkmm/adjustment.h>

#include <YGP/Check.h>
#include <YGP/DynTrace.h>
#include <YGP/ANumeric.h>
#include <YGP/MetaEnum.h>

#include <XGP/XFileEntry.h>

#include "Dialog.h"


namespace {
   /// Sets the horizontal/vertical margin of a widget
   void setMargins (Gtk::Widget& w, int x, int y) {
      w.set_margin_start (x);
      w.set_margin_end (x);
      w.set_margin_top (y);
      w.set_margin_bottom (y);
   }
}


static YGP::DynTrace _dt_ ("Dialog");

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
     pClient (Gtk::make_managed<Gtk::Grid> ()),
     lblLabel (Gtk::make_managed<Gtk::Label> ("AttributeLabel")),
     lblEntry (Gtk::make_managed<Gtk::Label> ("_AttributeEntry", true)),
     lblFileEntry (Gtk::make_managed<Gtk::Label> ("_FileEntry", true)),
     entryEnum (Gtk::make_managed<XGP::EnumEntry> (metaEnum)),
     lblNum (Gtk::make_managed<XGP::XAttributeLabel<YGP::ANumeric>> (numEntry)),
     entryNum (Gtk::make_managed<XGP::XAttributeEntry<YGP::ANumeric>> (numEntry)),
     spinNum (Gtk::make_managed<XGP::XAttributeSpinEntry<unsigned int>> (n1, Gtk::Adjustment::create (0, 0, 10000000.0, 1, 100))),
     entryFile (Gtk::make_managed<XGP::XFileEntry> ()),
     file_ (file) {
   TRACE9 ("Dialog::Dialog (ANumeric&, std::string&) - Num: " << numEntry
           << "; String: " << file);

   TRACE5 ("Dialog::Dialog (ANumeric&, std::string&) - Creating labels");
   lblLabel->set_justify (Gtk::Justification::LEFT);
   lblEntry->set_justify (Gtk::Justification::LEFT);
   lblFileEntry->set_justify (Gtk::Justification::LEFT);
   lblEntry->set_mnemonic_widget (*entryNum);
   lblFileEntry->set_mnemonic_widget (*entryFile);

   TRACE5 ("Dialog::Dialog (ANumeric&, std::string&) - Showing fields");
   entryFile->set_text (file);

   TRACE5 ("Dialog::Dialog (ANumeric&, std::string&) - Attaching controls");
   setMargins (*lblLabel, 5, 2); pClient->attach (*lblLabel, 0, 0);
   setMargins (*lblNum, 5, 2); lblNum->set_hexpand (); pClient->attach (*lblNum, 1, 0);

   setMargins (*lblEntry, 5, 2); pClient->attach (*lblEntry, 0, 1);
   setMargins (*entryNum, 5, 2); entryNum->set_hexpand (); pClient->attach (*entryNum, 1, 1);

   setMargins (*lblFileEntry, 5, 2); pClient->attach (*lblFileEntry, 0, 2);
   setMargins (*entryFile, 5, 2); entryFile->set_hexpand (); pClient->attach (*entryFile, 1, 2);

   Gtk::Label* lblEnum (Gtk::make_managed<Gtk::Label> ("_MetaEnum", true));
   lblFileEntry->set_mnemonic_widget (*entryEnum);

   entryEnum->set_active_text (metaEnum[n2]);
   std::cout << "Value: " << entryEnum->get_active_text () << std::endl << std::flush;
   setMargins (*lblEnum, 5, 2); pClient->attach (*lblEnum, 0, 3);
   setMargins (*entryEnum, 5, 2); entryEnum->set_hexpand (); pClient->attach (*entryEnum, 1, 3);

   Gtk::Label* lblSpin (Gtk::make_managed<Gtk::Label> ("_SpinButton", true));
   lblFileEntry->set_mnemonic_widget (*spinNum);

   setMargins (*lblSpin, 5, 2); pClient->attach (*lblSpin, 0, 4);
   setMargins (*spinNum, 5, 2); spinNum->set_hexpand (); pClient->attach (*spinNum, 1, 4);

   pClient->set_margin (5);
   get_content_area ()->append (*pClient);
   show ();

   entryNum->grab_focus ();
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
   std::cout << "okEvent" << std::endl << std::flush;
   ok->grab_focus ();              // So that AttributeEntry-fields are updated
   std::cout << "okEvent - commit entry" << std::endl << std::flush;
   entryNum->commit ();
   std::cout << "okEvent - commit spin" << std::endl << std::flush;
   spinNum->commit ();
   std::cout << "okEvent - getFilename" << std::endl << std::flush;
   file_ = entryFile->get_text ();

   std::cout << "okEvent - getEnumValue" << std::endl << std::flush;
   std::cout << "okEvent " << entryEnum->get_active_text () << std::endl << std::flush;
   n2 = metaEnum[entryEnum->get_active_text ()];
   XGP::XDialog::okEvent ();
}
