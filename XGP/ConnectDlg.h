#ifndef CONNECTDLG_H
#define CONNECTDLG_H

//$Id: ConnectDlg.h,v 1.7 2003/11/14 20:28:08 markus -Rel $

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


#include <XGP/XDialog.h>

namespace std {
   class domain_error;
}

namespace Gtk {
   class Label;
   class Entry;
   class Table;
}
namespace YGP {
   class Thread;
   class Socket;
   class ConnectionMgr;
}


namespace XGP {

/**Dialog to handle enable either waiting for clients to connect or to connect
   to a server.

   The result of this actions is returned in the ConnectionMgr object.
*/
class ConnectDlg : public XDialog {
 public:
   virtual ~ConnectDlg ();

   static void perform (unsigned int cMaxConnections,
                        unsigned int defPort, YGP::ConnectionMgr& connMgr);
   static void perform (unsigned int cMaxConnections,
                        const Glib::ustring& defPort, YGP::ConnectionMgr& connMgr);

 protected:
   enum { WAIT, CONNECT };

   void command (int action);
   void cancelEvent ();
   void okEvent ();

   void valueChanged () const;

   ConnectDlg (unsigned int cMaxConnections, const Glib::ustring& defPort,
               YGP::ConnectionMgr& connMgr);

   void* waitForConnections (void* socket) throw (std::domain_error);
   virtual YGP::Socket* addClient (int socket);
   virtual void connect (const Glib::ustring& target, unsigned int port) throw (std::domain_error);

   Gtk::Entry*  pTarget;  ///< Entry field holding the target computer (IP address or name)
   Gtk::Entry*  pPort;          ///< Entry field for the port of the connection
   Gtk::Button* pWait;        ///< Button to wait for connections (server mode)
   Gtk::Button* pConnect;      ///< Button to connect to a server (client mode)
   Gtk::Table*  pClient;          ///< Table holding the controls of the dialog

   YGP::ConnectionMgr& cmgr;                      ///< Class holding the connections

 private:
   Gtk::Label*  pExplain;
   Gtk::Label*  pLblServer;
   Gtk::Label*  pLblPort;

   Glib::ustring port;

   YGP::Thread*      pThread;

   ConnectDlg (const ConnectDlg& other);
   const ConnectDlg& operator= (const ConnectDlg& other);
};

}

#endif
