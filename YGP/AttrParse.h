#ifndef ATTRPARSE_H
#define ATTRPARSE_H

//$Id: AttrParse.h,v 1.1 2001/08/28 20:16:08 markus Exp $

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


#include <string>
#include <vector>

#include <Attribute.h>


// Macros to set the attributes of the request
#define ATTRIBUTE(req, type, var, name)  req.addAttribute (*new Attribute<type> (name, var));


// Class to parse attribute-assignments from a string and sets its values
// into attributes
class AttributeParse {
 public:
   //Section manager-functions
   AttributeParse () { }
   virtual ~AttributeParse ();

   int assignValues (const std::string& values) throw (std::string);

   void addAttribute (IAttribute& attr) throw (std::string);

   const IAttribute* findAttribute (const std::string& name) const;
   const IAttribute* findAttribute (const char* name) const;


 protected:
   const IAttribute* findAttribute () const;

 private:
   AttributeParse (const AttributeParse&);
   AttributeParse& operator= (const AttributeParse&);

   IAttribute** attrs;                 // Pointer to array holding attributes
   std::vector<IAttribute*> apAttrs;
};

#endif
