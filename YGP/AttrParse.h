#ifndef ATTRPARSE_H
#define ATTRPARSE_H

//$Id: AttrParse.h,v 1.5 2002/12/01 08:37:42 markus Rel $

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


// Macros to set the attribute-values to parse into the attribute-parser
#define ATTRIBUTE(APobj, type, var, name)  APobj.addAttribute (*new Attribute<type> (name, var));


// Class to parse attribute-assignments from a string and sets its values
// into attribute-values.
//
// Use this class like the following:
//
//   std::string file;
//    ANumeric    size;
//    ATimestamp  time;
//
//    AttributeParse attrs;
//    ATTRIBUTE (attrs, std::string, file, "File");
//    ATTRIBUTE (attrs, ANumeric, size, "Size");
//    ATTRIBUTE (attrs, ATimestamp, time, "Time");
//    attrs.assignValues ("File=\"ADate.cpp\";Time=01012000 121005;Size=18180");
//
// This example would assign the values "ADate.cpp" to file>, "18180" to size
// and the 1st of January 2000, 12:10:05 to time.
//
// Note that the order of the attributes while declaration and during parsing
// does not need to be the same!

class AttributeParse {
 public:
   //Section manager-functions
   AttributeParse () { }
   virtual ~AttributeParse ();

   void assignValues (const std::string& values) const throw (std::string);

   void addAttribute (IAttribute& attr);

   const IAttribute* findAttribute (const std::string& name) const;
   const IAttribute* findAttribute (const char* name) const;

 private:
   AttributeParse (const AttributeParse&);
   AttributeParse& operator= (const AttributeParse&);

   IAttribute** attrs;                 // Pointer to array holding attributes
   std::vector<IAttribute*> apAttrs;
};

#endif
