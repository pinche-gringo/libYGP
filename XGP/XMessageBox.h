#ifndef XMESSAGEBOX_H
#define XMESSAGEBOX_H

//$Id: XMessageBox.h,v 1.12 2003/03/03 05:53:43 markus Exp $

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


#include <vector>

#include <gtkmm/dialog.h>

#include <SmartPtr.h>


// Forward declarations
namespace Gtk {
   class HBox;
   class Label;
   class Image;
   class Button;
}
   

// Class to display simple messages in a dialog-window. This window contains
// action-buttons, a title (either a default one according to the type of the
// message or a specified one) and an icon according to the type of the message.
//
// The following (self explainatory) message types are supported:
//   - INFO
//   - QUESTION
//   - WARNING
//   - ERROR
//   - CRITICAL
// depending on the message displaced.
//
// The action buttons can be one of the following; selecting the button makes
// the message box return the same ID:
//   - OK
//   - CANCEL
//   - END
//   - YES
//   - NO
//   - RETRY
// or any of those combinations:
//   - YESNO
//   - YESNOCANCEL
//   - OKCANCEL
//   - RETRYCANCEL
//   - OKRETRYCANCEL
//
// If there is a cancel button, the dialog can also be closed by pressing the
// ESC-key.
class XMessageBox : public Gtk::Dialog {
 public:
   enum options { INFO, QUESTION, WARNING, ERROR, CRITICAL,
                  TYPEBITS = 4, TYPEMASK = 0xf,
                  OK = 16, RETRY = 32, YES = 64, NO = 128, CANCEL = 256,
                  END = CANCEL, YESNO = YES | NO, YESNOCANCEL = YESNO | CANCEL,
                  OKCANCEL = OK | CANCEL, RETRYCANCEL = RETRY | CANCEL,
                  OKRETRYCANCEL = OK | RETRYCANCEL };

   static int Show (const std::string& text, const std::string& title = "",
                    int flags = OK | INFO, unsigned int defButton = 0);
   static int Show (const std::string& text, int flags = OK | INFO,
                    unsigned int defButton = 0) {
      return Show (text, "", flags, defButton); }

   virtual ~XMessageBox ();

 protected:
   XMessageBox (const std::string& text, const std::string& title = "",
                int flags = OK | INFO, unsigned int defButton = 0);

   virtual void perform (int action);

 private:
   typedef SmartPtr<Gtk::Label> PLabel;
   typedef SmartPtr<Gtk::HBox>  PHBox;
   typedef SmartPtr<Gtk::Image> PImage;

   // Prohibited manager-functions
   XMessageBox ();
   XMessageBox (const XMessageBox&);
   
   const XMessageBox& operator= (const XMessageBox&);

   int ret;

   std::vector<Gtk::Button*> buttons;
   PLabel txt;
   PHBox  client;
   PImage icon;

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
   typedef void (T::*PCALLBACK) (unsigned int);

   static XMessageBox* Show (T& object, const PCALLBACK callback,
                             const std::string& text, const std::string& title = "",
                             int flags = OK | INFO, unsigned int defButton = 0) {
      return new XMessageDialog<T> (object, callback, text, title, flags, defButton); }

   static XMessageBox* Show (T& object, const PCALLBACK callback,
                             const std::string& text, int flags = OK | INFO,
                             unsigned int defButton = 0) {
      return Show (object, callback, text, "", flags, defButton); }

 protected:
   XMessageDialog (T& object, const PCALLBACK callback,
                   const std::string& text, const std::string& title = "",
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


extern "C"
int showMessageBox (char* text, char* title, int flags, unsigned int defButton);

#endif
