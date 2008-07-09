//$Id: TraceDlg.cpp,v 1.4 2008/03/30 13:39:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : XGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
//AUTHOR      : Markus Schwab
//CREATED     : 03.05.2006
//COPYRIGHT   : Copyright (C) 2006, 2008

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

#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include "gtkmm/messagedialog.h"

#include "YGP/Check.h"
#include "YGP/DynTrace.h"
#include "YGP/ANumeric.h"

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


namespace XGP {


/**Class describing the columns in the TraceObject list
 */
class TraceObjColumns : public Gtk::TreeModel::ColumnRecord {
 public:
   TraceObjColumns () : Gtk::TreeModel::ColumnRecord (), obj (), name (), level () {
      add (obj); add (name); add (level); }

   Gtk::TreeModelColumn<YGP::DynTrace*> obj;
   Gtk::TreeModelColumn<Glib::ustring>  name;
   Gtk::TreeModelColumn<int>            level;
};


//-----------------------------------------------------------------------------
/// (Default-)Constructor
//-----------------------------------------------------------------------------
TraceDlg::TraceDlg ()
   : XGP::XDialog (OKCANCEL),
     lstObjects (*manage (new Gtk::TreeView)) {
   set_title (_("Change trace-levels"));

   TraceObjColumns cols;
   Glib::RefPtr<Gtk::ListStore> mObjects (Gtk::ListStore::create (cols));

   lstObjects.set_size_request (100, 100);
   lstObjects.set_model (mObjects);

   lstObjects.append_column (_("Objects"), cols.name);
   lstObjects.append_column (_("Level"), cols.level);

   // Make name-column sort and resize-able
   Gtk::TreeViewColumn* column (lstObjects.get_column (0));
   column->set_sort_column (cols.name);
   column->set_resizable ();

   // Make level-column sort resize and editable
   Check3 (lstObjects.get_column_cell_renderer (1));
   Gtk::CellRenderer* r (lstObjects.get_column_cell_renderer (1)); Check3 (r);
   Check3 (typeid (*r) == typeid (Gtk::CellRendererText));
   Gtk::CellRendererText* rText (dynamic_cast<Gtk::CellRendererText*> (r));
   rText->property_editable () = true;
   rText->signal_edited ().connect (mem_fun (*this, &TraceDlg::levelChanged));

   column = lstObjects.get_column (1);
   column->set_sort_column (cols.level);
   column->set_resizable ();

   // Fill list with all available trace-objects
   for (std::map<const char*, YGP::DynTrace*>::const_iterator i (YGP::DynTraceManager::getObjects ().begin ());
	i != YGP::DynTraceManager::getObjects ().end (); ++i) {
      Gtk::TreeRow row (*mObjects->append ());
      Check3 (i->first); Check3 (i->second);
      TRACE1 ("Obj: " << i->second);
      row[cols.obj] = i->second;
      row[cols.name] = i->first;
      row[cols.level] = i->second->getLevel ();
   }

   get_vbox ()->pack_start (lstObjects, Gtk::PACK_EXPAND_WIDGET, 5);

   show_all_children ();
   show ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TraceDlg::~TraceDlg () {
}

//-----------------------------------------------------------------------------
/// Handling of the OK button; closes the dialog with commiting data
//-----------------------------------------------------------------------------
void TraceDlg::okEvent () {
   TRACE5 ("TraceDlg::okEvent ()");
   TraceObjColumns cols;

   for (Gtk::TreeIter i (lstObjects.get_model ()->children ().begin ());
	i != lstObjects.get_model ()->children ().end (); ++i) {
      YGP::ANumeric nr ((int)(*i)[cols.level]);
      YGP::DynTrace* obj ((*i)[cols.obj]); Check3 (obj);
      obj->setLevel (nr);
   }
}

//-----------------------------------------------------------------------------
/// Callback after changing a value in the listbox
/// \param path Path to changed line
/// \param value New value of entry
//-----------------------------------------------------------------------------
void TraceDlg::levelChanged (const Glib::ustring& path, const Glib::ustring& value) {
   TRACE9 ("TraceDlg::valueChanged (2x const Glib::ustring&)");

   Gtk::TreeModel::Row row (*lstObjects.get_model ()->get_iter (Gtk::TreeModel::Path (path)));

   try {
      YGP::ANumeric nr (value);
      if ((nr > 9) || (nr < -1))
	 throw std::invalid_argument (_("Level must be from -1 to 9!"));

      TraceObjColumns cols;
      row[cols.level] = nr;
   }
   catch (std::exception& e) {
      Gtk::MessageDialog dlg (e.what (), Gtk::MESSAGE_ERROR);
      dlg.set_title (_("Invalid value!"));
      dlg.run ();
   }
}

}
