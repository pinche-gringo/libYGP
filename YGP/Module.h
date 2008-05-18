#ifndef YGP_MODULE_H
#define YGP_MODULE_H

//$Id: Module.h,v 1.4 2008/05/18 13:21:27 markus Rel $

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

#ifdef HAVE_DLFCN_H
#  include <dlfcn.h>
#elif defined HAVE_WINDOWS_H
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif


#include <YGP/Exception.h>

namespace YGP {

/**Dynamically loading of modules.
 */
class Module {
 public:
   Module (const char* module) throw (FileError);
   virtual ~Module ();

   void* getSymbol (const char* symbol) const;

 private:
   Module ();
   Module (const Module& other);
   const Module& operator= (const Module& other);

#ifdef HAVE_DLFCN_H
   void* hDLL;
#elif defined HAVE_WINDOWS_H
   HMODULE hDLL;
#endif
};

}

#endif
