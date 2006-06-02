//$Id: Module.cpp,v 1.1 2006/06/02 20:09:47 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : YGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 25.05.2006
//COPYRIGHT   : Copyright (C) 2006

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


#include <ygp-cfg.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "Module.h"


namespace YGP {


//-----------------------------------------------------------------------------
/// Constructor from a std::string
/// \param module: Name of module to load (may contain the path; else
///                it is searched for in the default places)
/// \throw std::string: An describing text in case of an error
//-----------------------------------------------------------------------------
Module::Module (const char* module) throw (FileError) {
   TRACE3 ("Module::Module (const char*) - " << module);
   Check1 (module);

#ifdef HAVE_DLFCN_H
   if (!(hDLL = dlopen (module, RTLD_LAZY))) {
      std::string name (module);
      name += ".so";
      if (!(hDLL = dlopen (name.c_str (), RTLD_LAZY))) {
	 name = "lib" + name;
	 hDLL = dlopen (name.c_str (), RTLD_LAZY);
      }
   }
   if (!hDLL)
      throw (FileError (dlerror ()));
#else
#endif
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
Module::~Module () {
#ifdef HAVE_DLFCN_H
   dlclose (hDLL);
#endif
}


//-----------------------------------------------------------------------------
/// Returns a pointer to the function with the passed name
/// \param symbol: Name of function to find
/// \returns void*: Pointer to function or NULL
//-----------------------------------------------------------------------------
void* Module::getSymbol (const char* symbol) const {
   TRACE5 ("Module::getSymbol (const char*) const - " << symbol);
   Check1 (symbol);
#ifdef HAVE_DLFCN_H
   return dlsym (hDLL, symbol);
#endif
}

}
