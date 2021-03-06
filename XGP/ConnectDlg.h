#ifndef XGP_CONNECTDLG_H
#define XGP_CONNECTDLG_H

//$Id: ConnectDlg.h,v 1.11 2008/05/18 13:21:27 markus Rel $

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


#include <XGP/XDialog.h>

namespace Gtk {
   class Label;
   class Entry;
   class Table;
}
namespace YGP {
   class Thread;
   class Socket;
   class CommError;
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

   void* waitForConnections (void* socket) throw (YGP::CommError);
   virtual YGP::Socket* addClient (int socket);
   virtual void connect (const Glib::ustring& target, unsigned int port) throw (YGP::CommError);

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

   YGP::Thread*  pThread;
   unsigned int  cMaxConns;

   ConnectDlg (const ConnectDlg& other);
   const ConnectDlg& operator= (const ConnectDlg& other);
};

}

#endif
