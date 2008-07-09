//$Id: ConnectDlg.cpp,v 1.18 2008/03/30 13:39:17 markus Rel $

//PROJECT     : libXGP
//SUBSYSTEM   : X-windows
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.18 $
//AUTHOR      : Markus Schwab
//CREATED     : 21.07.2003
//COPYRIGHT   : Copyright (C) 2003 - 2006, 2008

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


#define CONVERT_TO_UTF8
#include <YGP/Internal.h>

#include <sstream>

#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/table.h>
#include <gtkmm/messagedialog.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/Socket.h>
#include <YGP/Thread.h>
#include <YGP/ConnMgr.h>

#include "XGP/ConnectDlg.h"


namespace XGP {

//-----------------------------------------------------------------------------
/// Default constructor
/// \param cMaxConnections Maximal number of connections the dialog (in
///    server mode) will wait for.
/// \param defPort Default port to listen at/send to
/// \param connMgr Connection manager; holding the connections to use
//-----------------------------------------------------------------------------
ConnectDlg::ConnectDlg (unsigned int cMaxConnections,
                        const Glib::ustring& defPort, YGP::ConnectionMgr& connMgr)
   : XDialog (_("Connect to"), OKCANCEL),
     pTarget (manage (new Gtk::Entry)),
     pPort (manage (new Gtk::Entry)),
     pWait (manage (new Gtk::Button (_("_Wait for connections"), true))),
     pConnect (manage (new Gtk::Button (_("Connec_t"), true))),
     pClient (manage (new Gtk::Table (3, 3))),
     cmgr (connMgr),
     pExplain (manage (new Gtk::Label (_("Click on \"Wait for connections\" "
					 "to wait for connections from "
					 "other computers.\n\nIf you want "
					 "to connect to a server, enter its "
					 "address (name or IP number) in "
					 "the entry field and click on "
					 "\"Connect\"."), 0, 0))),
     pLblServer (manage (new Gtk::Label (_("_Server:"), 0.0, 0.5, true))),
     pLblPort (manage (new Gtk::Label (_("_Port:"), 0.0, 0.5, true))),
     port (defPort), pThread (NULL), cMaxConns (cMaxConnections) {
   TRACE8 ("ConnectDlg::ConnectDlg (unsigned int, const Glib::ustring&, ConnectionMgr&) - "
           << cMaxConnections << "; Port: " << defPort);

   pExplain->set_line_wrap ();
   pLblServer->set_mnemonic_widget (*pTarget);
   pLblPort->set_mnemonic_widget (*pPort);

   pPort->set_text (defPort);

   pConnect->signal_clicked ().connect
       (sigc::bind<int> (sigc::mem_fun (*this, &ConnectDlg::on_response), CONNECT));
   pWait->signal_clicked ().connect
       (sigc::bind<int> (sigc::mem_fun (*this, &ConnectDlg::command), WAIT));

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

   pPort->signal_changed ().connect (mem_fun (*this, &ConnectDlg::valueChanged));
   pTarget->signal_changed ().connect (mem_fun (*this, &ConnectDlg::valueChanged));
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
/// \param cMaxConnections Maximal number of connections the dialog (in
///    server mode) will wait for.
/// \param defPort Default port to listen at/send to
/// \param connMgr Connection manager; holding the connections to use
/// \returns ConnectDlg::modeConnect Mode of the connection
/// \remarks This method cares about freeing the dialog afterwards
//----------------------------------------------------------------------------
void ConnectDlg::perform (unsigned int cMaxConnections, unsigned int defPort,
                          YGP::ConnectionMgr& connMgr) {
   std::ostringstream port;
   port << defPort;
   return perform (cMaxConnections, port.str (), connMgr);
}

//----------------------------------------------------------------------------
/// Performs the dialog (modal)
/// \param cMaxConnections Maximal number of connections the dialog (in
///    server mode) will wait for.
/// \param defPort Default port to listen at/send to
/// \param connMgr Connection manager; holding the connections to use
/// \returns ConnectDlg::modeConnect Mode of the connection
/// \remarks This method cares about freeing the dialog afterwards
//----------------------------------------------------------------------------
void ConnectDlg::perform (unsigned int cMaxConnections, const Glib::ustring& defPort,
                          YGP::ConnectionMgr& connMgr) {
   ConnectDlg* dlg (new ConnectDlg (cMaxConnections, defPort, connMgr));
   dlg->run ();
   delete dlg;
}

//----------------------------------------------------------------------------
/// Command handler of the dialog
/// \param action Selected command (button)
//----------------------------------------------------------------------------
void ConnectDlg::command (int action) {
   TRACE8 ("ConnectDlg::command (int) - " << action);

   switch (action) {
      case CONNECT: {
         try {
            Check3 (pPort->get_text_length ());
            unsigned int prt (YGP::Socket::getPortOfService (pPort->get_text ().c_str ()));
            connect (pTarget->get_text (), prt);
            valueChanged ();
            response (Gtk::RESPONSE_OK);
         }
         catch (YGP::CommError& err) {
            Glib::ustring msg (_("Can't connect to server!\n\nReason: %1"));
            msg.replace (msg.find ("%1"), 2, err.what ());
            Gtk::MessageDialog dlg (msg, false, Gtk::MESSAGE_ERROR,
                                    Gtk::BUTTONS_CANCEL);
            dlg.set_title (_("Connect error"));
            dlg.run ();
         }
         break;
      }

      case WAIT:
         try {
            Check3 (pPort->get_text_length ());
            unsigned int prt (YGP::Socket::getPortOfService (pPort->get_text ().c_str ()));
            cmgr.listenAt (prt);

            pThread = YGP::OThread<ConnectDlg>::create2
                (this, &ConnectDlg::waitForConnections, NULL);
            pThread->allowCancelation ();
            valueChanged ();
         }
         catch (YGP::CommError& err) {
            Glib::ustring msg (_("Can't bind to port!\n\nReason: %1"));
            msg.replace (msg.find ("%1"), 2, err.what ());
            Gtk::MessageDialog dlg (msg, false, Gtk::MESSAGE_ERROR,
                                    Gtk::BUTTONS_CANCEL);
            dlg.set_title (_("Connect error"));
            dlg.run ();
         }
         break;

      default:
         Check (0);
   }
}

//----------------------------------------------------------------------------
/// Callback if the OK button has been pressed
//----------------------------------------------------------------------------
void ConnectDlg::okEvent () {
   TRACE8 ("ConnectDlg::okEvent () - " << (pThread ? "Waiting" : "Finish"));
   if (pThread) {
      pThread->cancel ();
      pThread = NULL;
   }
}
//----------------------------------------------------------------------------
/// Callback if the CANCEL button has been pressed
//----------------------------------------------------------------------------
void ConnectDlg::cancelEvent () {
   TRACE8 ("ConnectDlg::cancelEvent () - " << (pThread ? "Waiting" : "Finish"));
   if (pThread) {
      pThread->cancel ();
      pThread = NULL;
   }
   cmgr.changeMode (YGP::ConnectionMgr::NONE);
}

//----------------------------------------------------------------------------
/// Callback if the CANCEL buttons has been pressed
//----------------------------------------------------------------------------
void ConnectDlg::valueChanged () const {
   pWait->set_sensitive (!pThread && pPort->get_text_length ()
			 && (cMaxConns != cmgr.getClients ().size ()));
   pConnect->set_sensitive (!pThread && pPort->get_text_length ()
                            && pTarget->get_text_length ());
}

//----------------------------------------------------------------------------
/// Waits for connections
/// \throw YGP::CommError In case of an connection error
//----------------------------------------------------------------------------
void* ConnectDlg::waitForConnections (void* pVoid) throw (YGP::CommError){
   while (true) {
       int socket (cmgr.getNewConnection ());
       ((YGP::Thread*)pVoid)->isToCancel ();
       addClient (socket);
   }
}

//----------------------------------------------------------------------------
/// Adds a connected client to the vector holding the connections
/// \param socket Socket over which the client communicates
/// \returns Socket* Pointer to created socket (or \c NULL)
//----------------------------------------------------------------------------
YGP::Socket* ConnectDlg::addClient (int socket) {
   YGP::Socket* newSocket (cmgr.addConnection (socket));
   if (cMaxConns == cmgr.getClients ().size ()) {
      delete pThread;
      pThread = NULL;
      pWait->set_sensitive (false);
   }
   return newSocket;
}

//----------------------------------------------------------------------------
/// Connects the client with the passed target
/// \param target Name or IP address of target
/// \param port Port the target is listening at
/// \throw YGP::CommError In case of an connection error
//----------------------------------------------------------------------------
void ConnectDlg::connect (const Glib::ustring& target, unsigned int port) throw (YGP::CommError) {
   TRACE3 ("PlayerConnectDlg::connect (const Glib::ustring&, unsigned int)"
           << target << ':' << port);
   cmgr.connectTo (target, port);
}

}
