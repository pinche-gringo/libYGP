//$Id: ConnectDlg.cpp,v 1.1 2003/07/25 00:30:20 markus Exp $

//PROJECT     : Cardgames
//SUBSYSTEM   : Common
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.07.2003
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


#include <Internal.h>

#include <sstream>

#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/table.h>
#include <gtkmm/messagedialog.h>

#define CHECK 9
#define TRACELEVEL 9
#include <Check.h>
#include <Trace_.h>
#include <Socket.h>
#include <Thread.h>
#include <ConnMgr.h>

#include "ConnectDlg.h"


//-----------------------------------------------------------------------------
/// Default constructor
/// \param cMaxConnections: Maximal number of connections the dialog (in
///    server mode) will wait for.
/// \param defPort: Default port to listen at/send to
/// \param connMgr: Connection manager; holding the connections to use
//-----------------------------------------------------------------------------
ConnectDlg::ConnectDlg (unsigned int cMaxConnections,
                        const Glib::ustring& defPort, ConnectionMgr& connMgr)
    : XDialog (_("Connect to"), OKCANCEL)
      , pExplain (manage (new Gtk::Label (_("Click on \"Wait for connections\" "
                                            "to wait for connections from "
                                            "other computers.\n\nIf you want "
                                            "to connect to a server, enter its "
                                            "address (name or IP number) in "
                                            "the entry field and click on "
                                            "\"Connect\"."), 0, 0)))
      , pLblServer (manage (new Gtk::Label (_("_Server:"), 0.0, 0.5, true)))
      , pTarget (manage (new Gtk::Entry ()))
      , pLblPort (manage (new Gtk::Label (_("_Port:"), 0.0, 0.5, true)))
      , pPort (manage (new Gtk::Entry ()))
      , pWait (manage (new Gtk::Button (_("_Wait for connections"), true)))
      , pConnect (manage (new Gtk::Button (_("Connec_t"), true)))
      , pClient (manage (new Gtk::Table (3, 3)))
      , port (defPort), cmgr (connMgr), pThread (NULL) {
   TRACE8 ("ConnectDlg::ConnectDlg (unsigned int, const Glib::ustring&, ConnectionMgr&) - "
           << cMaxConnections << "; Port: " << defPort);

   pExplain->set_line_wrap ();
   pLblServer->set_mnemonic_widget (*pTarget);
   pLblPort->set_mnemonic_widget (*pPort);

   pPort->set_text (defPort);

   pConnect->signal_clicked ().connect
       (bind (slot (*this, &ConnectDlg::command), CONNECT));
   pWait->signal_clicked ().connect
       (bind (slot (*this, &ConnectDlg::command), WAIT));

   pClient->attach (*pExplain, 0, 3, 0, 1, Gtk::FILL | Gtk::EXPAND,
                    Gtk::FILL | Gtk::EXPAND, 5, 3);
   pClient->attach (*pLblServer, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 5, 3);
   pClient->attach (*pTarget, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND,
                    Gtk::SHRINK, 5, 3);
   pClient->attach (*pLblPort, 0, 1, 2, 3, Gtk::SHRINK, Gtk::SHRINK, 5, 3);
   pClient->attach (*pPort, 1, 2, 2, 3, Gtk::FILL | Gtk::EXPAND,
                    Gtk::SHRINK, 5, 3);
   pClient->attach (*pConnect, 2, 3, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 5, 3);

   get_vbox ()->pack_start (*pClient, false, false, 5);
   get_action_area ()->pack_end (*pWait, false, false, 5);

   pPort->signal_changed ().connect (slot (*this, &ConnectDlg::valueChanged));
   pTarget->signal_changed ().connect (slot (*this, &ConnectDlg::valueChanged));
   valueChanged ();

   show_all ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ConnectDlg::~ConnectDlg () {
}


//----------------------------------------------------------------------------
/// Performs the dialog (modal)
/// \param cMaxConnections: Maximal number of connections the dialog (in
///    server mode) will wait for.
/// \returns ConnectDlg::modeConnect: Mode of the connection
/// \remarks This method cares about freeing the dialog afterwards
//----------------------------------------------------------------------------
void ConnectDlg::perform (unsigned int cMaxConnections, unsigned int defPort,
                          ConnectionMgr& connMgr) {
    std::ostringstream port;
    port << defPort;
    return perform (cMaxConnections, port.str (), connMgr);
}

//----------------------------------------------------------------------------
/// Performs the dialog (modal)
/// \param cMaxConnections: Maximal number of connections the dialog (in
///    server mode) will wait for.
/// \returns ConnectDlg::modeConnect: Mode of the connection
/// \remarks This method cares about freeing the dialog afterwards
//----------------------------------------------------------------------------
void ConnectDlg::perform (unsigned int cMaxConnections, const Glib::ustring& defPort,
                          ConnectionMgr& connMgr) {
    ConnectDlg* dlg (new ConnectDlg (cMaxConnections, defPort, connMgr));
    dlg->run ();
    delete dlg;
}

//----------------------------------------------------------------------------
/// Command handler of the dialog
/// \param action: Selected command (button)
//----------------------------------------------------------------------------
void ConnectDlg::command (int action) {
   TRACE8 ("ConnectDlg::command (int) - " << action);

   switch (action) {
      case CONNECT: {
         try {
            Check3 (pPort->get_text_length ());
            unsigned int prt (Socket::getPortOfService (pPort->get_text ().c_str ()));
            cmgr.connectTo (pTarget->get_text (), prt);
            valueChanged ();
            destroy_ ();
         }
         catch (std::domain_error& err) {
            Glib::ustring msg (_("Can't connect to server!\n\nReason: %1"));
            msg.replace (msg.find ("%1"), 2, err.what ());
            Gtk::MessageDialog dlg (msg, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CANCEL);
            dlg.set_title (_("Connect error"));
            dlg.run ();
         }
         break;
      }

      case WAIT:
         try {
            Check3 (pPort->get_text_length ());
            unsigned int prt (Socket::getPortOfService (pPort->get_text ().c_str ()));
            cmgr.listenAt (prt);

            pThread = OThread<ConnectDlg>::create2
                (this, &ConnectDlg::waitForConnections, NULL);
            valueChanged ();
         }
         catch (std::domain_error& err) {
            Glib::ustring msg (_("Can't connect to server!\n\nReason: %1"));
            msg.replace (msg.find ("%1"), 2, err.what ());
            Gtk::MessageDialog dlg (msg, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CANCEL);
            dlg.set_title (_("Connect error"));
            dlg.run ();
         }
         break;

      default:
         Check (0);
   }
}

//----------------------------------------------------------------------------
/// Callback if the CANCEL buttons has been pressed
//----------------------------------------------------------------------------
void ConnectDlg::cancelEvent () {
   TRACE8 ("ConnectDlg::cancelEvent () - " << (pThread ? "Waiting" : "Finish"));
   if (pThread) {
      pThread->cancel ();
      pThread = NULL;
   }
   cmgr.changeMode (ConnectionMgr::NONE);
}

//----------------------------------------------------------------------------
/// Callback if the CANCEL buttons has been pressed
//----------------------------------------------------------------------------
void ConnectDlg::valueChanged () const {
   pWait->set_sensitive (!pThread && pPort->get_text_length ());
   pConnect->set_sensitive (!pThread && pPort->get_text_length ()
                            && pTarget->get_text_length ());
}

//----------------------------------------------------------------------------
/// Waits for connections
//----------------------------------------------------------------------------
void* ConnectDlg::waitForConnections (void* pVoid) {
   while (true) {
       int socket (cmgr.getNewConnection ());
       ((Thread*)pVoid)->isToCancel ();
       cmgr.addConnection (socket);
   }
}
