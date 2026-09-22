// PROJECT     : libYGP
// SUBSYSTEM   : XGP
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 03.05.2006
// COPYRIGHT   : Copyright (C) 2006, 2008, 2026

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

#include "gtkmm/messagedialog.h"
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

#include "YGP/ANumeric.h"
#include "YGP/Check.h"
#include "YGP/DynTrace.h"

#undef TRACE
#undef TRACE0
#undef TRACE1
#undef TRACE2
#undef TRACE3
#undef TRACE4
#undef TRACE5
#undef TRACE6
#undef TRACE7
#undef TRACE8
#undef TRACE9

#include <YGP/Trace.h>

#include "TraceDlg.h"
#include "XGP/XDialog.h"

namespace XGP {

/**Class describing the columns in the TraceObject list
 */
class TraceObjColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    TraceObjColumns() : Gtk::TreeModel::ColumnRecord(), obj(), name(), level() {
        add(obj);
        add(name);
        add(level);
    }

    Gtk::TreeModelColumn<YGP::DynTrace*> obj;
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<int> level;
};

//-----------------------------------------------------------------------------
/// (Default-)Constructor
//-----------------------------------------------------------------------------
TraceDlg::TraceDlg() : XGP::XDialog(OKCANCEL), lstObjects(*Gtk::make_managed<Gtk::TreeView>()) {
    set_title(_("Change trace-levels"));

    TraceObjColumns cols;
    Glib::RefPtr<Gtk::ListStore> mObjects(Gtk::ListStore::create(cols));

    lstObjects.set_size_request(100, 100);
    lstObjects.set_model(mObjects);

    lstObjects.append_column(_("Objects"), cols.name);
    lstObjects.append_column(_("Level"), cols.level);

    // Make name-column sort and resize-able
    Gtk::TreeViewColumn* column(lstObjects.get_column(0));
    column->set_sort_column(cols.name);
    column->set_resizable();

    // Make level-column sort resize and editable
    Check3(lstObjects.get_column_cell_renderer(1));
    Gtk::CellRenderer* r(lstObjects.get_column_cell_renderer(1));
    Check3(r);
    Check3(typeid(*r) == typeid(Gtk::CellRendererText));
    auto* rText(dynamic_cast<Gtk::CellRendererText*>(r));
    rText->property_editable() = true;
    rText->signal_edited().connect(sigc::mem_fun(*this, &TraceDlg::levelChanged));

    column = lstObjects.get_column(1);
    column->set_sort_column(cols.level);
    column->set_resizable();

    // Fill list with all available trace-objects
    for (auto i : YGP::DynTraceManager::getObjects()) {
        Gtk::TreeRow row(*mObjects->append());
        Check3(i.first);
        Check3(i.second);
        TRACE1("Obj: " << i.second);
        row[cols.obj] = i.second;
        row[cols.name] = i.first;
        row[cols.level] = i.second->getLevel();
    }

    lstObjects.set_hexpand();
    lstObjects.set_vexpand();
    lstObjects.set_margin(5);
    get_content_area()->append(lstObjects);

    show();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TraceDlg::~TraceDlg() = default;

//-----------------------------------------------------------------------------
/// Handling of the OK button; closes the dialog with commiting data
//-----------------------------------------------------------------------------
void TraceDlg::okEvent() {
    TRACE5("TraceDlg::okEvent()");
    TraceObjColumns cols;

    for (auto& i : lstObjects.get_model()->children()) {
        YGP::ANumeric nr((int)i[cols.level]);
        YGP::DynTrace* obj(i[cols.obj]);
        Check3(obj);
        obj->setLevel(nr);
    }
}

//-----------------------------------------------------------------------------
/// Callback after changing a value in the listbox
/// \param path Path to changed line
/// \param value New value of entry
//-----------------------------------------------------------------------------
void TraceDlg::levelChanged(const Glib::ustring& path, const Glib::ustring& value) {
    TRACE9("TraceDlg::valueChanged(2x const Glib::ustring&)");

    try {
        Gtk::TreeModel::Row row(*lstObjects.get_model()->get_iter(Gtk::TreeModel::Path(path)));
        YGP::ANumeric nr(value);
        if ((nr > 9) || (nr < -1))
            throw std::invalid_argument(_("Level must be from -1 to 9!"));

        TraceObjColumns cols;
        row[cols.level] = nr;
    }
    catch (std::exception& e) {
        Gtk::MessageDialog dlg(e.what(), false, Gtk::MessageType::ERROR);
        dlg.set_title(_("Invalid value!"));
        runModal(dlg);
    }
}

} // namespace XGP
