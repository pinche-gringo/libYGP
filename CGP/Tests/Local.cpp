// $Id: Local.cpp,v 1.6 2004/01/15 06:26:40 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : CORBA/Test/Local
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.6 $
//AUTHOR      : Markus Schwab
//CREATED     : 11.7.2002
//COPYRIGHT   : Copyright (C) 2002 - 2004

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

#include <iostream>

#include <CORBA.h>

#include <YGP/Internal.h>
#include <YGP/Check.h>
#include <YGP/Trace.h>

#include <CGP/CIDirSrch.h>


int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);     // Specify messagefile for gettext
   textdomain (PACKAGE);

   CORBA::ORB_var orb = CORBA::ORB_init (argc, argv, "mico-local-orb" );

   CIDirectorySearch* srchSrv = new CIDirectorySearch (); Check3 (srchSrv);
   CORBA::String_var id = orb->object_to_string (srchSrv);

   TRACE2 ("Local: ID of created object: " << id);

   CDirectorySearch_var srchClt (CDirectorySearch::_narrow (orb->string_to_object (id)));
   Check3 (srchClt);
   srchClt->setSearchValue ("Loc*");

   unsigned int files (0);
   CFile_ptr file = (srchClt->find (CDirectorySearch::FILE_NORMAL));
   while (file) {
      TRACE1 ("Found: " << file->path () << file->name ());
      ++files;
      file = srchClt->next ();
   }

   CORBA::release (srchSrv);
   return (files != 3);
}
