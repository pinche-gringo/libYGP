//$Id: Check.cpp,v 1.3 2002/04/28 00:18:03 markus Rel $

//PROJECT     : XGeneral
//SUBSYSTEM   : Check
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 13.9.1999
//COPYRIGHT   : Anticopyright (A) 1999

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


#include <stdio.h>

#include <iostream>

#include "XMessageBox.h"


int check (const char* expr, const char* file, unsigned int line) {
   char title[strlen (file) + 30];
   sprintf (title, "Check in %s line %u", file, line);
   if (XMessageBox::Show (expr, title, 
                          XMessageBox::CRITICAL | XMessageBox::OKCANCEL)
       != XMessageBox::OK) {
      cerr << title << ": " << expr << "\n\t-> User canceled\n";
      exit (-1);
   }

   cerr << title << ": " << expr << "\n\t-> User continued\n";
   return 0;
}
