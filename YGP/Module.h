#ifndef MODULE_H
#define MODULE_H

//$Id: Module.h,v 1.2 2007/08/27 20:28:39 markus Exp $

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
