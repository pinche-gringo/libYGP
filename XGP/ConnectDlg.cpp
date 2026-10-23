// PROJECT     : libXGP
// SUBSYSTEM   : X-windows
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 21.07.2003
// COPYRIGHT   : Copyright (C) 2003 - 2006, 2008, 2009, 2026

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

#include <glibmm/main.h>

#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/passwordentry.h>

#include <YGP/Check.h>
#include <YGP/ConnMgr.h>
#include <YGP/Trace.h>

#include "XGP/ConnectDlg.h"

namespace XGP {

namespace {
/// Sets the horizontal/vertical margin of a widget
void setMargins(Gtk::Widget& w, int x, int y) {
    w.set_margin_start(x);
    w.set_margin_end(x);
    w.set_margin_top(y);
    w.set_margin_bottom(y);
}
} // namespace

//-----------------------------------------------------------------------------
/// Default constructor
/// \param cMaxConnections Maximal number of connections the dialog (in
///    server mode) will wait for.
/// \param defPort Default port to listen at/send to
/// \param connMgr Connection manager; holding the connections to use
//-----------------------------------------------------------------------------
ConnectDlg::ConnectDlg(unsigned int cMaxConnections, const Glib::ustring& defPort, YGP::ConnectionMgr& connMgr)
    : XDialog(_("Connect to"), OKCANCEL), pTarget(Gtk::make_managed<Gtk::Entry>()), pPort(Gtk::make_managed<Gtk::Entry>()),
      pPassword(Gtk::make_managed<Gtk::PasswordEntry>()),
      pWait(Gtk::make_managed<Gtk::Button>(_("_Wait for connections"), true)),
      pConnect(Gtk::make_managed<Gtk::Button>(_("Connec_t"), true)), pClient(Gtk::make_managed<Gtk::Grid>()), cmgr(connMgr),
      pExplain(Gtk::make_managed<Gtk::Label>(_("Click on \"Wait for connections\" "
                                               "to wait for connections from "
                                               "other computers.\n\nIf you want "
                                               "to connect to a server, enter its "
                                               "address (name or IP number) in "
                                               "the entry field and click on "
                                               "\"Connect\".\n\nAll partners must "
                                               "enter the same password."))),
      pLblServer(Gtk::make_managed<Gtk::Label>(_("_Server:"), true)), pLblPort(Gtk::make_managed<Gtk::Label>(_("_Port:"), true)),
      pLblPassword(Gtk::make_managed<Gtk::Label>(_("Pass_word:"), true)), port(defPort), cMaxConns(cMaxConnections) {
    TRACE8("ConnectDlg::ConnectDlg(unsigned int, const Glib::ustring&, ConnectionMgr&) - " << cMaxConnections << "; Port: "
	   << defPort);

    pExplain->set_wrap();
    pExplain->set_xalign(0);
    pExplain->set_yalign(0);
    pLblServer->set_xalign(0);
    pLblServer->set_yalign(0.5);
    pLblPort->set_xalign(0);
    pLblPort->set_yalign(0.5);
    pLblPassword->set_xalign(0);
    pLblPassword->set_yalign(0.5);
    pLblServer->set_mnemonic_widget(*pTarget);
    pLblPort->set_mnemonic_widget(*pPort);
    pLblPassword->set_mnemonic_widget(*pPassword);
    pPassword->set_show_peek_icon();

    pPort->set_text(defPort);

    pConnect->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &ConnectDlg::on_response), static_cast<int>(CONNECT)));
    pWait->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &ConnectDlg::command), static_cast<int>(WAIT)));

    pExplain->set_hexpand();
    pExplain->set_vexpand();
    setMargins(*pExplain, 5, 3);
    pClient->attach(*pExplain, 0, 0, 3, 1);

    setMargins(*pLblServer, 5, 3);
    pClient->attach(*pLblServer, 0, 1, 1, 1);

    pTarget->set_hexpand();
    setMargins(*pTarget, 5, 3);
    pClient->attach(*pTarget, 1, 1, 1, 1);

    setMargins(*pLblPort, 5, 3);
    pClient->attach(*pLblPort, 0, 2, 1, 1);

    pPort->set_hexpand();
    setMargins(*pPort, 5, 3);
    pClient->attach(*pPort, 1, 2, 1, 1);

    setMargins(*pLblPassword, 5, 3);
    pClient->attach(*pLblPassword, 0, 3, 1, 1);

    pPassword->set_hexpand();
    setMargins(*pPassword, 5, 3);
    pClient->attach(*pPassword, 1, 3, 1, 1);

    setMargins(*pConnect, 5, 3);
    pClient->attach(*pConnect, 2, 1, 1, 1);

    setMargins(*pClient, 0, 5);
    get_content_area()->append(*pClient);

    pWait->set_halign(Gtk::Align::END);
    setMargins(*pWait, 5, 0);
    get_content_area()->append(*pWait);

    pPort->signal_changed().connect(sigc::mem_fun(*this, &ConnectDlg::valueChanged));
    pTarget->signal_changed().connect(sigc::mem_fun(*this, &ConnectDlg::valueChanged));
    valueChanged();

    show();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
ConnectDlg::~ConnectDlg() { stopWaiting(); }

