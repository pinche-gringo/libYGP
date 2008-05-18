#ifndef YGP_ATTRPARSE_H
#define YGP_ATTRPARSE_H

//$Id: AttrParse.h,v 1.15 2008/05/18 13:21:27 markus Rel $

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


#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif

#include <string>
#include <vector>

#include <YGP/Exception.h>
#include <YGP/Attribute.h>


namespace YGP {

// Macros to set the attribute-values to parse into the attribute-parser
#define ATTRIBUTE(APobj, type, var, name)  APobj.addAttribute (*new YGP::Attribute<type> (name, var));
#define MEATTRIBUTE(APobj, meobj, var, name)  APobj.addAttribute (*new YGP::MetaEnumAttribute (name, meobj, var));


/**Class to parse attribute-assignments from a string and sets its values
 into attribute-values.

 Use this class like the following:

\verbatim    std::string file;
    ANumeric    size;
    ATimestamp  time;

    AttributeParse attrs;
    ATTRIBUTE (attrs, std::string, file, "File");
    ATTRIBUTE (attrs, ANumeric, size, "Size");
    ATTRIBUTE (attrs, ATimestamp, time, "Time");

    try {
       attrs.assignValues ("File=\"ADate.cpp\";Time=01012000 121005;Size=18180");
    }
    catch (YGP::ParseError& e) {
       // Errorhandling
    }
\endverbatim

 This example would assign the values \c ADate.cpp to \c file, \c 18180 to \c
 size and the <tt>1st of January 2000, 12:10:05</tt> to \c time.

 MEATTRIBUTE defines an attribute whose values are restricted to the
 values in the respective YGP::MetaEnum.

 \note Tthe order of the attributes while declaration and during parsing
       does not need to be the same!
*/
class AttributeParse {
 public:
   //Section manager-functions
   /// Default constructor; creates an empty object
   AttributeParse () : apAttrs () { }
   virtual ~AttributeParse ();

   void assignValues (const std::string& values) const throw (YGP::ParseError);

   void addAttribute (IAttribute& attr);

   const IAttribute* findAttribute (const std::string& name) const;
   const IAttribute* findAttribute (const char* name) const;

 private:
   AttributeParse (const AttributeParse&);
   AttributeParse& operator= (const AttributeParse&);

   std::vector<IAttribute*> apAttrs;   // Pointer to vector holding attributes
};

}

#endif
