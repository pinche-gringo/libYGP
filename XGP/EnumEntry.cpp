// PROJECT     : libXGP
// SUBSYSTEM   : EnumEntry
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 28.04.2005
// COPYRIGHT   : Copyright (C) 2005, 2008, 2011, 2026

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

#include <YGP/MetaEnum.h>

#include "EnumEntry.h"

namespace XGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param values MetaEnum whose values should be shown
//-----------------------------------------------------------------------------
EnumEntry::EnumEntry(const YGP::MetaEnum& values) {
    for (auto i(values.begin()); i != values.end(); ++i)
        append(i->second);
    // set_active (0);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
EnumEntry::~EnumEntry() = default;

} // namespace XGP
