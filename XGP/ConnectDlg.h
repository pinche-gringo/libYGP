#ifndef CONNECTDLG_H
#define CONNECTDLG_H

//$Id: ConnectDlg.h,v 1.3 2003/07/27 03:35:45 markus Exp $

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


#include <XDialog.h>

namespace Gtk {
   class Label;
   class Entry;
   class Table;
}

class Thread;
class Socket;
class ConnectionMgr;


/**Dialog to handle enable either waiting for clients to connect or to connect
   to a server.

   The result of this actions is returned in the ConnectionMgr object.
*/
class ConnectDlg : public XDialog {
 public:
   virtual ~ConnectDlg ();

   static void perform (unsigned int cMaxConnections,
                        unsigned int defPort, ConnectionMgr& connMgr);
   static void perform (unsigned int cMaxConnections,
                        const Glib::ustring& defPort, ConnectionMgr& connMgr);

 protected:
   enum { WAIT, CONNECT };

   void command (int action);
   void cancelEvent ();

   void valueChanged () const;

   ConnectDlg (unsigned int cMaxConnections, const Glib::ustring& defPort,
               ConnectionMgr& connMgr);

   void* waitForConnections (void* socket);
   virtual Socket* addClient (int socket);
   virtual void connect (const Glib::ustring& target, unsigned int port);

 protected:
   Gtk::Entry*  pTarget;
   Gtk::Entry*  pPort;
   Gtk::Button* pWait;
   Gtk::Button* pConnect;
   Gtk::Table*  pClient;

   ConnectionMgr& cmgr;

 private:
   Gtk::Label*  pExplain;
   Gtk::Label*  pLblServer;
   Gtk::Label*  pLblPort;

   Glib::ustring port;

   Thread*      pThread;

   ConnectDlg (const ConnectDlg& other);
   const ConnectDlg& operator= (const ConnectDlg& other);
};

#endif