//----------------------------------------------------------------------------
/// Performs the dialog (modal)
/// \param cMaxConnections Maximal number of connections the dialog (in
///    server mode) will wait for.
/// \param defPort Default port to listen at/send to
/// \param connMgr Connection manager; holding the connections to use
/// \returns ConnectDlg::modeConnect Mode of the connection
/// \remarks This method cares about freeing the dialog afterwards
//----------------------------------------------------------------------------
void ConnectDlg::perform(unsigned int cMaxConnections, unsigned int defPort, YGP::ConnectionMgr& connMgr) {
    std::ostringstream port;
    port << defPort;
    return perform(cMaxConnections, port.str(), connMgr);
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
void ConnectDlg::perform(unsigned int cMaxConnections, const Glib::ustring& defPort, YGP::ConnectionMgr& connMgr) {
    auto* dlg(new ConnectDlg(cMaxConnections, defPort, connMgr));
    runModal(*dlg);
    delete dlg;
}

//----------------------------------------------------------------------------
/// Command handler of the dialog
/// \param action Selected command (button)
//----------------------------------------------------------------------------
void ConnectDlg::command(int action) {
    TRACE8("ConnectDlg::command(int) - " << action);

    switch (action) {
    case CONNECT: {
        try {
            Check3(pPort->get_text_length());
            cmgr.setPassword(pPassword->get_text().raw());
            connect(pTarget->get_text(), pPort->get_text());
            valueChanged();
            response(static_cast<int>(Gtk::ResponseType::OK));
        }
        catch (boost::system::system_error& err) {
            Glib::ustring msg(_("Can't connect to server!\n\nReason: %1"));
            msg.replace(msg.find("%1"), 2, err.code().message());
            Gtk::MessageDialog dlg(msg, false, Gtk::MessageType::ERROR, Gtk::ButtonsType::CANCEL);
            dlg.set_title(_("Connect error"));
            runModal(dlg);
        }
        break;
    }

    case WAIT:
        try {
            Check3(pPort->get_text_length());
            cmgr.setPassword(pPassword->get_text().raw());
            cmgr.listenAt(pPort->get_text());

            waitForConnection();
            poller = Glib::signal_timeout().connect(sigc::mem_fun(*this, &ConnectDlg::pollConnections), 100);
            valueChanged();
        }
        catch (boost::system::system_error& err) {
            Glib::ustring msg(_("Can't bind to port!\n\nReason: %1"));
            msg.replace(msg.find("%1"), 2, err.code().message());
            Gtk::MessageDialog dlg(msg, false, Gtk::MessageType::ERROR, Gtk::ButtonsType::CANCEL);
            dlg.set_title(_("Connect error"));
            runModal(dlg);
        }
        break;

    default:
        Check(0);
    }
}

//----------------------------------------------------------------------------
/// Callback if the OK button has been pressed
//----------------------------------------------------------------------------
void ConnectDlg::okEvent() {
    TRACE8("ConnectDlg::okEvent() - " << (poller.connected() ? "Waiting" : "Finish"));
    stopWaiting();

    // Don't accept connections anymore (the partners are known)
    cmgr.stopListening();
}
//----------------------------------------------------------------------------
/// Callback if the CANCEL button has been pressed
//----------------------------------------------------------------------------
void ConnectDlg::cancelEvent() {
    TRACE8("ConnectDlg::cancelEvent() - " << (poller.connected() ? "Waiting" : "Finish"));
    stopWaiting();
    cmgr.changeMode(YGP::ConnectionMgr::NONE);
}

//----------------------------------------------------------------------------
/// Callback if the CANCEL buttons has been pressed
//----------------------------------------------------------------------------
void ConnectDlg::valueChanged() const {
    pWait->set_sensitive(!poller.connected() && pPort->get_text_length() && (cMaxConns != cmgr.getClients().size()));
    pConnect->set_sensitive(!poller.connected() && pPort->get_text_length() && pTarget->get_text_length());
    pPassword->set_sensitive(!poller.connected());
}

//----------------------------------------------------------------------------
/// Waits (asynchronously) for the next connection
//----------------------------------------------------------------------------
void ConnectDlg::waitForConnection() {
    cmgr.asyncGetNewConnection([this](std::unique_ptr<YGP::Connection> connection) {
        addClient(std::move(connection));
        if (poller.connected())
            waitForConnection();
    });
}

//----------------------------------------------------------------------------
/// Stops waiting for connections
//----------------------------------------------------------------------------
void ConnectDlg::stopWaiting() {
    if (poller.connected()) {
        poller.disconnect();
        cmgr.cancelGetNewConnection();
    }
}

//----------------------------------------------------------------------------
/// Processes the connections which arrived in the meantime
/// \returns bool True, if waiting should continue
//----------------------------------------------------------------------------
bool ConnectDlg::pollConnections() {
    auto& ctx(cmgr.getContext());
    ctx.restart();
    ctx.poll();
    return poller.connected();
}

//----------------------------------------------------------------------------
/// Adds a connected client to the vector holding the connections
/// \param connection Connection over which the client communicates (NULL,
///    if the client could not connect, e.g. because of a wrong password)
/// \returns YGP::Connection* Pointer to added connection (or \c NULL)
//----------------------------------------------------------------------------
YGP::Connection* ConnectDlg::addClient(std::unique_ptr<YGP::Connection> connection) {
    auto* newConnection(cmgr.addConnection(std::move(connection)));
    if (cMaxConns == cmgr.getClients().size()) {
        stopWaiting();
        pWait->set_sensitive(false);
    }
    return newConnection;
}

//----------------------------------------------------------------------------
/// Connects the client with the passed target
/// \param target Name or IP address of target
/// \param port Port the target is listening at (numeric or service name)
/// \throw boost::system::system_error In case of an connection error
//----------------------------------------------------------------------------
void ConnectDlg::connect(const Glib::ustring& target, const Glib::ustring& port) {
    TRACE3("ConnectDlg::connect(const Glib::ustring&, const Glib::ustring&)" << target << ':' << port);
    cmgr.connectTo(target, port);
}

} // namespace XGP
