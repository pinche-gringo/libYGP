//$Id: AttrParse.cpp,v 1.3 2001/10/12 23:05:40 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : AttributeParse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 26.8.2001
//COPYRIGHT   : Anticopyright (A) 2001

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


#define DEBUG 0
#include "Trace_.h"

#include "AssParse.h"

#include "AttrParse.h"


/*--------------------------------------------------------------------------*/
//Purpose     : Destructor
/*--------------------------------------------------------------------------*/
AttributeParse::~AttributeParse () {
   std::vector<IAttribute*>::reverse_iterator i;
   for (i = (apAttrs.rbegin ()); i != apAttrs.rend (); ++i)
      delete *i;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds an attribute to the request
//Parameters: attribute: Attribute to add
//Throws    : In the debug-versions an exception is thrown, it the attribute
//            already exists
/*--------------------------------------------------------------------------*/
void AttributeParse::addAttribute (IAttribute& attribute) throw (std::string) {
   TRACE5 ("AttributeParse::addAttribute (IAttribute&) - "
           << attribute.getName ());

#ifndef NDEBUG
   if (findAttribute (attribute.getName ()))
      throw (std::string ("Attribute '") + std::string (attribute.getName ())
             + std::string ("' already exists"));
#endif

   apAttrs.push_back (&attribute);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Tries to find an attribute with the passed name in the request
//Parameters: name: Name of attribute to find
//Returns   : IAttribute*: Pointer to attribute or NULL (if not found)
/*--------------------------------------------------------------------------*/
const IAttribute* AttributeParse::findAttribute (const char* name) const {
   std::vector<IAttribute*>::const_iterator i;
   for (i = apAttrs.begin (); i != apAttrs.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Tries to find an attribute with the passed name in the request
//Parameters: name: Name of attribute to find
//Returns   : IAttribute*: Pointer to attribute or NULL (if not found)
/*--------------------------------------------------------------------------*/
const IAttribute* AttributeParse::findAttribute (const std::string& name) const {
   std::vector<IAttribute*>::const_iterator i;
   for (i = apAttrs.begin (); i != apAttrs.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads data from a socket and tries to assign the values
//Parameters: name: Name of attribute to find
//Returns   : IAttribute*: Pointer to attribute or NULL (if not found)
/*--------------------------------------------------------------------------*/
int AttributeParse::assignValues (const std::string& values) const throw (std::string) {
   TRACE9 ("AttributeParse::assignValues (const std::string&) - " << values);
   AssignmentParse ass (values);

   std::string node;
   while (!((node = ass.getNextNode ()).empty ())) {
      // Try to find the key
      TRACE6 ("AttributeParse::assignValues (const std::string&) - Search for key "
              << ass.getActKey ());

      IAttribute* attr (const_cast <IAttribute*> (findAttribute (ass.getActKey ())));
      if (attr) {
	 std::string value (ass.getActValue ());

         TRACE5 ("AttributeParse::assignValues (const std::string&) - Assigning "
                 << value << " (" << value.length () << ')');
         if (!attr->assign (value.c_str (), value.length ())) {
            std::string error ("Error assigning '" + value
                               + "' to " + ass.getActKey ());
            throw (error);
         }
      } // endif
      else {
         std::string error ("Key '" + ass.getActKey () + "' not found");
         throw (error);
      }
   } // end-while
}
