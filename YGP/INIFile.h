#ifndef INIFILE_H
#define INIFILE_H

//$Id: INIFile.h,v 1.8 2001/08/26 14:38:26 markus Exp $

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
// Key2=Value2
//
// [Section2]
// Key3=Value3
//
// Every key must be inside a section (which means after a section-entry). A
// section is ended with the begin of a new section.


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
#define INIFILE_READ()       _inifile_.read ();
                            

// Class to bundle attributes of an INI-file together (into a section)
class INISection {
 public:
   INISection (const char* name);
   virtual ~INISection ();

   const IAttribute* findAttribute (const std::string& name) const;
   const IAttribute* findAttribute (const char* name) const;
   void addAttribute (IAttribute& attribute) throw (std::string);

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
   std::vector<IAttribute*> attributes;

   INISection (const INISection&);
   INISection& operator= (const INISection&);

   typedef OFParseAttomic<INISection> OMParseAttomic;
   typedef OFParseText<INISection>    OMParseText;

   IAttribute* pFoundAttr;

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
