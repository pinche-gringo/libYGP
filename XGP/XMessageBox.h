#ifndef XMESSAGEBOX_H
#define XMESSAGEBOX_H

//$Id: XMessageBox.h,v 1.8 2002/08/01 03:51:49 markus Rel $

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


#include <vector.h>

#include <gtk--/dialog.h>

#include "SmartPtr.h"


// Forward declarations
namespace Gtk {
   class HBox;
   class Label;
   class Pixmap;
   class Button;

}
   
using namespace Gtk;


// Class to display simple messages in a dialog-window. This window contains
// action-buttons and a title (if not a specific one is passed) according to
// the passed flag.
class XMessageBox : public Dialog {
 public:
   enum options { INFO, QUESTION, WARNING, ERROR, CRITICAL,
                  TYPEBITS = 4, TYPEMASK = 0xf,
                  OK = 16, RETRY = 32, YES = 64, NO = 128, CANCEL = 256,
                  END = CANCEL, YESNO = YES | NO, YESNOCANCEL = YESNO | CANCEL,
                  OKCANCEL = OK | CANCEL, RETRYCANCEL = RETRY | CANCEL,
                  OKRETRYCANCEL = OK | RETRYCANCEL };

   static int Show (const string& text, const string& title = "",
                    int flags = OK | INFO, unsigned int defButton = 0);
   static int Show (const string& text, int flags = OK | INFO,
                    unsigned int defButton = 0) {
      return Show (text, "", flags, defButton); }

   virtual ~XMessageBox ();

 protected:
   XMessageBox (const string& text, const string& title = "",
                int flags = OK | INFO, unsigned int defButton = 0);

   virtual void perform (int action);

 private:
   typedef SmartPtr<Label>  PLabel;
   typedef SmartPtr<HBox>   PHBox;
   typedef SmartPtr<Pixmap> PPixmap;

   // Prohibited manager-functions
   XMessageBox ();
   XMessageBox (const XMessageBox&);
   
   const XMessageBox& operator= (const XMessageBox&);

   int ret;

   vector<Button*> buttons;
   PLabel txt;
   PHBox  client;
   PPixmap icon;

   static const char* const iconInfo[];
   static const char* const iconQuestion[];
   static const char* const iconWarning[];
   static const char* const iconError[];
   static const char* const iconCritical[];

   static const char* const titles[];
   static const char* const labels[];
   static const char* const * const icons[];
};


template <class T> class XMessageDialog : public XMessageBox {
 public:
   virtual ~XMessageDialog () { }

   typedef void (T::*PCALLBACK) (unsigned int);

   static XMessageBox* Show (T& object, const PCALLBACK callback,
                             const string& text, const string& title = "",
                             int flags = OK | INFO, unsigned int defButton = 0) {
      return new XMessageDialog<T> (object, callback, text, title, flags, defButton); }

   static XMessageBox* Show (T& object, const PCALLBACK callback,
                             const string& text, int flags = OK | INFO,
                             unsigned int defButton = 0) {
      return Show (object, callback, text, "", flags, defButton); }

 protected:
   XMessageDialog (T& object, const PCALLBACK callback,
                   const string& text, const string& title = "",
                   int flags = OK | INFO, unsigned int defButton = 0)
      : obj (object), pCallback (callback)
      , XMessageBox (text, title, flags, defButton) { }

   virtual void perform (int action) {
      (obj.*pCallback) (action);
      delete this; }

 private:
   T& obj;
   PCALLBACK pCallback;
};

#endif
