// $Id: Remote.cpp,v 1.2 2002/07/15 21:00:18 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : CORBA/Test/Remote
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.2 $
//AUTHOR      : Markus Schwab
//CREATED     : 11.7.2002
//COPYRIGHT   : Anticopyright (A) 2002

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

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>

#include <CORBA.h>

#include <Internal.h>

#include <Trace_.h>

#include <CIDirSrch.h>


int main (int argc, char* argv[]) {
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);     // Specify messagefile for gettext
   textdomain (PACKAGE);

   // Don't perform any shared code in the old parent as the whole test should
   // be between two different programs; so pass the id of the object via a pipe
   int aiPipe[2];

   if (pipe (aiPipe)) {
      cerr << "Remote: Can't create pipe\n";
      return -1;
   }

   switch (fork ()) {
   case 0: {                                                     // Child-part
      close (aiPipe[1]);

      char id[1024];
      int bytes = read (aiPipe[0], id, sizeof (id) - 1);
      id[bytes] = '\0';
      TRACE2 ("Remote (client): ID of created object: '" << id << '\'');
      close (aiPipe[0]);

      CORBA::ORB_var orb = CORBA::ORB_init (argc, argv, "mico-local-orb" );

      CORBA::Object_var cltObj (orb->string_to_object (id));
      assert (cltObj);
      CDirectorySearch_var srchClt (CDirectorySearch::_narrow (cltObj));
      assert (srchClt);
      srchClt->setSearchValue ("Rem*");

      unsigned int files (0);
      CFile_var file = (srchClt->find (CDirectorySearch::FILE_NORMAL));
      while (file) {
         CORBA::String_var id = orb->object_to_string (file);
         TRACE2 ("Remote (client): ID of found file: " << id);

         TRACE1 ("Found: " << file->path () << file->name ());
         ++files;
         file = srchClt->next ();
      }

      srchClt->exit ();
      return files != 3;
      }
      break;

   case -1:
      TRACE ("Remote: Can't create child process");
      return -2;

   default: {
      close (aiPipe[0]);

      CORBA::ORB_var orb (CORBA::ORB_init (argc, argv, "mico-local-orb" ));
      CORBA::BOA_var boa (orb->BOA_init (argc, argv, "mico-local-boa" ));

      CIDirectorySearch* srchSrv = new CIDirectorySearch (); assert (srchSrv);
      boa->impl_is_ready (CORBA::ImplementationDef::_nil ());

      CORBA::String_var id = orb->object_to_string (srchSrv);

      TRACE2 ("Remote (parent): ID of created object: '" << id << '\'');

      FILE* stream = fdopen (aiPipe[1], "w");
      fputs (id, stream);
      fclose (stream);

      orb->run ();
      CORBA::release (srchSrv);
      }
   } // end-switch

   return 0;
}
