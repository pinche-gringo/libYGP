//$Id: Module.cpp,v 1.3 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : YGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 25.05.2006
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


#include <ygp-cfg.h>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include "Module.h"


namespace YGP {


//-----------------------------------------------------------------------------
/// Constructor from a std::string
/// \param module Name of module to load (may contain the path; else
///                it is searched for in the default places)
/// \throw std::string An describing text in case of an error
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
#elif defined HAVE_WINDOWS_H
   if (!(hDLL = LoadLibrary (module))) {
      int err (GetLastError ());
      char msg[128];
      FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		     NULL, err, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		     msg, sizeof (msg), NULL);
      throw FileError (msg);
   }
#endif
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
Module::~Module () {
#ifdef HAVE_DLFCN_H
   dlclose (hDLL);
#elif defined HAVE_WINDOWS_H
   FreeLibrary (hDLL);
#endif
}


//-----------------------------------------------------------------------------
/// Returns a pointer to the function with the passed name
/// \param symbol Name of function to find
/// \returns void* Pointer to function or NULL
//-----------------------------------------------------------------------------
void* Module::getSymbol (const char* symbol) const {
   TRACE5 ("Module::getSymbol (const char*) const - " << symbol);
   Check1 (symbol);
#ifdef HAVE_DLFCN_H
   return dlsym (hDLL, symbol);
#elif defined HAVE_WINDOWS_H
   return (void*)GetProcAddress (hDLL, symbol);
#else
   return NULL;
#endif
}

}
