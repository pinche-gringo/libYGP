//$Id: AttrParse.cpp,v 1.19 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : AttributeParse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.19 $
//AUTHOR      : Markus Schwab
//CREATED     : 26.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2006, 2008

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


#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Internal.h"
#include "YGP/AssParse.h"

#include "YGP/AttrParse.h"


namespace YGP {

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
AttributeParse::~AttributeParse () {
   std::vector<IAttribute*>::reverse_iterator i;
   for (i = (apAttrs.rbegin ()); i != apAttrs.rend (); ++i)
      delete *i;
}


//----------------------------------------------------------------------------
/// Adds an attribute to the request
/// \param attribute Attribute to add
/// \remark In the debug version a message is shown, if the attribute already exists
//----------------------------------------------------------------------------
void AttributeParse::addAttribute (IAttribute& attribute) {
   TRACE5 ("AttributeParse::addAttribute (IAttribute&) - "
           << attribute.getName ());

#if CHECK > 1
   if (findAttribute (attribute.getName ())) {
      std::string error (_("Attribute '%1' already exists"));
      error.replace (error.find ("%1"), 2, attribute.getName ());
      CheckMsg (0, error.c_str ());
   }
#endif

   apAttrs.push_back (&attribute);
}

//----------------------------------------------------------------------------
/// Searches for an attribute with the passed name.
/// \param name Name of attribute to find
/// \return IAttribute* Pointer to attribute or NULL (if not found)
//----------------------------------------------------------------------------
const IAttribute* AttributeParse::findAttribute (const char* name) const {
   std::vector<IAttribute*>::const_iterator i;
   for (i = apAttrs.begin (); i != apAttrs.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

//----------------------------------------------------------------------------
/// Searches for an attribute with the passed name.
/// \param name Name of attribute to find
/// \return IAttribute* Pointer to attribute or NULL (if not found)
//----------------------------------------------------------------------------
const IAttribute* AttributeParse::findAttribute (const std::string& name) const {
   std::vector<IAttribute*>::const_iterator i;
   for (i = apAttrs.begin (); i != apAttrs.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

//----------------------------------------------------------------------------
/// Assigns the values from the passed string to the attribute-values stored
/// inside the object. If the name does not match any of the attributes or the
/// value does not fit to the type, an exception is thrown.
/// \param values Name of attribute to find
/// \throw YGP::ParseError_argument in case of an unknown name or an invalid value
//----------------------------------------------------------------------------
void AttributeParse::assignValues (const std::string& values) const throw (YGP::ParseError) {
   TRACE9 ("AttributeParse::assignValues (const std::string&) - " << values);
   AssignmentParse ass (values);

   std::string node;
   while (!((node = ass.getNextNode ()).empty ())) {
      // Try to find the key
      TRACE6 ("AttributeParse::assignValues (const std::string&) - Search for key "
              << ass.getActKey ());

      IAttribute* attr = const_cast<IAttribute*> (findAttribute (ass.getActKey ()));
      if (attr) {
	 std::string value (ass.getActValue ());

         TRACE5 ("AttributeParse::assignValues (const std::string&) - Assigning "
                 << value << " (" << value.length () << ')');
         if (!attr->assign (value.c_str (), value.length ())) {
            std::string error (_("Error assigning '%1' to %2"));
            error.replace (error.find ("%1"), 2, value);
            error.replace (error.find ("%2"), 2, ass.getActKey ());
            throw (YGP::ParseError (error));
         }
      } // endif
      else {
         std::string error (_("Key '%1' not found"));
         error.replace (error.find ("%1"), 2, ass.getActKey ());
	 throw (YGP::ParseError (error));
      }
   } // end-while
}

}
