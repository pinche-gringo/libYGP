#ifndef INIFILE_H
#define INIFILE_H

//$Id: INIFile.h,v 1.11 2002/09/13 04:43:00 markus Exp $

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


// Class to handle INI-files (containing settings for an application).
// This INI-files have a semantic similar to those in OS/2 (and M$ Windoze),
// which means the consist of sections having keys and values.
// The structure is like this
//
// [Section1]
// Key1=Value1
// Key2=2
//
// [Section2]
// Key3=03012000
//
// Every key must be inside a section (which means after a section-entry). A
// section is ended with the begin of a new section.
//
// There are some predefined macros to make the generation of the
// data-structure to parse an INI-file easier. They must be used in
// that (top-down) order.
//
// INIFILE (file) 
//      Defines an object of type INIFile named _inifile_;
//        
//INISECTION (name) Defines an object of type INISection. Both the
//      defined variable and the section-name in the INI-file are called
//      name.
//
//      Note: This macro defines a variable (of type INISection) called name.
//
//INIATTR (section, type, name)
//      Defines an attribute for section section having the key (in the INI-file)
//      of name. The value of this key is assigned to a variable of type type and
//      (also) name name. 
//
//      Note: This macro defines a variable (of type INIAttribute<type>) called name_.
//
//INIATTR2 (section, type, attr, name) 
//      Defines an attribute for section section having the key (in
//      the INI-file) of name. The value of this key is assigned to a
//      variable of type type and name attr.
//
//      Note: This macro defines a variable (of type INIAttribute<type>) called name_.
//
// To parse the INI-file above use the following commands:
//
//    int Key1;
//    std::string attr2;
//    ADate Key3;
//
//    INIFILE ("Test.ini");
//    INISECTION (Section1);
//    INIATTR (Section1, int, Key1);
//    INIATTR2 (Section1, std::string, attr2, Key2);
//    INISECTION (Section2);
//    INIATTR (Section2, ADate, Key3);

#include <stdlib.h>
#include <assert.h>

#include <string>
#include <vector>
#include <stdexcept>

#include <Parse.h>
#include <XStream.h>
#include <Attribute.h>


// Macros to define the INI-file-structure.
#define INIFILE(file)       INIFile _inifile_ (file);
#define INISECTION(section) INISection section (#section); \
                            _inifile_.addSection (section);
#define INIATTR(section, type, attr) Attribute<type> attr##_ (#attr, attr); \
                             section.addAttribute (attr##_);
#define INIATTR2(section, type, attr, name) Attribute<type> name##_ (#name, attr); \
                             section.addAttribute (name##_);
#define INIFILE_READ()       _inifile_.read ()
                            

// Class to handle the information stored in a section of an INI-file.
// Usually this class is just used to bundle the attributes of a
// section together; none of their members is called directly. The
// only exception is for INI-files without any section, only with
// attributes. In that case the readAttributes-member can be used to
// parse those values (regardless of any header-information).
class INISection {
 public:
   INISection (const char* name);
   virtual ~INISection ();

   const IAttribute* findAttribute (const std::string& name) const;
   const IAttribute* findAttribute (const char* name) const;
   void addAttribute (const IAttribute& attribute);

   int readFromStream (Xistream& stream) throw (std::string);
   int readAttributes (Xistream& stream) throw (std::string);

   const char* getName () const { return pName; }

   bool matches (const char* name) const {
      assert (name);
      return !strcmp (name, pName); }

 protected:
   virtual int foundSection (const char* section);
   virtual int foundKey (const char* key);
   virtual int foundValue (const char* value);

 private:
   const char* pName;
   std::vector<const IAttribute*> attributes;

   INISection (const INISection&);
   INISection& operator= (const INISection&);

   typedef OFParseAttomic<INISection> OMParseAttomic;
   typedef OFParseText<INISection>    OMParseText;

   const IAttribute* pFoundAttr;

   // Parser-Objects
   ParseObject*   _Section[3];
   ParseSequence  Section;
   ParseObject*   _SectionHeader[4];
   ParseSequence  SectionHeader;
   ParseObject*   _Attributes[4];
   ParseSequence  Attributes;
   OMParseAttomic SectionName;
   OMParseAttomic Identifier;
   OMParseText    Value;
};


// Class to handle the information of an INI-file
class INIFile {
 public:
   INIFile (const char* filename) throw (std::string);
   virtual ~INIFile ();

   void addSection (const INISection& section);

   int read () throw (std::string);

 protected:
   virtual int foundSection (const char* section);

   const INISection* findSection (const char* name) const;

 private:
   INIFile (const INIFile&);
   INIFile& operator= (const INIFile&);

   Xifstream file;
   std::vector<const INISection*> sections;
   INISection* pSection;

   typedef OFParseAttomic<INIFile> OMParseAttomic;

   ParseObject*   _SectionHeader[4];
   ParseSequence  SectionHeader;
   OMParseAttomic SectionName;
};

#endif
