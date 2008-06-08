//$Id: Version.cpp,v 1.4 2008/06/08 12:32:07 markus Rel $

//PROJECT     : General/CORBA
//SUBSYSTEM   : CDirSrch
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
//AUTHOR      : Markus Schwab
//CREATED     : 19.1.2001
//COPYRIGHT   : Copyright (C) 2001 - 2004, 2008

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

extern "C" {

static const char* Version = "\n!@$%" PACKAGE " V" VERSION
                             " Compiled on " __DATE__ "%$@!\n";

};
