#ifndef DIALOG_H
#define DIALOG_H

//$Id: Dialog.h,v 1.1 2003/02/03 03:47:42 markus Exp $

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


#include <string>

#include <XDialog.h>

#include <XAttrLabel.h>
#include <XAttrEntry.h>

namespace Gtk {
   class HBox;
   class Table;
}

class ANumeric;
class XFileEntry;


class Dialog : public XDialog {
 public:
   Dialog (ANumeric& numEntry, string& file);
   virtual ~Dialog ();

   static Dialog* perform (ANumeric& numEntry, string& file) {
      return new Dialog (numEntry, file); }

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

   XAttributeLabel<ANumeric>* lblNum;
   XAttributeEntry<ANumeric>* entryNum;

   XFileEntry* entryFile;

   string& file_;
};


template <class T>
class TDialog : Dialog {
 public:
   typedef void (T::*PCALLBACK) (void);

   TDialog (T& caller, const PCALLBACK callback,
            ANumeric& numEntry, string& file) : Dialog (numEntry, file)
      , obj (caller), cb (callback) { }
   virtual ~TDialog () { }

   static Dialog* perform (T& caller, const PCALLBACK callback,
                           ANumeric& numEntry, string& file) {
      return new TDialog (caller, callback, numEntry, file); }

 private:
   T& obj;
   PCALLBACK cb;

   virtual void okEvent () {
      Dialog::okEvent ();
      (obj.*cb) ();
   }
};


#endif