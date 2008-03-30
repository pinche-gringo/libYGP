#ifndef DIALOG_H
#define DIALOG_H

//$Id: Dialog.h,v 1.10 2008/03/30 13:39:17 markus Rel $

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


#include <string>

#include <XGP/EnumEntry.h>
#include <XGP/XAttrSpin.h>
#include <XGP/XAttrLabel.h>
#include <XGP/XAttrEntry.h>

#include <XGP/XDialog.h>

namespace Gtk {
   class HBox;
   class Table;
}
namespace YGP {
   class ANumeric;
}
namespace XGP {
   class EnumEntry;
   class XFileEntry;
}


class Dialog : public XGP::XDialog {
 public:
   Dialog (YGP::ANumeric& numEntry, std::string& file);
   virtual ~Dialog ();

   static Dialog* create (YGP::ANumeric& numEntry, std::string& file) {
      Dialog* dlg (new Dialog (numEntry, file));
      dlg->signal_response ().connect (mem_fun (*dlg, &Dialog::free));
      return dlg;
   }

 protected:
   virtual void okEvent ();

 private:
   //Prohibited manager functions
   Dialog (const Dialog& other);
   const Dialog& operator= (const Dialog& other);

   Gtk::Table* pClient;

   Gtk::Label* lblLabel;
   Gtk::Label* lblEntry;
   Gtk::Label* lblFileEntry;

   XGP::EnumEntry*                      entryEnum;
   XGP::XAttributeLabel<YGP::ANumeric>* lblNum;
   XGP::XAttributeEntry<YGP::ANumeric>* entryNum;

   Gtk::Adjustment* adjNum;
   XGP::XAttributeSpinEntry<unsigned int>* spinNum;

   XGP::XFileEntry* entryFile;

   std::string& file_;
};


template <class T>
class TDialog : Dialog {
 public:
   typedef void (T::*PCALLBACK) (void);

   TDialog (T& caller, const PCALLBACK callback,
            YGP::ANumeric& numEntry, std::string& file) : Dialog (numEntry, file)
      , obj (caller), cb (callback) { }
   virtual ~TDialog () { }

   static Dialog* create (T& caller, const PCALLBACK callback,
                          YGP::ANumeric& numEntry, std::string& file) {
      Dialog* dlg (new TDialog (caller, callback, numEntry, file));
      dlg->signal_response ().connect (mem_fun (*dlg, &TDialog<T>::free));
      return dlg;
   }

 private:
   T& obj;
   PCALLBACK cb;

   virtual void okEvent () {
      Dialog::okEvent ();
      (obj.*cb) ();
   }
};


#endif
